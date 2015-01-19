% tool
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
% $Revision: 1.4 $

function is_there=fexist(datei)
%usage: is_there=fexist(datei)
% returns a boolean (1 or 0) whether the requested datei exist or not

is_there=0;


try 
	nr=exist(datei);
catch 
	is_there=0;
	return;
end

% is a normal file:
if nr==2
	is_there=1;
	return;
end

% is a directory:
if nr==7
	is_there=1;
	return;
end


% 
% 
% FID = -1;
% FID = fopen(datei,'r');
% if FID ~=-1
%     is_there=1;
%     fclose(FID);
%     return
% end
% 
% % if still here, it can be a directory
% % try to find the name in the list
% dir_struct = dir;
% [sorted_names,sorted_index] = sortrows({dir_struct.name}');
% 
% for i=1:length(sorted_index)
%     if strcmp(sorted_names(i),datei)
%         is_there=1;
%         return
%     end
% end
% is_there=0;
% 