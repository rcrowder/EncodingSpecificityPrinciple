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
% $Date: 2003/01/17 16:57:45 $
% $Revision: 1.3 $

function ret=replacestr(orginal,org,sub)
% usage: ret=replacestr(orginal,org,sub)
% replaces org with sub without any error checking

a=findstr(orginal,org);
while ~isempty(a)
    orginal(a)=sub;
    a=findstr(orginal,org);
end 

ret=orginal;
