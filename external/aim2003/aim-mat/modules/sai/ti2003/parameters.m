% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/09/03 16:35:39 $
% $Revision: 1.14 $
%%%%%%%%%%%%%
% sai
% hidden parameters
ti2003.generatingfunction='gen_ti2003';
ti2003.displayname='time integration stabilized auditory image on several sources';
ti2003.revision='$Revision: 1.14 $';

% parameters relevant for the calculation of this module
ti2003.criterion='change_weights'; % can be 'integrate_erbs','change_weights'
% relevant for all criterions:
ti2003.mindelay=0.0005;
ti2003.maxdelay=0.035;
ti2003.buffer_memory_decay=0.03;
ti2003.frames_per_second=200;

ti2003.weight_threshold=0.0; 	% when strobe weight drops under this threshold, forget it!
ti2003.do_normalize=1; % yes, strobes are normalized to a weight of 1
ti2003.do_times_nap_height=0; % no, nap height is not multiplied per default
ti2003.do_adjust_weights=1;	% yep, the weights are changed by the following parameter
ti2003.strobe_weight_alpha=0.5; % the factor by which the strobe weight decreases
ti2003.delay_weight_change=0.5; % change the weights after this time


ti2003.do_click_reduction=0;
ti2003.click_reduction_sai='click_frame.mat';


