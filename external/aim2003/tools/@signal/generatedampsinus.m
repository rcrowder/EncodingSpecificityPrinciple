% method of class @signal
% function sig=generatedampsinus(sig,carfre,modfre,amplitude,halflife)
%   INPUT VALUES:
%       sig: original @signal with length and samplerate 
%       carfre: carrier frequency (Hz) [1000]
%       modfre: modulation frequency (Hz) [100]
%       amplitude: [1]
%       halflife: time for the envelope envelope to decrease exponentielly
%       to 1/2
% 
%   RETURN VALUE:
%       sig:  @signal 
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/07/28 14:20:20 $
% $Revision: 1.6 $


function sig=generatedampsinus(sig,carfre,modfre,amplitude,halflife,onsettime)
% generates a damped sinusoid, that is a carrier pure tone modulated with a
% exponentially decreasing envelope. 
% sig is the signal
% carfre is the carrier frequency of the pure tone
% modfre is the modulation frequency (in Hz)
% amplitude is the final amplitude
% halflife is the time in seconds, in which the envelope drops to its half value
% onsettime is the time in wich the envelope reaches its maximum in seconds
% shift is a shift of the envelope in seconds


% if nargin < 7
%     shift=0;
% end
if nargin < 6
    onsettime=0;
end
if nargin < 5
    halflife=0.01;
end
if nargin < 4
    amplitude=1;
end

if nargin < 3
    modfre=100;
end
if nargin < 2
    carfre=1000;
end


sinus=generatesinus(sig,carfre,amplitude,0);

% calculate envelope and mult both
envelope=sig;
time_const=halflife/0.69314718;

env_vals=getvalues(envelope);
time=0;
sr=getsr(envelope);
reprate=1/modfre;

for i=1:getnrpoints(envelope);
    time=time+1/sr;
    
%     env_vals(i)= exp(-(time)/time_const);
    env_vals(i)= power(time,onsettime)*exp(-(time)/time_const);
    time=mod(time,reprate);
    
end
envelope=setvalues(envelope,env_vals);
envelope=envelope/max(envelope)*amplitude;
envelope=setstarttime(envelope,0);

% set the envelope and the amplitude
sig=sinus*envelope;

sig=setname(sig,sprintf('Damped Sinus %4.2f kHz, Modulation=%4.2f Hz, halflife=%4.2f ms',carfre/1000,modfre,halflife*1000));