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

function [cent,oct,relcent]=getmusicaltone(fre)
% usage: [cent,oct]=getmusicaltone(fre)
% gives back the cent of this tone and in which octave above 27.5 Hz it is
% and the relative cent above the last octave - redundant, but useful

min_fre=27.5; % the frequency, that defines the smallest frequency (A2)

oct=floor(log2(fre/min_fre));

% one octave is 1200 cent:
% f= f_min*power(2,oct)*power(2,cent/1200)

cent=1200*log2(fre/min_fre);

relcent=cent-oct*1200;
