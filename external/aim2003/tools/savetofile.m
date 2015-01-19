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
% $Date: 2003/03/06 14:24:21 $
% $Revision: 1.4 $

function savetofile(in,file)

id=fopen(file,'wt');

nr=length(in);
for i=1:nr
    fprintf(id,'%s\n',in{i});
end
    
fclose(id);
