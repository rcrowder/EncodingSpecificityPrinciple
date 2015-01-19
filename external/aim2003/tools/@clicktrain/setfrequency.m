% method of class @clicktrain (derived from @signal)
% function ct=setfrequency(ct,fre)
%   INPUT VALUES:
%       fre: frequency of the clicktrain in Hz
%   RETURN VALUE:
%       ct: new clicktrain
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:46 $
% $Revision: 1.3 $


function ct=setfrequency(ct,was)
ct.frequency=was;