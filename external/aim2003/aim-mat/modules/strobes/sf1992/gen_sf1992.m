% generating function for 'aim-mat'
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/02/25 18:42:03 $
% $Revision: 1.5 $

function [allstrobeprocesses,allthresholds]=gen_sf1992(nap,strobeoptions)

% this is the same function as sf2003, only the strobe criterium is changed
[allstrobeprocesses,allthresholds]=gen_sf2003(nap,strobeoptions);




% % find in each channel each strobe and gives it back as an list of
% % structures of strobes
% 
% waithand=waitbar(0,'generating strobes');
% % % if not all channels are wanted to be seen
% % 
% allthresholds=nap;
% cfs=getcf(nap);
% 
% 
% % switch strobeoptions.strobe_criterion
% % 	case 'threshold'
% % 	case 'peak'
% % 	case 'temporal_shadow'
% % 	case 'temporal_shadow_plus'
% % 	case 'delta_gamma'
% % 	case 'parabola'
% % 	case 'bunt'
% % 	case 'adaptive'
% % end
% 
% % strobeoptions.strobe_criterion='peak';
% % strobeoptions.strobe_criterion='temporal_shadow';
% % strobeoptions.strobe_criterion='temporal_shadow_plus';
% 
% 
% nr_channels=getnrchannels(nap);
% for ii=1:nr_channels
%     waitbar(ii/nr_channels);
%     single_channel=getsinglechannel(nap,ii);
%     current_cf=cfs(ii);
% 
%     [strobe_points,threshold]=findstrobes(single_channel,strobeoptions);
%     strobe_vals=gettimevalue(single_channel,strobe_points);
%     
%     thresvals=getvalues(threshold);
% 
% 	if size(strobe_points,1) > size(strobe_points,2)
% 		strobe_points=strobe_points';
% 		strobe_vals=strobe_vals';
% 	end
%     allstrobeprocesses{ii}.strobes=strobe_points;
%     allstrobeprocesses{ii}.strobe_vals=strobe_vals;
%     allthresholds=setsinglechannel(allthresholds,ii,thresvals);
% end
% close(waithand);