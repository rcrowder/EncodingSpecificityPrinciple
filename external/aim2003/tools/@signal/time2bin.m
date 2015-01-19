% method of class @
%function bin=time2bin(sig,time) 
%   INPUT VALUES:
%  		sig: @signal
%		time: time in seconds
%   RETURN VALUE:
%		bin: interpolated bin value of that time in the signal corrected by
%		the signals start time
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:43 $
% $Revision: 1.3 $

function ret=time2bin(sig,time)
% returns the real value of the times in bins in the signal. This must not
% be the value that is needed to plot! (see time2plotbin)



time=time-sig.start_time;

ret=time*sig.samplerate;
ret=round(ret); 
