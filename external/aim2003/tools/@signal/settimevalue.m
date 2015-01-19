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
% $Date: 2003/01/17 16:57:43 $
% $Revision: 1.3 $

function sig=settimevalue(sig,time,val)
%usage: sig=setvalue(sig,time,val)
% sets the double time value "time" to "val"

nr=time2bin(sig,time);
sig.werte(nr)=val;