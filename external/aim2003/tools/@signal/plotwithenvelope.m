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

function plotwithenvelope(signal,border,stil)

h=hilbertenvelope(signal);
hh=GetData(h);
plot(hh,'b-','linewidth',1.2);hold on

if nargin==3
    plot(signal,border,stil); 
end

if nargin==2
    plot(signal,border); 
end

if nargin==1
    plot(signal); 
end




