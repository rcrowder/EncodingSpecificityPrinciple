% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/03/10 16:08:18 $
% $Revision: 1.1 $
%%%%%%%%%%%%%
% the parameters for the graphics.
% They are independent from the module parameters

irinosai.minimum_time=0.001;
irinosai.maximum_time=0.035;
irinosai.is_log=1;
irinosai.time_reversed=1;
irinosai.display_time=0;
irinosai.time_profile_scale=1;
irinosai.plotstyle='waterfall';
irinosai.colormap='summer';
irinosai.colorbar='off';
irinosai.viewpoint=[0 80];
irinosai.camlight=[50,30;0,90]; % several different possible
irinosai.lighting='phong';
irinosai.shiftcolormap=0.8; % a linear shift of the colormap towards higher numbers (if >0.5) or towards lower numbers