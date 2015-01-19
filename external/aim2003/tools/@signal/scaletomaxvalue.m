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

function sig=scaletomaxvalue(si,maxv)
%usage: sig=scaleToMaxValue(si,maxv)
% scales signal so, that the maximum value is maxv (usefull for saving as wav)

sig=si;
dat=sig.werte;
ma=max(dat);
mi=min(dat);
if -mi > ma
    ma=-mi;
end

sig.werte(:)=sig.werte(:)*maxv/ma;
 