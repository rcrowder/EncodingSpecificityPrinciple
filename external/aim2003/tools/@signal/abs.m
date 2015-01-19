% method of class @signal
% function sig=abs(sig)
% calculates the abs value of the signal
%
%   INPUT VALUES:
%       sig:  original @signal
%    
%   RETURN VALUE:
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/06/11 10:46:32 $
% $Revision: 1.1 $

function sig=abs(sig)

s=abs(sig.werte);
sig.werte=s;

