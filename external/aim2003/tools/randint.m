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

function int=randint(von,bis)
% produces an int between von and bis or from 1 to von

if nargin < 2
    bis=von;
    von=1;
end

v=rand(1)*(bis-von);
v=v+von;
int=round(v);
