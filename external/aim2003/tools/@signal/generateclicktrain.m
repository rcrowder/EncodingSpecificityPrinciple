% method of class @signal
% function sig=generateclicktrain(sig,frequency,[amplitude])
%   INPUT VALUES:
%       sig: original @signal with length and samplerate 
%       frequency: frequency of the clicktrain (Hz)
%       amplitude: amplitude [1]
% 
%   RETURN VALUE:
%       sig:  @signal 
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/03/04 10:32:20 $
% $Revision: 1.5 $

function sig=generateclicktrain(sig,frequency,amplitude)

if nargin < 3
    amplitude=1;
end

name= sprintf('Clicktrain %5.2f Hz',frequency);

df=floor(getsr(sig)/frequency);
to=time2bin(sig,getlength(sig));
clicks=1:df:to; % start at the sampletime

sig=setbinvalue(sig,clicks,amplitude);

