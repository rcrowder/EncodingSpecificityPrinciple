% method of class @signal
% function sig=autocorrelate(sig,[delay_start],[delay_stop],[normalization_mode])
%
% calculates the auto corrlelation between of the signal sig
% return value is a signal that covers the correlation between 
% between delay_start and delay_stop.
%
%   INPUT VALUES:
%       sig: original @signal
%		delay_start: start of the correlation : default -length(sig)
%		delay_stop: longest delay of the correlation : default length(sig)
% 		normalization_mode: normalizateion: default: 'biased' (see help 'xcorr')
% 		
%   RETURN VALUE:
% 		@sig: the correlation values at each delay
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/02/01 14:15:26 $
% $Revision: 1.1 $

function sig=autocorrelate(sig,delay_start,delay_stop,normalization_mode)
if nargin==1
	sig=crosscorrelate(sig,sig);
else if nargin==2
		sig=crosscorrelate(sig,sig,delay_start);
	else if nargin==3
			sig=crosscorrelate(sig,sig,delay_start,delay_stop);
		else if nargin==4
				sig=crosscorrelate(sig,sig,delay_start,delay_stop,normalization_mode);
			end
		end
	end
end