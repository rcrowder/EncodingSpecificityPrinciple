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

function sig=generateAMnoise(sig,fre,modgrad)

len=getlength(sig);
sr=getsr(sig);

% generate white noise:
vals=getvalues(sig);
vals=rand(size(vals)).*2-1;
sig=setvalues(sig,vals);

envelope=generatesinus(sig,fre,1,0);
envelope=(envelope+1)/2;

sig=sig*envelope;


sig=setname(sig,sprintf('AM noise Frequency %4.1f Hz',fre));

return
