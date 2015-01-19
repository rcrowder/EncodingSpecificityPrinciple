% method of class @
% function writetowavefile(sig,name)
%   INPUT VALUES:
%  		sig: @signal
%		name: name of the resulting wavefile
%   RETURN VALUE:
%	none
% 
% writetowavefile saves the signal to a wavefile without changing its
% amplitude (contrary to savewave)
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:43 $
% $Revision: 1.3 $

function writetowavefile(sig,name)
samplerate=getsr(sig);
readsounddata=getdata(sig);
audiowrite(readsounddata,samplerate,name);
