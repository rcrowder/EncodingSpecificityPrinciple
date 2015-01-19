function x=solve(p,y)
% solves the polynom p for the value at y
%
%   INPUT VALUES:
% input: a polynom
%  
%   RETURN VALUE:
% return value(s) are the values, where the polynom is equal the
% y-value
%
 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/06/11 10:46:32 $
% $Revision: 1.1 $


p(end)=p(end)-y;
x=roots(p);
