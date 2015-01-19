% generating function for 'aim-mat'
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% calucates the autocorrleation of the nap. the strobes are not neccessary
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/07/17 10:56:16 $
% $Revision: 1.3 $

function returnframes=gen_autocorr(nap,strobes,options)




returnframes=[];
% start time of values that are returned
if isfield(options,'start_time')
	start_time=options.start_time;
else
	start_time=0;
end

% length of the resulting autocorrelation
if isfield(options,'maxdelay')
	maxdelay=options.maxdelay;
else
	maxdelay=0;
end

% how many frames per second in the output
if isfield(options,'framespersecond')
	framespersecond=options.framespersecond;
else
	framespersecond=100;
end

if ~isfield(options,'select_channels')
	options.select_channels=1:getnrchannels(nap);
end

len=getlength(nap);
output_times=1/framespersecond:1/framespersecond:len;
nr_output_times=length(output_times); % so many pictures in the end

output_counter=1;

% construct the starting SAI with zeros
sr=getsr(nap);
sampletime=1/sr;
nrdots_insai=round(options.maxdelay*sr);

signal_start_time=getminimumtime(nap);
nr_channels=getnrchannels(nap);
nr_dots=getnrpoints(nap);
current_time=signal_start_time;
times_per_ms=round(sr*0.002);
cfs=getcf(nap);
napvalues=getvalues(nap);
count=0;
tic;

saibuffer=zeros(nr_channels,nrdots_insai);
% % waithand=waitbar(0,'generating SAI','CreateCancelBtn','cancel_callback');
waithand=waitbar(0,'generating autocorrelation');


for iii=1:nr_output_times
	waitbar(iii/nr_output_times);
	current_time=output_times(iii);

	for channel_nr=1:nr_channels
		this_channel=getsinglechannel(nap,channel_nr);
		min_time=current_time-2*maxdelay; 	% begin of the correlation
		max_time=current_time;	% end of correlation

		siglen=getlength(this_channel);	% the length of this part of the nap
		
		if min_time>0 && max_time <= siglen
			this_channel_part=getpart(this_channel,min_time,max_time);
		else if min_time < 0
				this_channel_part=getpart(this_channel,0,max_time);
			else if max_time > siglen
					this_channel_part=getpart(this_channel,min_time,siglen);
				end
			end
		end
		
		corr=autocorrelate(this_channel_part,0,maxdelay);
		corvals=getvalues(corr);
		corvals=corvals(2:end)'; % skip the 0 delay and transpose
		
		saibuffer(channel_nr,:)=corvals;
	end
	resframes{iii}=saibuffer;
end


% translate the arrays from data to frames
nrfrms=length(resframes);
start_time=getminimumtime(nap);	% start time of first frame
if nrfrms>1
	interval=output_times(2)-output_times(1);
else
	interval=0; % doesnt matter...
end
maxval=-inf;
maxsumval=-inf;
maxfreval=-inf;
cfs=getcf(nap);

if nr_channels>1
	for ii=1:nrfrms
		cfr=frame(resframes{ii});
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


for ii=1:nrfrms
	cfr=frame(resframes{ii});
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

close(waithand);