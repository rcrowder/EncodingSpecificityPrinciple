% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/09/03 16:36:52 $
% $Revision: 1.8 $
%%%%%%%%%%%%%
% user defined module
% hidden parameters
bmm_movie.generatingfunction='gen_bmm_movie';
bmm_movie.displayname='produces a movie from the basilar membrane motion';
bmm_movie.revision='$Revision: 1.8 $';

% parameters relevant for the calculation of this module
% how fast the movement of the bmm should be recorded 
bmm_movie.physical_frames_per_second=1000;

% how many frames per second shell the movie have.
bmm_movie.shown_frames_per_second=20;

% how many seconds of the BMM should be shown
bmm_movie.show_bmm_duration=0.01;

% instead of the bmm we can also generate movies of the nap:
bmm_movie.use_nap_instead=0;

bmm_movie.withfre=1;
bmm_movie.withtime=1;
bmm_movie.withsignal=1;
bmm_movie.data_scale=1;
bmm_movie.movie_width=640;
bmm_movie.movie_height=400;
bmm_movie.quality=0.9;



