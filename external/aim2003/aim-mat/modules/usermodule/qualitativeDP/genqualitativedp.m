% generating function for 'aim-mat'
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Christoph Lindner
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/06/11 10:29:34 $
% $Version$

function usermodule=genqualitativedp(sai,options)

% find out about scaling:
maxval=-inf;
maxfreval=-inf;
maxsumval=-inf;

nr_frames=length(sai);
for ii=1:nr_frames
	maxval=max([maxval getmaximumvalue(sai{ii})]);
	maxsumval=max([maxsumval getscalesumme(sai{ii})]);
	maxfreval=max([maxfreval getscalefrequency(sai{ii})]);
end

waithand = waitbar(0,'Generating dualprofile with peak detection');
for frame_number=1:nr_frames
    
    waitbar(frame_number/nr_frames, waithand);     
    
	current_frame = sai{frame_number};
	current_frame = setallmaxvalue(current_frame, maxval);
	current_frame = setscalesumme(current_frame, maxsumval);
	current_frame = setscalefrequency(current_frame, maxfreval);
	
    usermodule{frame_number}.interval_sum =  getsum(current_frame);      
    usermodule{frame_number}.frequency_sum = getfrequencysum(current_frame);      
    % Normalisation
    usermodule{frame_number}.interval_sum =  usermodule{frame_number}.interval_sum/getnrpoints( usermodule{frame_number}.frequency_sum);
    usermodule{frame_number}.frequency_sum = usermodule{frame_number}.frequency_sum/getnrpoints(usermodule{frame_number}.interval_sum)*options.scalefactor;
    

    % Peak Picker for linear TimeIntervalProvile
    p.dyn_thresh = options.dynamic_threshold_TIP;
    p.smooth_sigma = options.smooth_sigma_TIP;
    usermodule{frame_number}.peaks_interval_sum = PeakPicker(usermodule{frame_number}.interval_sum, p);
    % Peak Picker for FrequencyProfile
    p.dyn_thresh = options.dynamic_threshold_FP;
    p.smooth_sigma = options.smooth_sigma_FP;
    usermodule{frame_number}.peaks_frequency_sum = PeakPicker(usermodule{frame_number}.frequency_sum, p);
    usermodule{frame_number}.channel_center_fq = getcf(sai{frame_number});

     
end
close(waithand);





