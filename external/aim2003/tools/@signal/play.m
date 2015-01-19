% method of class @signal
% 
%   INPUT VALUES:
%  sig,attenuation
% sig is the signa
% attenuation is the attenuation against the lowdest possible tone
% with amplitude =1
% default=1
%
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/07/15 09:48:33 $
% $Revision: 1.6 $

function play(sig,attenuation,ramp)

if nargin < 3 
	ramp=0.01;
end
if nargin < 2
	attenuation=0;
end

if attenuation > 0
% 	error('cant play sounds louder then maximum, reduce attenuation!');
 	disp('warning: signal\play:: play sounds louder then maximum, reduce attenuation!');
end


sig=rampamplitude(sig,ramp);
sig=attenuate(sig,attenuation);

if max(sig)>1
 	disp('warning: signal\play:: clipping in signal');
end
sound(sig.werte,sig.samplerate);

pause(getlength(sig));