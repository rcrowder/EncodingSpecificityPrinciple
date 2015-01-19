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
% $Date: 2003/03/10 10:24:34 $
% $Revision: 1.4 $

function ret=sortstruct(str,topic,nr_second_dimension)
% sorts the structure str by the values in "topic"
% if topic is twodimensional, then "nr_second_dimension" gives the chosen
% number


if nargin < 3
    nr=size(str,2);
    for i=1:nr
        sortcount(i)=eval(sprintf('str{%d}.%s',i,topic));
    end
    for i=1:nr
        [ismax,womax]=max(sortcount);
        ret{i}=str{womax};
        sortcount(womax)=-inf; % den möchte ich nicht mehr sehen!
    end
else
    nr=size(str,2);
    for i=1:nr
        sortcount(i)=eval(sprintf('str(%d).%s(%d)',i,topic,nr_second_dimension));
    end
    for i=1:nr
        [ismax,womax]=max(sortcount);
        ret(i)=str(womax);
        sortcount(womax)=-inf; % den möchte ich nicht mehr sehen!
    end
end