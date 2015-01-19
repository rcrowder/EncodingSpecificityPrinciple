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

function val=rms(sig)
%usage: val=rms(data)
%
%returns rms of given vector
%another way would be val=norm(data)/sqrt(length(data))
% David R R Smith   22/05/02

data=getvalues(sig);
val=(mean(data.^2)).^0.5;

