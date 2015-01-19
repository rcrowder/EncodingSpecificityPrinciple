% method of class @
%function sig=uminus(sig,b) 
%   INPUT VALUES:
%  		sig: original signal
%   RETURN VALUE:
%		sig: inverted signal
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:43 $
% $Revision: 1.3 $

function sig=uminus(a,b)
%Vorzeichenwechsel
sig=a;
sig.werte= -sig.werte;
