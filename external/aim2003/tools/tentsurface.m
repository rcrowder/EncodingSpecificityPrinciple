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

function tentsurface(framestruct_a);
% plots the current frame (cframe)
% all relevant data must be in the frame-object
% same as aisurface, only of the tent


if ~isstruct(framestruct_a)
%     error('AIsum must be called with a structure');
    framestruct.current_frame=framestruct_a;
else
    framestruct=framestruct_a;
end
current_frame=framestruct.current_frame;
current_frame=buildtent(current_frame);
framestruct.current_frame=current_frame;

aisurface(framestruct);