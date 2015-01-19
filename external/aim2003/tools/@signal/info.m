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

function info(sig)
disp(sprintf('Name:       %s',sig.name));
disp(sprintf('Length:     %5.2f ms',GetLength(sig)*1000));
disp(sprintf('Points:     %d',size(sig.werte,1)));
disp(sprintf('Samplerate: %5.2f kHz',sig.samplerate/1000));
disp(sprintf('Unit X:     %s',sig.unit_x));
disp(sprintf('Unit Y:     %s',sig.unit_y));
disp(sprintf('Start time: %5.2f sec',sig.start_time));

