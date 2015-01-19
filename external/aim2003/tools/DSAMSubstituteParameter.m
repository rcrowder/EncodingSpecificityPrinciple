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

function new=DSAMSubstituteParameter(in,diesen,jenen)
% usage: success=DSAMSubstituteParameter(diesen,jenen)
% replaces the parameter "diesen" in the lines "In" by "jenen
% eg:   NUM_RUNS.ams.0           	2         	Number of repeat runs, or segments/frames.
% becomes   NUM_RUNS.ams.0           10
% 

len=size(diesen,2);
new=in;

nr=size(in,2);
for i=1:nr
    a=strfind(in{i},diesen);
    if ~isempty(a)
        line=i;
        break
    end
end

% and now substitute it:
tab=sprintf('\t');
new{i}=[diesen tab jenen tab];

return % das wars :-)

