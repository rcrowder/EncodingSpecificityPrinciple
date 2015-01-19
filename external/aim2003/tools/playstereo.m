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
% $Date: 2003/01/17 16:57:45 $
% $Revision: 1.3 $

function playstereo(sig1,sig2)

sig1=rampamplitude(sig1,0.005);
sig1=scaletomaxvalue(sig1,0.999);
sig2=rampamplitude(sig2,0.005);
sig2=scaletomaxvalue(sig2,0.999);

values1=getvalues(sig1);
values2=getvalues(sig2);

finvals=[values1 values2];
sr=getsr(sig1);

wavwrite(finvals,sr,'last stereo');

sound(finvals,sr);

