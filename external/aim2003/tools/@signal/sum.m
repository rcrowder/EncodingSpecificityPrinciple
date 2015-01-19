% method of class @
%function s=sum(sig,t_start,t_stop) 
%   INPUT VALUES:
%  	sig: @signal
%	t_start: start time (default: 0)
% 	t_stop: sum until here (default: signal-length)
%
%   RETURN VALUE:
%		s: sum of the signal in that region
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/07/27 15:29:59 $
% $Revision: 1.4 $

function s=sum(sig,t_start,t_stop)

if nargin < 2
    t_start=getminimumtime(sig);
end
if nargin < 3
    t_stop=getmaximumtime(sig);
end

intstart=time2bin(sig,t_start)+1;
intstop=time2bin(sig,t_stop);


if intstart<1
    intstart=1;
    disp('signal::sum: starttime too small');
end
if intstop>getnrpoints(sig)
    intstop=getnrpoints(sig);
    disp('signal::sum: intstop too big!');
end

% if intstart>intstop
%     error('signal::sum: stoptime < starttime');
% end


s=sum(sig.werte(intstart:intstop));