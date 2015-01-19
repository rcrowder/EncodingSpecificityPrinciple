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

function sig=secondderivation(sig)
% calculates the second derivation by calculation of the differences


val=getvalues(sig);

nr=length(val);


nval=val;
nval(1)=0;
nval(2)=0;
for i=3:nr

    if i==370
        a=0;
    end
    nval(i)=(val(i-2)-2*val(i-1)+val(i))/2;
end

sig=setvalues(sig,nval);
