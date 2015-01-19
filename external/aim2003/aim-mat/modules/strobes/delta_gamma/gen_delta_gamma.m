% generating function for 'aim-mat'
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/07/09 15:24:56 $
% $Revision: 1.1 $

function [allstrobeprocesses,allthresholds]=gen_delta_gamma(nap,options)

allthresholds=nap;
cfs=getcf(nap);


integrage_octaves_above=options.integrage_octaves_above;
integrage_octaves_below=options.integrage_octaves_below;


nr_channels=getnrchannels(nap);

% first calculate, which channels must be integrated
for ii=1:nr_channels
    % 	waitbar(ii/nr_channels);
    
    current_fre=cfs(ii);
    % calculate, which channels are wanted
    min_selected_frequency=cent2fre(current_fre,-1200*integrage_octaves_below);
    max_selected_frequency=cent2fre(current_fre,+1200*integrage_octaves_below);
    % which channels are these?
    min_channel=round(fre2chan(nap,min_selected_frequency));
    if isnan(min_channel)
        min_channel=1;
    end
    max_channel=round(fre2chan(nap,max_selected_frequency));
    if isnan(max_channel)
        max_channel=nr_channels;
    end
    
    % restrict range
    a=[min_channel:max_channel];
    nr_c=length(a);
    
    neighboring_channels(ii,1:nr_c)=a;
    
    maxlr=max((max_channel-ii),(ii-min_channel));
    hamw=hamming(2*maxlr+1);
    for kk=neighboring_channels(ii,:)
        if kk>0
            dist=abs(kk-ii);
            if dist>=0 && dist <= maxlr
                neighboring_channels_weight_factor(ii,kk)=hamw(maxlr+dist+1);
            end
        end
    end
end


options.short_time_constant=0.003;
options.long_time_constant=0.03;
options.threshold_decay_constant=0.2;


% translate all napchannels in their leaky integrator lowpassed versions:
short_time_constant=options.short_time_constant;
long_time_constant=options.long_time_constant;
lowpass_cutoff_frequency=1/(2.*pi.*short_time_constant);

storenap=nap;
for jj=1:nr_channels
    cur_chan=getsinglechannel(nap,jj);
    cur_chan=leakyintegrator(cur_chan,lowpass_cutoff_frequency,1);
    nap=setsinglechannel(nap,jj,cur_chan);
end



% now the big loop over the time
current_threshold=zeros(nr_channels,1);
sr=getsr(nap);
napvals=getvalues(nap);
tresholdvals=zeros(size(napvals));
accumvals=zeros(size(napvals));
dgvals=zeros(size(napvals));


nr_dots=length(napvals);
times_per_ms=round(sr*0.005); % how often the bar should be updated
counter=ones(nr_channels,1);

% values for the fast leaky integration
short_leaky_b=exp(-1/(sr.*short_time_constant));
short_leaky_gain=1./(1-short_leaky_b);
long_leaky_b=exp(-1/(sr.*long_time_constant));
long_leaky_gain=1./(1-long_leaky_b);
leaky_yn1=zeros(1,nr_channels);
leaky_yn2=zeros(1,nr_channels);
leaky_yn3=zeros(1,nr_channels);
leaky_yn4=zeros(1,nr_channels);
% values for the derivative
last_val=zeros(1,nr_channels);
% values for threshold
last_threshold=zeros(1,nr_channels);
threshold_decay_constant=options.threshold_decay_constant;

waithand=waitbar(0,'generating Delta Gamma Strobes');

for ii=2:nr_dots-1
    current_time=ii/sr;
    if mod(ii,times_per_ms)==0
        waitbar(ii/nr_dots);
    end
    
    %     for jj=1:nr_channels
    for jj=25
        % the current value of each channel is the weighted sum of all
        % low_pass filterted channels around it
        current_val=0;
        count=0;
        %         for chn=neighboring_channels(jj,:);
        %             if chn>0
        %                 weight=neighboring_channels_weight_factor(jj,chn);
        %                 current_val=current_val+weight*napvals(chn,ii);
        %                 count=count+1;
        %             end
        %         end
        %         current_val_weighted_sum=current_val/count;
        %         current_val=current_val_weighted_sum;
        
        current_val=napvals(jj,ii);
        
        % three branches:  (Patterson,Irino,modeling temporal... 1998)
        % first branch: delta- gamma process
        % first operation: short term leaky integrator and 
        yn= current_val + short_leaky_b*leaky_yn1(jj);
        leaky_yn1(jj)=yn;
        current_val=yn/short_leaky_gain;
    dga1(ii)=current_val;
        % take the temporal derivativ:
        current_deriv=current_val-last_val(jj);
        last_val(jj)=current_val;
    dga2(ii)=current_deriv;
        
        % and another leaky integration
        current_val=current_deriv;
        yn= current_val + short_leaky_b*leaky_yn2(jj);
        leaky_yn2(jj)=yn;
        current_val=yn/short_leaky_gain;
    dga3(ii)=current_val;
        % last operation: sigmoid function 
        %         current_val=max(current_val,0);
        %         current_val=min(current_val,1);
        
        final_delta_gamma_process=current_val;
        dgvals(jj,ii)=final_delta_gamma_process;
        
        
        %         % second branch
        %         % accumulator
        %         current_val=current_val_weighted_sum;
        %         % multyply with deltagamma
        %         current_val=current_val*final_delta_gamma_process;
        %         % and another leaky integration
        %         yn= current_val + long_leaky_b*leaky_yn3(jj);
        %         leaky_yn3(jj)=yn;
        %         current_val=yn/long_leaky_gain;
        %         final_accumulator_value=current_val;
        %         accumvals(jj,ii)=final_accumulator_value;
        %         
        %         % third branch
        %         % adaptive threshold
        %         current_val=current_val_weighted_sum;
        %         threshold=current_val;
        %         % and another leaky integration
        %         yn= current_val + short_leaky_b*leaky_yn4(jj);
        %         leaky_yn4(jj)=yn;
        %         current_val=yn/short_leaky_gain;
        %         % fast peak extractor with slow decay:
        %         % threshold falls slower then 
        %         if current_val> threshold   % the threshold follows fast rise
        %             threshold=current_val;
        %         else    % but falls much slower
        %             threshold=threshold*1-(threshold_decay_constant/sr*1000);
        %         end
        %         last_threshold(jj)=threshold;
        %         thresholdvals(jj,ii)=threshold;
        %         
        %         % finally the comparator
        %         
        %         if final_accumulator_value>threshold % a new strobe!
        % 			strobe_points(jj,counter(jj))=ii;
        % 			strobe_time(jj,counter(jj))=ii/sr;
        % 			counter(jj)=counter(jj)+1;	% strobecounter
        %             % reset the accumulator:
        %             leaky_yn3(jj)=0;
        %         end
        %         
        p=1;
    end
    
end
figure(2143234);
clf
plot(napvals(jj,:));
hold on
plot(dga1,'r');
plot(dga2*100,'g');
plot(dga3*100,'m');

p=0;
return
% thres=setvalues(nap,thresholdvals);
% delga=setvalues(nap,dgvals);
% accum=setvalues(nap,accumvals);
% 
% save all_vals

for jj=1:nr_channels
    single_channel=getsinglechannel(nap,jj);
    strobe_points=strobe_points(jj,:);
    allstrobeprocesses{jj}.strobes=strobe_points;
    strobe_vals=gettimevalue(single_channel,strobe_points);
    allstrobeprocesses{jj}.strobe_vals=strobe_vals;
    allthresholds=setsinglechannel(allthresholds,jj,thresholdvals(jj,:));
end

close(waithand);


return
