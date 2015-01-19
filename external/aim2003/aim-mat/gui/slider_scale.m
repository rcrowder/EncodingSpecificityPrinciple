% procedure for 'aim-mat'
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/02/19 12:01:18 $
% $Revision: 1.4 $

function handles=slider_scale(hObject, eventdata, handles)

curval=get(hObject,'Value');
handles.slideredit_scale=...
	slidereditcontrol_set_rawslidervalue(handles.slideredit_scale,curval);

return

