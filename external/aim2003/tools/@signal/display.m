% method of class @signal
% function display(sig)
% overwritten function that is called from the shell and for the tooltips
%
%   INPUT VALUES:
%       sig: original @signal
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:42 $
% $Revision: 1.3 $

function display(sig)

if max(size(sig)) > 1
    disp(sprintf('Array of user class Signal with %d signals',max(size(sig))));;
else
    disp('User Signal');
    disp(sprintf('Name: %s',sig.name));
    l=getlength(sig);
    if l > 1
        disp(sprintf('Length=%3.2f sec',getlength(sig)));
    else
        disp(sprintf('Length=%4.1f ms',getlength(sig)*1000));
    end
    disp(sprintf('Points=%d',size(sig.werte,1)));
    disp(sprintf('Samplerate=%5.2f Hz',getsr(sig)));
end