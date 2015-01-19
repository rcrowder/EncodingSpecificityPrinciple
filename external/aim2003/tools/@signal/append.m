% method of class @signal
% function sigresult=append(sig1,sig2)
% appends the second signal behind the first
%
%   INPUT VALUES:
%       sig1:  first @signal
%       sig2:  second @signal
%    
%   RETURN VALUE:
%       sigresult:  @signal
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/03/08 12:26:04 $
% $Revision: 1.4 $


function sig=append(sig,b)

nr=max(size(b));

if nr==1
%     b=setstarttime(b,getmaximumtime(sig));
 	a=[sig.werte' getvalues(b)'];
	sig.werte=a';
% 	sig=add(sig,b,getlength(sig));
    return;
end

for i=1:nr
    sig=add(sig,b{i},getlength(sig));
end

