% method of class @signal
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

function sig=plus(a,b)
% addition 
% einfachster Fall: Addiere eine konstante Zahl
% sonst: Addiere ein zweites Signal zum Zeitpunkt Null

if isnumeric(b)
    a.werte=a.werte+b;
    sig=a;
    return
end

if isobject(a)
    dauer=getlength(b);
    start=getminimumtime(a);
    sig=add(a,b,start,dauer);
end