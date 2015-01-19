% method of class @signal
% function [meansig,stdsig]=average(sig,[t_start],[t_stop])
% calculates the average value of the signal
%
%   INPUT VALUES:
%       sig:  original @signal
%       t_start: start time in seconds [0]
%       t_stop: stop time in seconds [getlength(sig)]
%    
%   RETURN VALUE:
%       meansig: mean value of the signal
%       stdsug: standart deviation
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/25 12:47:43 $
% $Revision: 1.4 $

function [meana,stda]=average(sig,t_start,t_stop)


if nargin < 2
    t_start=getminimumtime(sig);
end
if nargin < 3
    t_stop=t_start+getlength(sig);
end

intstart=time2bin(sig,t_start);
intstop=time2bin(sig,t_stop);

if intstart==0
    intstart=1;
end

if intstart>intstop
    error('signal::average: stoptime < starttime');
end


s=sig.werte(intstart:intstop);
if max(size(s))>1
    meana=mean(s);
    stda=std(s);
else
    meana=s;
    stda=0;
end    
