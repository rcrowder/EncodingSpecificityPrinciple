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

function display(fsig)

disp('Frequency Spectrum');
disp(sprintf('Name: %s',getname(fsig)));
disp(sprintf('Points: %d',getnrpoints(fsig)));
disp(sprintf('Frequency from 0 to %5.2f Hz',fsig.max_fre));
