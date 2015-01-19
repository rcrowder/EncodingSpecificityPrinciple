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

function sig=hilbertenvelope(a)

sr=getsr(a);
duration=getlength(a);

sig=signal(duration,sr);
sig.name=sprintf('Hilbert Envelope of %s',a.name);

b=hilbert(a.werte);
sig.werte=abs(b);