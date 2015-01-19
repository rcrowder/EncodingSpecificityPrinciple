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

function bool=DSAMtParameterIsCommented(in,diesen,nr_occurance)
% ret=DSAMCommentOutParameter(in,diesen,nr_occurance)
% comments out the line in which the parameter "diesen" is by putting a @ in front of it
% the nth occurance is commented out. If there are not so many occurances, than 
% a -1 is returned
% because this routine is used for commenting out the displays, it only looks till  the first }


if nargin < 3
    nr_occurance=1; %take the first
end

len=size(diesen,2);
new=in;

nr=size(in,2);
occurance=0;
linenr=0;
for i=1:nr
    is_header=strfind(in{i},'}');
    if is_header    %stop, when out of header
        break;
    end
    a=strfind(in{i},diesen);
    if a>0
        occurance=occurance+1;
        if occurance==nr_occurance
            linenr=i;
            break
        end
    end
end

if linenr==0
    new=-1;
    return;
end

% and now comment it:
% but *only*, when it is not already commented! Otherwise it crashes!
zeile=new{linenr};
is_commented=strfind(in{i},'@');
if is_commented    %if commented, then the result is true
    bool=1;
else
    bool=0;
end


return % das wars :-)
