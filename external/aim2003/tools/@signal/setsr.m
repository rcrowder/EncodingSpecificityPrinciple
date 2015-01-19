% method of class @signal
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:43 $
% $Revision: 1.3 $

function sig=setsr(sig,sr_neu)
% usage: sig=setsr(a,sr_neu)
% simply sets the sample rate to a fixed value without changing the data!
% if you want to change the samplerate of a signal, that is already there,
% use changesr(sig,new)!


sig.samplerate=sr_neu;
