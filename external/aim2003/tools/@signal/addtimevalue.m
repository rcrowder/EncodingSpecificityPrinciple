% method of class @signal
% function sig=addtimevalue(sig,time,val)
%usage: sig=addtimevalue(sig,time,val)
% adds the double time value "time" to "val"
%
%   INPUT VALUES:
%       sig:  original @signal
%       time: time in seconds, where the value is added
%       val: value, that is added at time
%    
%   RETURN VALUE:
%       sig: @signal
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:42 $
% $Revision: 1.3 $

function sig=addtimevalue(sig,time,val)

nr=time2bin(sig,time);
sig.werte(nr)=sig.werte(nr)+val;