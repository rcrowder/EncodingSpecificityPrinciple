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
% $Date: 2003/03/06 14:24:21 $
% $Revision: 1.6 $

function handles=slider_duration(hObject, eventdata, handles)


curval=get(hObject,'Value');
handles.slideredit_duration=...
	slidereditcontrol_set_rawslidervalue(handles.slideredit_duration,curval);


% check range
start=slidereditcontrol_get_value(handles.currentslidereditcombi);
dur=slidereditcontrol_get_value(handles.slideredit_duration);
sig=handles.data.signal;
siglen=getmaximumtime(sig);
% siglen=getlength(sig);
if start+dur>siglen
	dur=siglen-start;
	handles.slideredit_duration=...
		slidereditcontrol_set_value(handles.slideredit_duration,dur);
end

% set the range of the start_slider accordingly
curstart=slidereditcontrol_get_value(handles.currentslidereditcombi);
dur=slidereditcontrol_get_value(handles.slideredit_duration);
handles.slideredit_start.maxvalue=siglen-dur;
handles.slideredit_start=slidereditcontrol_set_value(handles.slideredit_start,curstart);	% the duration
handles.slideredit_start=slidereditcontrol_set_range(handles.slideredit_start,dur);	% the duration

if handles.info.current_plot<6
	handles.currentslidereditcombi=handles.slideredit_start;
end

return
