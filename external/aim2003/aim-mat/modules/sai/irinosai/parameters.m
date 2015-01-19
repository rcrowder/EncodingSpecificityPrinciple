% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/03/10 15:13:15 $
% $Revision: 1.1 $
%%%%%%%%%%%%%
% sai
% hidden parameters
irinosai.generatingfunction='genirinosai';
irinosai.displayname='time integration stabilized auditory image';
irinosai.revision='$Revision: 1.1 $';

% parameters relevant for the calculation of this module
irinosai.start_time=0;
irinosai.maxdelay=0.035;
irinosai.buffer_memory_decay=0.03;
irinosai.frames_per_second=200;
irinosai.delay_time_strobe_weight_decay=0.02;

% ti2003.display_logarithmic=1;
% ti2003.display_reverse=1;
% ti2003.strobe_weight_alpha=1;
% ti2003.phase_adjustment=0.5;
% ti2003.mindelay_in_cycles=1.5;
% ti2003.weightthreshold=0.001;
