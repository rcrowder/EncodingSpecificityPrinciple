% method of class @signal
% function sig=genbandpassnoise(sig,varargin)
%   INPUT VALUES:
%       sig: @signal with length and samplerate 
%   RETURN VALUE:
%       sig:  @signal 
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/07/17 10:58:38 $
% $Revision: 1.1 $

function sig=genbandpassnoise(sig,lowfrequency,highfrequency)
% steepness is given in dB per octave (to make it compatible with Fastl)

len=getlength(sig);
sr=getsr(sig);

% generate white noise:
vals=getvalues(sig);

nyquist_frequenz=sr/2;
N=getnrpoints(sig);

n1=round(lowfrequency/nyquist_frequenz*N/2);
n2=round(highfrequency/nyquist_frequenz*N/2);

if n1<=0
    n1=1;
end
if n2>N
    n2=N;
end

noise1=zeros(size(vals));
noise2=zeros(size(vals));
for i=n1:n2
    noise1(i)=rand(1);
end
for i=N-n2:N-n1
    noise2(i)=rand(1);
end
ftband=noise1+ i*noise2;
vals=ifft(ftband);
vals=real(vals);

sig=setvalues(sig,vals);
sig=setname(sig,sprintf('Bandpass filtered noise from %4.1f Hz to %4.1f Hz',lowfrequency,highfrequency));

return
