% procedure for 'aim-mat'
% 
% function handles=do_aim_calculate(handles)
%
%   INPUT VALUES:
%   RETURN VALUE:
%     a new unique movie name
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/02/24 10:56:22 $
% $Revision: 1.1 $


function new_name=generate_new_movie_name(handles)
% looks through the directory and decides, what name a new movie should get

dirname=handles.info.directoryname;
cd(dirname);

allmovies=dir('*.mov');
nr_movies=length(allmovies);
newnumber=nr_movies+1;
new_name=sprintf('%s.movie%d.mov',handles.info.uniqueworkingname,newnumber);
cd ..
