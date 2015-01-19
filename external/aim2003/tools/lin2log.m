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

function logmeasure=lin2log(value,scaleinfo)
% usage: function logmeasure=lin2log(value,scaleinfo)
% wandelt einen Wert entsprechend den Angaben in scaleinfo 
% von einem linearen in einen logarithmischen Wert um
% scaleinfo.from;
% scaleinfo.to;
% scaleinfo.steps;
% 


from=scaleinfo.from;
to=scaleinfo.to;
steps=scaleinfo.steps;


if steps==1
    logmeasure=(from+to)/2;
    logmeasure=from;
    return
end

a1=log(from);
a2=log(to);
st=(a2-a1)/(steps-1);

res=st*value;
res=exp(res);

logmeasure=res*from;

