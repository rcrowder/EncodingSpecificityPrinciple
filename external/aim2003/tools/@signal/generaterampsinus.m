% method of class @signal
% function sig=generaterampsinus(sig,carfre,modfre,amplitude,halflife)
%   INPUT VALUES:
%       sig: original @signal with length and samplerate 
%       carfre: carrier frequency (Hz) [1000]
%       modfre: modulation frequency (Hz) [100]
%       amplitude: [1]
%       halflife: time for the envelope envelope to rise exponentially
%       to 1/2
% 
%   RETURN VALUE:
%       sig:  @signal 
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/04/27 15:06:59 $
% $Revision: 1.4 $


function sig=generaterampsinus(sig,carfre,modfre,amplitude,halflife)

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
    
    env_vals(i)= exp(-(reprate-time)/time_const);
    time=mod(time,reprate);
    
end

envelope=setvalues(envelope,env_vals);

sig=sinus*envelope;
% sig=sig*amplitude;

sig=setname(sig,sprintf('Ramp Sinus %4.2f kHz, Modulation=%4.2f Hz, halflife=%4.2f ms',carfre/1000,modfre,halflife*1000));