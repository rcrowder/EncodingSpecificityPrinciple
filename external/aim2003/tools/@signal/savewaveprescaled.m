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

function savewaveprescaled(sig,name)
% does some things, to make a nice sound out of it
% but assumes sig has already been scaled!
% just take out one line of savewave

sig=rampamplitude(sig,0.005);
%sig=scaletomaxvalue(sig,0.999);
if ~strfind(name,'.wav')
    name=sprintf('%s.wav',name);
end

writetowavefile(sig,name);