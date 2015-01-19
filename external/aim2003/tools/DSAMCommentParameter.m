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

function new=DSAMCommentParameter(in,diesen,whether,nr_occurance)
% new=DSAMCommentParameter(in,diesen,whether,nr_occurance)
% if whether=1:
% comments the line in which the parameter "diesen" is by putting a @ in front of it
% the nth occurance is commented out. If there are not so many occurances, than 
% a -1 is returned
% if whether=0:
% uncomments the line in which the parameter "diesen" is by removing the @ in front of it
% because this routine is used for commenting out the displays, it only looks till  the first '}'


if nargin < 4
    nr_occurance=1; %take the first
end

if nargin < 3
    whether=1; %comment it
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
if is_commented   &  ~whether  % remove comment
    zeile(is_commented)=' ';    % overwrite the comment
    new{linenr}=zeile;
end
if size(is_commented,1)==0   &  whether  % add comment
    new{linenr}=['@' zeile];
end
% if size(is_commented,1)>0   &  ~whether  % nothing to change
%     new=in;  
% end
% if is_commented   &  whether
%     new=in;     % nothing to change!
% end


return % das wars :-)
