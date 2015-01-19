% method of class @signal
% function time=firsttimebiggerzero(sig)
%
% returns the time, where the signal is for the first time
% bigger then zero
% useful for throwing away empty parts of signals
%
%   INPUT VALUES:
%       sig: original @signal 
% 
%   RETURN VALUE:
%       time: time, when signal is bigger 0 for first time
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:42 $
% $Revision: 1.3 $

function time=firsttimebiggerzero(sig)

vals=getvalues(sig);
big=find(vals>0);
m=min(big);

time=bin2time(sig,m);

