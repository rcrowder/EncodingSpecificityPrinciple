% method of class @signal
% function sig=generatesinus(sig,[fre],[amplitude],[phase])
%   INPUT VALUES:
%       sig: original @signal with length and samplerate 
%       fre: frequency (Hz) [1000]
%       amplitude: [1]
%       phase: startphase [0]
%       phases must be in degrees!
%   RETURN VALUE:
%       sig:  @signal 
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:42 $
% $Revision: 1.3 $


function sig=generatesinus(sig,fre,amplitude,phase)

if nargin < 4
    phase=0;
end
if nargin < 3
    amplitude=1;
end
if nargin < 2
    fre=1000;
end

nr_points=getnrpoints(sig);
sr=getsr(sig);
length=getlength(sig);

von=0+phase;
periode=1/fre;
bis=2*pi*length/periode + phase;


temp=linspace(von,bis,nr_points);
data=sin(temp);

data=data*amplitude;

sig=signal(data);
sig=setsr(sig,sr);
sig=setname(sig,sprintf('Sinus %4.2f kHz',fre/1000));