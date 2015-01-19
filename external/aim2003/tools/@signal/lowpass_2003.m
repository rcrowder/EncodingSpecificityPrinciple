%  
% function 
%
%   INPUT VALUES:
% 		
%
%   RETURN VALUE:
%		
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/03/04 17:41:26 $
% $Revision: 1.3 $



function sig=lowpass_2003(sig,cutoff,order)


if nargin<2
    order=1;
end


sr=getsr(sig);
vals=getvalues(sig);

[b a] = butter(order,cutoff/(sr/2));


sig_len = length(vals);


filt_vals(1:sig_len) = filter(b,a,vals);
vals=filt_vals;


sig=setvalues(sig,vals);
