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

function linmeasure=log2lin(value,scaleinfo)

% wandelt einen Wert entsprechend den Angaben in scaleinfo wieder 
% in den ursprünglichen linearen Wert zurück

from=scaleinfo.from;
to=scaleinfo.to;
steps=scaleinfo.steps;

a1=log(from);
a2=log(to);
st=(a2-a1)/(steps-1);

linmeasure=log(value/from)/st+1;
