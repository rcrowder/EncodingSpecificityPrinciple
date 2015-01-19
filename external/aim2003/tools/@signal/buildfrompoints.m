% method of class @signal
% function sig=buildfrompoints(sig,xx,yy)
% calculates a @signal from the points in x and y 
%
%   INPUT VALUES:
%       sig:  original @signal
%       xx: x-values of points
%       yy: y-values of points
%    
%   RETURN VALUE:
%       sig: new @signal 
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:42 $
% $Revision: 1.3 $

function sig=buildfrompoints(sig,xx,yy)


x1=1;
y1=0;

sig=mute(sig);

nr_points=length(xx);
for i=1:nr_points
    x2=round(time2bin(sig,xx(i)));
    y2=yy(i);

    line=linspace(y1,y2,x2-x1+1);
    sig=setvalues(sig,line,x1);

    x1=x2+1;
    y1=y2;
    
end


