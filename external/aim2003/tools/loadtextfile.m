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
% $Date: 2003/01/17 16:57:44 $
% $Revision: 1.3 $

function str=loadtextfile(name)
% usage: str=loadtextfile(pfad,name)
% loads the pfad,name in a structure of strings. one per line

id=fopen(name,'rt');
%titel=fgetl(id);    % in der ersten Zeile stehen die Namen der PArameter drin
if id<=0
    disp('File not found');
end

ret=sprintf('\n');
str=[];
i=1;
while ~feof(id)
    zeile=fgetl(id);
    str{i}=zeile;
    i=i+1;
end
fclose(id);
