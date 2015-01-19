% method of class @fsignal (derived from @signal)
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:47 $
% $Revision: 1.3 $

function [m,f]=max(fsig)
% returns the maximum as Value and Frequency!!
%if the return frequency is 0 then the next value is searched

werte=getdata(fsig);
werteneu=werte(2:end); % ohne den Gleichanteil!

[m,f]=max(werteneu);
    
f=(f-1)*fsig.df;    % umrechnung von bin zu Frequenz


