%%%%%%%%%%%%%%%%%%%%%
% usermodule ssi %
%%%%%%%%%%%%%%%%%%%%%

% hidden parameters
ssi.generatingfunction = 'gen_ssi';
ssi.displayname = 'ssi Image';
% ssi.displayfunction = 'displayssi';
ssi.revision = '$Revision: 1.1 $';

% Sets the frames, the ssi image is calculated for.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% If do_all_frames = 1 
% all frames of the auditory image are transformed to a ssi image
% If do_all_frames = 0 only the frames specified in 
% framerange = [start_frame end_frame] are transformed
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
ssi.do_all_frames = 1;
ssi.framerange = [0 0];

% Sets the Range for the Auditory image
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Calculation of the ssi image for the points
% audiorange = [start_point end_point]
% if do_all_image = 1: start_point = 1, end_point = last point in the ai
% flipimage = 1 flips the auditory image (for ti1992)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
ssi.do_all_image = 1;
ssi.audiorange = [1 200];
ssi.flipimage = 0;

% Sets the variables h and c
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
ssi.c_2pi = [0:0.05:30];
ssi.TFval = [0:0.05:16]; 