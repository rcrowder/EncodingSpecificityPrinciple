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

function ret=sig2simulinkinput(sig)
% wandelt das signal sig in eine Struktur um, die Simulink lesen kann

sr=getsr(sig);
len=getlength(sig);

ret.time=(1/sr:1/sr:len)';
ret.signals(1).values=getvalues(sig);
ret.signals(1).dimensions=1;

