% method of class @signal
% function sigresult=copy(sig1,sig2,[start_time])
% copies the first signal in the second, also when sig2 is a struct. 
%
%   INPUT VALUES:
%       sig1:       first @signal
%       sig2:       second @signal or struct
%       start_time: start time for copying. [default: 0]
%   RETURN VALUE:
%       sigresult:  @signal `
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:42 $
% $Revision: 1.3 $

function sig=copy(a,b,start_time)

if nargin<3
    start_time=0;
end

if isobject(werte)
    not implemented yet
end

if isnumeric(b)
    sig=signal(a);
    sig=mute(sig);  % setze alle Werte auf Null
    
    nr=size(b,1);
    dauer=bin2time(sig,nr);
    
    sig=add(a,b,start_time,dauer)
end


   

