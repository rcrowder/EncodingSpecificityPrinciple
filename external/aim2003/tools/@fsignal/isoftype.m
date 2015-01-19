% method of class @fsignal (derived from @signal)
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:47 $
% $Revision: 1.3 $

function ret=isoftype(inp,str)

if strcmp(str,'fsignal')
    ret=1;
else
    ret=0;
end