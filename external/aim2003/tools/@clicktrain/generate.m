% method of class @clicktrain (derived from @signal)
% function ct=generate(ct) 
%   INPUT VALUES:
%       ct=old @clicktrain
%   RETURN VALUE:
%       ct=new @clicktrain with the correct values
%
%
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/06/11 10:44:50 $
% $Revision: 1.4 $


function ct=generate(ct)

ct=mute(ct);

df=floor(getsr(ct)/ct.frequency);
to=time2bin(ct,getlength(ct));
clicks=1:df:to;

ct=setbinvalue(ct,clicks,ct.amplitude);