% method of class @signal
% 
%   INPUT VALUES:
%  sig,attenuation
% sig is the signa
% attenuation is the attenuation against the lowdest possible tone
% with amplitude =1
% default=1
%
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/06/11 10:46:32 $
% $Revision: 1.1 $



function sig=attenuate(sig,attenuation);

amphigh=max(sig);
amp=amphigh / power(10,-attenuation/20);
sig=scaletomaxvalue(sig,amp);

