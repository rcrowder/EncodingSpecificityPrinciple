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
% $Date: 2003/01/17 16:57:42 $
% $Revision: 1.3 $

function val=getbinvalue(sig,bin)
% usage: val=gettimevalue(sig,bin)
% returns the value at this bin
% if the time is not exact on one bin, than interpolate 
% correctly

nr_points=getnrpoints(sig);
x=1:nr_points;
Y=sig.werte;
xi=bin;
method='linear';


% val=sig.werte(bin);

val=interp1(x,Y,xi,method);
