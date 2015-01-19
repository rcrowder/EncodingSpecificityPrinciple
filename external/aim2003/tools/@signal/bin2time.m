% method of class @signal
% function res=bin2time(sig,bin)
% calculates the time according to the value of the bin in
%
%   INPUT VALUES:
%       sig:  original @signal
%       bin: value of bin (not necessary integer)
%    
%   RETURN VALUE:
%       res: time of bin in seconds
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:42 $
% $Revision: 1.3 $

function res=bin2time(sig,val)
% gibt die Zeit zurück, bei dem dieses Bin ist
% Zeit immer in Sekunden
% Samplerate immer in Bins pro Sekunde (96 kHz)
sr=sig.samplerate;
% eines abgezogen, weil Matlab bei 1 startet also bin 1der Anfangszeitpunkt ist
% res=(val-1)/sr;
res=val/sr;

res=res+sig.start_time;