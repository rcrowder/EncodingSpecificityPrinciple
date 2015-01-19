% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/07/15 09:48:33 $
% $Revision: 1.11 $
%%%%%%%%%%%%%
% sai
% hidden parameters
grouped.generatingfunction='gen_grouped';
grouped.displayname='time integration stabilized auditory image on several sources';
grouped.revision='$Revision: 1.11 $';

% parameters relevant for the calculation of this module
grouped.criterion='change_weights'; % can be 'integrate_erbs','change_weights'
% relevant for all criterions:
grouped.start_time=0;
grouped.maxdelay=0.035;
grouped.buffer_memory_decay=0.03;
grouped.frames_per_second=200;

grouped.weight_threshold=0.0; 	% when strobe weight drops under this threshold, forget it!
grouped.do_normalize=1; % yes, strobes are normalized to a weight of 1
% grouped.do_times_nap_height=1; % yes, nap height is multiplied
grouped.do_adjust_weights=1;	% yep, the weights are changed by the following parameter
grouped.strobe_weight_alpha=0.5; % the factor by which the strobe weight decreases
grouped.delay_weight_change=0.5; % change the weights after this time



