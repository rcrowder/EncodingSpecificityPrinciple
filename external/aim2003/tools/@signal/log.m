% method of class @signal
% function sig=log(sig)
%   INPUT VALUES:
%       sig: original @signal with length and samplerate 
% 
%   RETURN VALUE:
%       sig:  @signal 
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/07/27 15:31:43 $
% $Revision: 1.1 $


function sig=log(sig)
sig.werte=log(sig.werte);