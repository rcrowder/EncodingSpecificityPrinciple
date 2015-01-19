% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/02/20 14:10:18 $
% $Revision: 1.4 $
%%%%%%%%%%%%%
% sai
% hidden parameters
ti1992.generatingfunction='gen_ti1992';
ti1992.displayname='time integration stabilized auditory image old model';
ti1992.revision='$Revision: 1.4 $';

% parameters relevant for the calculation of this module
ti1992.buffer_memory_decay=0.03;
ti1992.buffer_tilt=0.04; % memory decay in the buffer
ti1992.maxdelay=0.035;
ti1992.mindelay=0.001;
ti1992.frames_per_second=100;