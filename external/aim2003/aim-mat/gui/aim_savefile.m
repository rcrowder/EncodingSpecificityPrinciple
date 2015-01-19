% procedure for 'aim-mat'
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/06/11 10:45:20 $
% $Revision: 1.5 $

function aim_savefile(handles,fr,name,type,modul,options,all_options)

str.type=modul;
str.data=fr;
str.options=options;
str.all_options=all_options;

eval(sprintf('%s=str;',type));

lookpath=fullfile(handles.info.original_soundfile_directory,name);

str5=sprintf('save(''%s'',''%s'');',lookpath,type);
eval(str5);

