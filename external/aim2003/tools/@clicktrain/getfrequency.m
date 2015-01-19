% method of class @clicktrain (derived from @signal)
% function fre=getfrequency(ct)
%   INPUT VALUES:
%       ct: @clicktrain
%   RETURN VALUE:
%       fre=frequency of the clicktrain
%
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:46 $
% $Revision: 1.3 $


function fre=getfrequency(ct)
fre=ct.frequency;