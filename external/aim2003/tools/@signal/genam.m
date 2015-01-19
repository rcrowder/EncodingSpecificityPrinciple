% method of class @signal
% function sig=genam(sig,fcar,fmod,modgrad)
%
%   INPUT VALUES:
%       sig: original @signal with length and samplerate 
%       fcar: carrier frequency (Hz)
%       fmod: modulation frequency (Hz)
%       modgrad: modulation depth in (0-1)
% 
%   RETURN VALUE:
%       sig:  @signal 
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/25 12:47:43 $
% $Revision: 1.4 $

function sig=genam(sig,fc,f0,modgrad)


sr=getsr(sig);
len=getlength(sig);

f1=fc-f0;
f2=fc;
f3=fc+f0;


sin1=sinus(len,sr,f1,modgrad/2,0);
sin2=sinus(len,sr,f2,1,0);
sin3=sinus(len,sr,f3,modgrad/2,0);


sig=sin1;
sig=sig+sin2;
sig=sig+sin3;

name=sprintf('AM: modulation: %3.1f Hz, carrier: %4.1f kHz, modgrad: %2.1f',f0,fc/1000,modgrad);
sig=setname(sig,name);
sig=scaletomaxvalue(sig,1);
% sig=RampAmplitude(sig,0.01); % baue eine Rampe

