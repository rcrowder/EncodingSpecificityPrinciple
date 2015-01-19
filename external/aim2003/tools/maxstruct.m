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
% $Date: 2003/06/11 10:46:32 $
% $Revision: 1.1 $

function [ret,womax]=maxstruct(str,topic,nr_second_dimension)
% gets back the structitem with the highest value in "topic"
% if topic is twodimensional, then "nr_second_dimension" gives the chosen
% number


if nargin < 3
    nr=size(str,2);
    for i=1:nr
        sortcount(i)=eval(sprintf('str{%d}.%s',i,topic));
    end
    [ismax,womax]=max(sortcount);
	ret=str{womax};
else
    nr=size(str,2);
    for i=1:nr
        sortcount(i)=eval(sprintf('str(%d).%s(%d)',i,topic,nr_second_dimension));
    end
    [ismax,womax]=max(sortcount);
	ret=str{womax};
end