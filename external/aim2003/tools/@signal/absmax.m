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
% $Date: 2003/01/23 10:56:28 $
% $Revision: 1.4 $

function [m,x]=absmax(sig)
% returns the maximum absolut value of the signal

val=getdata(sig);
[m,x]=max(abs(val));
x=bin2time(sig,x-1);
