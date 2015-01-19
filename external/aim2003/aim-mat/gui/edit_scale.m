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
% $Date: 2003/01/17 16:57:45 $
% $Revision: 1.3 $

function handles=edit_scale(hObject, eventdata, handles)


curval=get(hObject,'String');
curval=str2num(curval);
handles.slideredit_scale=...
	slidereditcontrol_set_raweditvalue(handles.slideredit_scale,curval);


return



% strval=str2double(get(hObject,'String'));
% von=handles.data.min_scale;
% bis=handles.data.max_scale;
% if strval>bis
%     strval=bis;
% end
% if strval<von
%     strval=von;
% end
% val=f2f(strval,von,bis,0,1,'loglin');
% val=fround(val,3);
% set(handles.slider1,'Value',val);
