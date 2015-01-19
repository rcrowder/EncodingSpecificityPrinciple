% method of class @signal
% function sig=buildspikesfrompoints(sig,xx,yy)
% calculates a @signal from the points in x and y
% the new signal is zero everywhere except from the points in xx
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

function sig=buildspikesfrompoints(sig,xx,yy)
% uage: sig=buildspikesfrompoints(sig,xx,yy)
% makes a dot of the hight given in yy at each point given in xx
% all other values =0


sig=mute(sig);

nr_points=length(xx);
for i=1:nr_points
    oldval=gettimevalue(sig,xx(i));
    newval=oldval+yy(i);
    sig=addtimevalue(sig,xx(i),newval);
end


