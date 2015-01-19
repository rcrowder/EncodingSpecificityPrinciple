% generating function for 'aim-mat'
%function returnframes=gen_ti2003(nap,strobes,options)
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% time integration
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/03/20 17:41:37 $
% $Revision: 1.8 $

function returnframes=genirinosai(NAP,strobes,options)
% calculates the stablized image from the data given in options



%function [SAI3d, RAI3d, SAIparam, StrobeInfo] ...
%        = CalSAIstinfo(NAPPhsCmp,StrobeInfo,SAIparam);

%grab the sampling rate
fs  = getsr(NAP);   

%read in all of the frames in the NAP array - converting them into Irino's
%2d matrix
NAPPhsCmp(:,:)=getvalues(NAP);
[a, no_channels]=size(strobes);
for ii=1:no_channels
    StrobeInfo.NAPpoint(ii,:)=(strobes{ii}.strobes)*fs;
end;


%find the array locations of the strobe points
%close to what Irino does but not indentical
StrobeInfo.EventLoc=StrobeInfo.NAPpoint(1,:);
StrobeInfo.EventLoc(1)=0;

%%%%% Parameter setting %%%%
SAIparam.Nwidth     = 0;   % Negative width of auditory image (ms).
SAIparam.Pwidth     = 35;   % Positive width of auditory image (ms).
SAIparam.NAPdecay   = -2.5; % NAP (input) decay rate (%/ms)
SAIparam.FrstepAID  = 0;    % default : Event base 
SAIparam.SwSmthWin = 1;  % Switch of Smoothing Window
SAIparam.ImageDecay = 0;
nExtDur = [];
SAIparam.CoefSmthWin = hanning(5); %hanning window
SAIparam.CoefSmthWin  = SAIparam.CoefSmthWin/sum(SAIparam.CoefSmthWin);

% if StrobeInfo.NAPpoint is missing, then Reproduce NAPpoint.
% Max Loc Error is about 100 sample point. Error in MI3d is about 1.5%
% check on 16 Oct 2002
%

% disp('*** SAI Calculation ***');
% disp(SAIparam)

%%%%% Initialize %%%%
[NumCh, LenNAP]	= size(NAPPhsCmp);
LenNwid       	= abs(SAIparam.Nwidth)*fs/1000;
LenPwid       	= abs(SAIparam.Pwidth)*fs/1000;
LenSAI        	= LenPwid +LenNwid +1;
NAPdecayWin   	= 1 + (SAIparam.NAPdecay/100/fs*1000)*(-LenNwid:LenPwid);
NAPdecayWin   	= ones(NumCh,1)*NAPdecayWin;
if SAIparam.ImageDecay > 0,
    SAIdecayConst 	= 1 - log(2)/(SAIparam.ImageDecay*fs/1000); % ^ (sample point)
end;
RawAI 		= zeros(NumCh,LenSAI);
SAI   		= zeros(NumCh,LenSAI);

LenStrb = length(StrobeInfo.EventLoc);
if SAIparam.FrstepAID > 0, LenFrame = fix(LenNAP/(SAIparam.FrstepAID*fs/1000));
else                       LenFrame = LenStrb; % event base
end;
FrameTiming 	= (1:LenFrame)*SAIparam.FrstepAID*fs/1000;
SAI3d 		= zeros(NumCh,LenSAI,LenFrame);
RAI3d 		= zeros(NumCh,LenSAI,LenFrame);
IntgrPoint	= zeros(1,LenStrb);
dIntgrPoint	= zeros(1,LenStrb); 
MeandIP0	= 8*fs/1000; % setting to 8 ms period 

if SAIparam.FrstepAID == 0, %%%%%  Event Base %%%%
    
    for nst = 1:LenStrb
        for nch = 1:NumCh
            if length(nExtDur) > 0 
                nrng = StrobeInfo.EventLoc(nst) + nExtDur;
                nrng = min( LenNAP, max(1,nrng));
                [val Npeak] = max(NAPPhsCmp(nch,nrng));
                nstp =  min(nrng)+Npeak-1;
                StrobeInfo.NAPpoint(nch,nst) = nstp;
            end;
            npt = StrobeInfo.NAPpoint(nch,nst) +(-LenNwid:LenPwid);
            npt=int16(npt); %added to fix an not understood bug
            npt = min(LenNAP,max(1,npt));
            RawAI(nch,:) = NAPPhsCmp(nch,npt); % Raw SAI
        end;
        
        IntgrPoint(nst) = max(StrobeInfo.NAPpoint(:,nst))+1; % in sample point
        dIntgrPoint(nst) =  IntgrPoint(nst) - IntgrPoint(max(1,nst-1));
        MeandIP = mean(dIntgrPoint(max(1,nst-10):nst)); % last 10 strobe
        % MeandIP = MeandIP0; % fixed --> NG level difference. 13 Jun 2002
        
        RAI3d(:,:,nst) = RawAI;
        
        if SAIparam.ImageDecay > 0
            AmpDecay = SAIdecayConst^dIntgrPoint(nst);	
            AmpIntgr = min(MeandIP/MeandIP0,3)*NAPdecayWin;
            SAI = AmpDecay * SAI  + AmpIntgr .*  RawAI; % 12 June 2002
            SAI3d(:,:,nst) = SAI;
        end;
        
        % 	if rem(nst, 50) == 0 | nst == LenFrame
        %          disp(['SAI-STI Event-Base Frame #' ...
        % 		int2str(nst) '/#' int2str(LenFrame) ':  ' ...
        % 		'elapsed_time = ' num2str(toc,3) ' (sec)']);
        %    	end;
    end;
    
    % size(StrobeInfo.NAPpoint)
    
    if SAIparam.ImageDecay == 0 & SAIparam.SwSmthWin == 0
        %      disp('Set SAI3d == RAI3d');
        SAI3d = RAI3d;
        return;
    end;
    
    %%% Smoothing with window %%%
    if SAIparam.ImageDecay == 0
        for nst = 1:LenStrb
            SAI = zeros(NumCh,LenSAI);
            LenCSW = length(SAIparam.CoefSmthWin);
            for nwin = -fix(LenCSW/2):fix(LenCSW/2)
                ncf = nwin+fix(LenCSW/2)+1;
                SAI = SAI + SAIparam.CoefSmthWin(ncf) ...
                    * RAI3d(:,:,min(max(nst+nwin,1), LenStrb));
            end;
            SAI3d(:,:,nst) = SAI;
        end;
    end;
    
    %output to frames
    no_frames=size(SAI3d);
    for jj=1:no_frames(3);
        returnframes{1,jj}=frame(SAI3d(:,:,jj));
    end;
    
    %check whether it uses the last bit- doesn't for this example
    
    IntgrPoint(nst) = max(StrobeInfo.NAPpoint(:,nst))+1; % in sample point
    dIntgrPoint(nst) =  IntgrPoint(nst) - IntgrPoint(max(1,nst-1));
    MeandIP = mean(dIntgrPoint(max(1,nst-10):nst)); % last 10 strobe
    % MeandIP = MeandIP0; % fixed --> NG level difference. 13 Jun 2002
    
    RAI3d(:,:,nst) = RawAI;
    
    if SAIparam.ImageDecay > 0
        AmpDecay = SAIdecayConst^dIntgrPoint(nst);	
        AmpIntgr = min(MeandIP/MeandIP0,3)*NAPdecayWin;
        SAI = AmpDecay * SAI  + AmpIntgr .*  RawAI; % 12 June 2002
        SAI3d(:,:,nst) = SAI;
    end;
    
    if rem(nst, 50) == 0 | nst == LenFrame
        disp(['SAI-STI Event-Base Frame #' ...
                int2str(nst) '/#' int2str(LenFrame) ':  ' ...
                'elapsed_time = ' num2str(toc,3) ' (sec)']);
    end;
end;

% size(StrobeInfo.NAPpoint)

if SAIparam.ImageDecay == 0 & SAIparam.SwSmthWin == 0
    disp('Set SAI3d == RAI3d');
    SAI3d = RAI3d;
    return;
end;

%%% Smoothing with window %%%
if SAIparam.ImageDecay == 0
    for nst = 1:LenStrb
        SAI = zeros(NumCh,LenSAI);
        LenCSW = length(SAIparam.CoefSmthWin);
        for nwin = -fix(LenCSW/2):fix(LenCSW/2)
            ncf = nwin+fix(LenCSW/2)+1;
            SAI = SAI + SAIparam.CoefSmthWin(ncf) ...
                * RAI3d(:,:,min(max(nst+nwin,1), LenStrb));
        end;
        SAI3d(:,:,nst) = SAI;
    end;
end;

%rich's thresholding experiment
%actually seems to make very little difference
no_frames=size(SAI3d);
% threshold=0; %value below which to chuck stuff
% for jj=1:no_frames(1);
% for kk=1:no_frames(2);
% for ll=1:no_frames(3);
%     if SAI3d(jj,kk,ll)<threshold
%         SAI3d(jj,kk,ll)=0;
%     end;
% end;
% end;
% end;

%output to frames
for jj=1:no_frames(3);
    returnframes{1,jj}=frame(SAI3d(:,:,jj));
    
    %set the sample rate of the frames
    returnframes{1,jj}=setsr(returnframes{1,jj},fs);
end;



% %check whether it uses the last bit- doesn't for this example
% 
% else   %%%%%%%%%%% if SAIparam.FrstepAID > 0, %%%%%  Constant Frame Base %%%%
%     
%     %appears to be no point in the following code 
%     error('Constant Frame Base. I am not sure it is maintained. 12 Jun 2002');
%     IntgrPoint(1) = 1;
%     nfr = 0;
%     
%     for nst = 2:LenStrb
%         for nch = 1:NumCh
%             npt = StrobeInfo.NAPpoint(nch,nst) +(-LenNwid:LenPwid);
%             npt = min(LenNAP,max(1,npt));
%             RawAI(nch,:) = NAPPhsCmp(nch,npt);
%         end;
%         
%         IntgrPoint(nst) = max(StrobeInfo.NAPpoint(:,nst))+1;
%         
%         nkk = find( FrameTiming >= IntgrPoint(nst-1) & ...
%             FrameTiming <  IntgrPoint(nst));
%         if nst == LenStrb, nkk2 = find(FrameTiming >= IntgrPoint(LenStrb)); 
%         else		   nkk2 = [];
%         end;
%         
%         for nn = [nkk nkk2],
%             nfr = nfr+1;
%             dTim  =  FrameTiming(nn) - IntgrPoint(nst-1);
%             SAI3d(:,:,nfr) = SAI * ( SAIdecayConst^dTim);
%             RAI3d(:,:,nfr) = RawAI;
%             SwOn(nfr) = nfr;
%             
%             if rem(nfr, 50) == 0 | nfr == LenFrame
%                 disp(['SAI-STI ' int2str(SAIparam.FrstepAID) ...
%                         '-ms Uniform Frame #' int2str(nfr) '/#' int2str(LenFrame) ':  ' ...
%                         'elapsed_time = ' num2str(toc,3) ' (sec)']);
%             end;
%         end;
%         
%         %%% Add in here %%%
%         dIntgrPoint = IntgrPoint(nst)-IntgrPoint(nst-1);
%         SAI = SAI*( SAIdecayConst^dIntgrPoint) + RawAI.*NAPdecayWin;
%     end;
%     
% end;   %%%%%%%%%%% if SAIparam.FrstepAID > 0, %%%%%  


nr_channels=getnrchannels(NAP);
% translate the arrays from data to frames
nrfrms=length(returnframes);
start_time=getminimumtime(NAP);	% start time of first frame
% if nrfrms>1
% 	interval=output_times(2)-output_times(1);
% else
% 	interval=0; % doesnt matter...
% end
interval=1/options.frames_per_second;
maxval=-inf;
maxsumval=-inf;
maxfreval=-inf;
cfs=getcf(NAP);

if nr_channels>1
    for ii=1:nrfrms
        cfr=frame(returnframes{ii});
        maxv=getmaximumvalue(cfr);
        maxval=max([maxv maxval]);
        maxs=max(getsum(cfr));
        maxsumval=max([maxs maxsumval]);
        maxf=max(getfrequencysum(cfr));
        maxfreval=max([maxf maxfreval]);
    end
    
else
    maxfreval=1;
    maxsumval=1;
    maxval=1;
end

sr=getsr(NAP);
for ii=1:nrfrms
    cfr=frame(returnframes{ii});
    cfr=setsr(cfr,sr);
    cfr=setcurrentframenumber(cfr,ii);
    cfr=setcurrentframestarttime(cfr,start_time);
    cfr=setcf(cfr,cfs);
    start_time=start_time+interval;
    cfr=setscalesumme(cfr,maxsumval);
    cfr=setallmaxvalue(cfr,maxval);
    cfr=setallminvalue(cfr,0);
    cfr=setscalefrequency(cfr,maxfreval);
    returnframes{ii}=cfr;
end
% 
% if nr_channels>1
% 	close(waithand);
% end


return;
