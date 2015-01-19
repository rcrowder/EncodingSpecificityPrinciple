% method of class @signal
% function sig=generateirn(sig,delay,g,niter)
%   INPUT VALUES:
%       sig: original @signal with length and samplerate 
%       delay: delay, after which the noise is added again
%       g: gain
%       niter: number of iterations that are added
% 
%   RETURN VALUE:
%       sig:  @signal 
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:42 $
% $Revision: 1.3 $

function sig=generateirn(sig,delay,g,niter)

srate=getsr(sig);
dur=getlength(sig);

dels=round(delay*srate);
npts=round(dur*srate);

nois=randn(size(1:npts));

for i=1:niter;
    dnois=nois;
    dnoist=dnois(1:dels);
    dnois=[dnois dnoist];
    dnois=dnois(dels+1:npts+dels);
    dnois=dnois.*g;
    nois=nois+dnois;
end;

rms=sqrt(mean(nois.*nois));
b=nois./rms;

sig=setvalues(sig,b);