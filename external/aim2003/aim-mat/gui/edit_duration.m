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
% $Revision: 1.11 $

function handles=edit_duration(hObject, eventdata, handles)


curval=get(hObject,'String');
curval=str2num(curval);

handles.slideredit_duration=...
	slidereditcontrol_set_raweditvalue(handles.slideredit_duration,curval);

sig=handles.data.signal;

if handles.info.current_plot>=6 %sai has additionally the start time below:
	current_start_time=slidereditcontrol_get_value(handles.slideredit_duration); % the new value
	framelen=getcurrentframestarttime(handles.data.sai{2})-getcurrentframestarttime(handles.data.sai{1});

	nr_frames=length(handles.data.sai);
	current_frame_nr=round((current_start_time-getminimumtime(sig))/framelen)+1; % plus 1 because of the duration of the frame itself
	current_frame_nr=min(nr_frames,current_frame_nr);
	current_frame_nr=max(1,current_frame_nr);
	handles.currentslidereditcombi=slidereditcontrol_set_value(handles.currentslidereditcombi,current_frame_nr);
	realval=(current_frame_nr-1)*framelen+getminimumtime(sig);
	handles.slideredit_duration=...
		slidereditcontrol_set_value(handles.slideredit_duration,realval);
else
	% check range
	start=slidereditcontrol_get_value(handles.currentslidereditcombi);
	dur=slidereditcontrol_get_value(handles.slideredit_duration);
	siglen=getmaximumtime(sig);
	if start+dur>siglen
		dur=siglen-start;
	end
	
	
	handles.slideredit_duration=...
		slidereditcontrol_set_value(handles.slideredit_duration,dur);
	
	% set the range of the start_slider accordingly
	len=slidereditcontrol_get_value(handles.slideredit_duration);
	handles.currentslidereditcombi=slidereditcontrol_set_range(handles.currentslidereditcombi,len);	% the duration
	
end
