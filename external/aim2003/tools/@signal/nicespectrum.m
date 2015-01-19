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

function res=nicespectrum(a)

nr=getnrpoints(a);

%1. schiebe es so, dass der mittelwert 0 ist
a=settozeroaverage(a);

%2. multipliziere das Signal mit einem HanningFenster
han=hanning(nr,'periodic');
a=a*han;

% 3. powerspectrum

res=powerspectrum(a);

