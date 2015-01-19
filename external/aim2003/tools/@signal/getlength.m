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

function res =getlength(sig)
% returns the length in seconds
    
nr=size(sig.werte,1);
% r1=bin2time(sig,0);
% r2=bin2time(sig,nr);
% res=r2-r1;

sr=getsr(sig);
res=nr/sr;

