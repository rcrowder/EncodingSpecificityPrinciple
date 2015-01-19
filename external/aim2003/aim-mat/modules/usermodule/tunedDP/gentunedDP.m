% generating function for 'aim-mat'
%
%   How it works:
%   This function is calculating the spectral and residue pitch strength
%   for the spectral and the time-interval profile respectively.
%   It works automatically. Sometimes the 'wrong' residue peak is detected.
%   In this case change the module parameter 'aprioriTIPpeakfq' to the
%   modulation frequency in Herz and the peak at the a according position
%   is taken as residue peak
% 
%   INPUT VALUES:
%           see parameter file
%  
%   RETURN VALUE:   
%           struct
%               interval_sum                Time interval profile (signal class)
%               frequency_sum               spectral profile (signal class)
%               (for then next two see function 'PeakPickertunedDP')
%               peaks_interval_sum          peaks of the time interval profile
%               peaks_frequency_sum         peaks of spectral profile 
%               pitch_strength_prediction   result from function 'analyse_frequency_profiletunedDP'
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Christoph Lindner
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/06/27 17:52:54 $
% $Version$

function usermodule=gentunedDP(sai,options)



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

waithand = waitbar(0,'Generating dualprofile with pitch strength prediction - enhanced formular ;-)');
for frame_number=1:nr_frames
    
    waitbar(frame_number/nr_frames, waithand);     
    
	current_frame = sai{frame_number};
	current_frame = setallmaxvalue(current_frame, maxval);
	current_frame = setscalesumme(current_frame, maxsumval);
	current_frame = setscalefrequency(current_frame, maxfreval);
	
    % Average over last frames according to parameter 'NoFramestoAverage'
    usermodule{frame_number}.interval_sum =  getsum(current_frame);      
    usermodule{frame_number}.frequency_sum = getfrequencysum(current_frame);      
    startpoint=frame_number-options.NoFramestoAverage;
    if startpoint<1
        startpoint=1;
    end
    divisor = frame_number-startpoint+1;
    usermodule{frame_number}.interval_sum =  usermodule{frame_number}.interval_sum / divisor;
    usermodule{frame_number}.frequency_sum = usermodule{frame_number}.frequency_sum / divisor;
    for i=(frame_number-1):-1:startpoint
    	current_frame = sai{frame_number};
    	current_frame = setallmaxvalue(current_frame, maxval);
    	current_frame = setscalesumme(current_frame, maxsumval);
    	current_frame = setscalefrequency(current_frame, maxfreval);
        usermodule{frame_number}.interval_sum =  usermodule{frame_number}.interval_sum + getsum(current_frame)/ divisor;      
        usermodule{frame_number}.frequency_sum = usermodule{frame_number}.frequency_sum + getfrequencysum(current_frame)/ divisor;      
    end
    
    % normalise to channel number ...
    usermodule{frame_number}.interval_sum =  usermodule{frame_number}.interval_sum/getnrpoints( usermodule{frame_number}.frequency_sum);
    usermodule{frame_number}.frequency_sum = usermodule{frame_number}.frequency_sum/getnrpoints(usermodule{frame_number}.interval_sum)*options.scalefactor;
    
    % Peak Picker for linear TimeIntervalProvile
    p.dyn_thresh = options.dynamic_threshold_TIP;
    p.smooth_sigma = options.smooth_sigma_TIP;
    usermodule{frame_number}.peaks_interval_sum = PeakPickertunedDP(usermodule{frame_number}.interval_sum, p);
    % Peak Picker for FrequencyProfile
    p.dyn_thresh = options.dynamic_threshold_FP;
    p.smooth_sigma = options.smooth_sigma_FP;
    usermodule{frame_number}.peaks_frequency_sum = PeakPickertunedDP(usermodule{frame_number}.frequency_sum, p);
    usermodule{frame_number}.channel_center_fq = getcf(sai{frame_number});

    % Analyse the dualprofile
    apTIP=options.aprioriTIPpeakfq;
    apFQP=options.aprioriFQPpeakfq;
    r = analyse_aim_profilestunedDP(usermodule{frame_number}.interval_sum, ...
                                    usermodule{frame_number}.frequency_sum, ...
                                    usermodule{frame_number}.peaks_interval_sum,...
                                    usermodule{frame_number}.peaks_frequency_sum,... 
                                    usermodule{frame_number}.channel_center_fq,apTIP,apFQP);
    usermodule{frame_number}.pitch_strength_prediction = r;
    cf =getcf(sai{frame_number});
    
end
close(waithand);





