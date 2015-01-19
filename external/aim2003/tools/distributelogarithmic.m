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

function [res,scaleinfo]=distributelogarithmic(from,to,steps)
% usage: [res,scaleinfo]=distributelogarithmic(from,to,steps)
% gives back values that start from "from" and go to "to" in 
% "steps" steps, so that the stepwidth is logarithmic

% returnvalues to keep track (needed in some cases)
scaleinfo.from=from;
scaleinfo.to=to;
scaleinfo.steps=steps;


res=0:steps-1;
res=lin2log(res,scaleinfo);
