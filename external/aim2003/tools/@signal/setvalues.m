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
% $Date: 2003/05/20 17:45:42 $
% $Revision: 1.4 $

function sig=setvalues(org_sig,new_vals,startint)
%usage: sig=setvalues(si,new,vals)
% sets all values of the signal org_sig to the new values
% if only one value is given, than all vals are set to this value

if getnrpoints(org_sig)~=max(size(new_vals)) & nargin < 3 
    disp('error: setvalues: different size of signal and new values');
end

if nargin < 3
    startint=1;
end

if length(new_vals)==1
    len=getnrpoints(org_sig)-startint+1;
    new_vals=ones(len,1)*new_vals;
end
if size(new_vals,1)< size(new_vals,2)
    new_vals=new_vals';
end

len=size(new_vals,1);

nval=org_sig.werte;
nval(startint:startint+len-1)=new_vals;
org_sig.werte=nval;
sig=org_sig;
 