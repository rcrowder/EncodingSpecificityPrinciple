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

function ret=extracttolist(list,delimiter)
%usage: ret=extracttolist(list,delimiter)
% returns a list of strings that are in 'list' and deliminited by delimiter

nrwhere=findstr(list,delimiter); % 9 is tab!
start=1;
for i=1:length(nrwhere)
    stop=nrwhere(i)-1;
    ret{i}=list(start:stop);
    start=nrwhere(i);
end 

% and the last one before the end of the line
ret{i+1}=list(start:end);
