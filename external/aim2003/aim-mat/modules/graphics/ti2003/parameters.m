% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/09/03 16:36:18 $
% $Revision: 1.8 $
%%%%%%%%%%%%%
% the parameters for the graphics.
% They are independent from the module parameters

ti2003.minimum_time=0.001;
ti2003.maximum_time=0.035;
ti2003.is_log=1;
ti2003.time_reversed=1;
ti2003.display_time=0;
ti2003.time_profile_scale=1;
ti2003.plotstyle='waterfall';
ti2003.colormap='summer';
ti2003.colorbar='off';
ti2003.viewpoint=[0 80];
ti2003.camlight=[50,30;0,90]; % several different possible
ti2003.lighting='phong';
ti2003.shiftcolormap=0.8; % a linear shift of the colormap towards higher numbers (if >0.5) or towards lower numbers