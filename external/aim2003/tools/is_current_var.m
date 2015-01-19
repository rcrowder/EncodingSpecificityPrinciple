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
% $Date: 2003/02/10 19:29:02 $
% $Revision: 1.4 $

function wheter=is_current_var(varname,in)
% usage: is_current_var(varname,)
% returns a bool whether this variable is a defined variable or not

n=size(in,1);
for i=1:n
    if strcmp(in(i),varname)
        wheter=1;
        return
    end
end
wheter=0;

