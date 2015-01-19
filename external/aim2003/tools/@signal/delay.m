% method of class @signal
% function delay(sig)
% delays the signal by so many seconds. Fills the start with zeros and cuts
% the end
%
%   INPUT VALUES:
%       sig: original @signal
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/08/06 10:46:30 $
% $Revision: 1.1 $

function sig=delay(sig,del)

sr=getsr(sig);
% nr_del=del*sr;
nr_del=round(del*sr);
vals=getvalues(sig);
nvals1=zeros(1,nr_del);
nvals2=vals(1:end-nr_del);

new_vals=[nvals1 nvals2'];
sig=setvalues(sig,new_vals);
sig=setname(sig,sprintf('%s delayed by %f sec',getname(sig),del));


