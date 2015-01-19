% generating function for 'aim-mat'
%function returnframes=gen_ti2003(nap,strobes,options)
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% time integration
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/09/03 15:32:17 $
% $Revision: 1.1 $

function nap=genusernap(NAP,options2)
% calculates the stablized image from the data given in options

% commented by Stefan because of strange errors. Richard can comment this
% back in when he fixes that bug


% 
% %set the sampling rate
% fs  = 48000;   
% 
% %if you wish to use a stored version
% filetoload=[options2.path options2.file '.mat'];
% load(filetoload);
% 
% str=sprintf('nap_matrix=%s;',options2.file);
% eval(str);
% nap=frame(nap_matrix);  
% nap=setsr(nap,fs);