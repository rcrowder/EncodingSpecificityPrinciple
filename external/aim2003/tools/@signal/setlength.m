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

function sig=setlength(sig,newlen)

% sig.length=newlen;
nr_points=time2bin(sig,newlen);
old_nr_points=getnrpoints(sig);
if nr_points < old_nr_points
    new_vals=sig.werte(1:nr_points);
else
    new_vals=zeros(nr_points,1);
    new_vals(1:old_nr_points)=sig.werte;
end    
clear sig.werte;
sig.werte=new_vals;
