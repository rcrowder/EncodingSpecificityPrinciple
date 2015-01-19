% procedure for 'aim-mat'
% 
%   INPUT VALUES:
%       handles: 
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:45 $
% $Revision: 1.4 $

function handles=aim_deletefile(handles,type)
% deletes the file, if it is there

switch type
	case 'bmm'
		todelete=handles.info.bmmname;
		handles.info.bmm_loaded=0;
	case 'nap'
		todelete=handles.info.napname;
		handles.info.nap_loaded=0;
	case 'strobes'
		todelete=handles.info.strobesname;
		handles.info.strobes_loaded=0;
	case 'sai'
		todelete=handles.info.sainame;
		handles.info.sai_loaded=0;
	case 'usermodule'
		todelete=handles.info.usermodulename;
		handles.info.usermodule_loaded=0;
	case 'movie'
		handles.info.movie_loaded=0;
		mnames=handles.info.moviename;
		for i=1:length(mnames)
			todelete=handles.info.moviename{i};
			if ~fexist(todelete)
				return
			end
			delete(todelete);
		end
		return
end        

if ~fexist(todelete)
	return
end

delete(todelete);
