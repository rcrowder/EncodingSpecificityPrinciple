% tool
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:44 $
% $Revision: 1.3 $

function res=bin2time(bin,samplerate)
% gibt die Zeit zurück, bei dem dieses Bin ist
% Zeit immer in Sekunden
% Samplerate immer in Bins pro Sekunde (96 kHz)

res=bin/samplerate;