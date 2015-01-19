% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/07/28 14:20:21 $
% $Revision: 1.7 $
%%%%%%%%%%%%%
% the parameters for the graphics.
% They are independent from the module parameters

grouped.minimum_time=0.001;
grouped.maximum_time=0.035;
grouped.is_log=1;
grouped.time_reversed=1;
grouped.display_time=0;
grouped.time_profile_scale=1;
grouped.plotstyle='waterfall';
grouped.colormap='summer';
grouped.colorbar='off';
grouped.viewpoint=[0 80];
grouped.camlight=[50,30;0,90]; % several different possible
grouped.lighting='phong';
grouped.shiftcolormap=0.8; % a linear shift of the colormap towards higher numbers (if >0.5) or towards lower numbers