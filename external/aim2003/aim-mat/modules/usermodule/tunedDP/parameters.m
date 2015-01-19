%%%%%%%%%%%%%
% usermodules
tuneddp.generatingfunction='gentunedDP';
tuneddp.displayname='tunedDP';
tuneddp.displayfunction='displaytunedDP';
tuneddp.revision='$Revision: 1.3 $';

tuneddp.scalefactor = 2.335;      % scale fqp - not used
tuneddp.dynamic_threshold_TIP=0;  % 0 = off  TimeIntervalProvile not used 
tuneddp.smooth_sigma_TIP = 3;     % 0 = off  smooth TIP to avoid the detection of
                                  % small maxima as peaks
tuneddp.dynamic_threshold_FP=0;   % 0 = off  FrequencyProfile not used
tuneddp.smooth_sigma_FP = 0;      % 0 = off  smoothes FQP
tuneddp.NoFramestoAverage=40;     % Profiles are average of last ... frames 
tuneddp.tuningFactor=1;           % not used

                                  % give a priori information where to find
                                  % the peak
tuneddp.aprioriTIPpeakfq=-1;         % -1 = off
tuneddp.aprioriFQPpeakfq=-1;         % -1 = off