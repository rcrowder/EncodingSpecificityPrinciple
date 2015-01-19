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
% $Date: 2003/03/10 16:08:17 $
% $Revision: 1.14 $

function handles=do_aim_autoscale(handles)


maxscalenumber=50; % this is the value to which the highest point is scaled to -> should look ok
multiplier=1;

start_time=slidereditcontrol_get_value(handles.currentslidereditcombi);
% start_time=start_time+getminimumtime(handles.data.signal);
duration=slidereditcontrol_get_value(handles.slideredit_duration);
stop_time=start_time+duration;
scale=slidereditcontrol_get_value(handles.slideredit_scale);

switch handles.info.current_plot
    case {-1,0,1}
        return
    case 2 % pcp
        data=handles.data.pcp;
%         data=getpart(data,start_time,stop_time);
		multiplier=0.01;
	case 3	% bmm
        data=handles.data.bmm;
        data=getpart(data,start_time,stop_time);
        nr_channels=getnrchannels(data);
		if nr_channels==1
	        multiplier=0.8;
		else
 	        multiplier=0.2/nr_channels;
		end
    case {4,5}
        data=handles.data.nap;
%         data=getpart(data,start_time,stop_time);
        nr_channels=getnrchannels(data);
        nr_channels=getnrchannels(data);
		if nr_channels==1
	        multiplier=0.8;
		else
  			multiplier=10/nr_channels;
		end
    case 6
        sai=handles.data.sai;
        nr_frames=length(sai);
        current_frame_number=round(slidereditcontrol_get_value(handles.currentslidereditcombi));
        current_frame=sai{current_frame_number};
        nr_channels=getnrchannels(current_frame);
        data=current_frame;
		if nr_channels>1
	        multiplier=30/nr_channels;
%  			multiplier=multiplier*max(data)/getallmaxvalue(data);
		else
			multiplier=1;
		end
    case 7
		return;
    case 8
		return;
end

if handles.info.current_plot < 6
	maxdata=max(data);
	if maxdata>0
		newscale=maxscalenumber*multiplier/maxdata;
	    newscale=min(newscale,handles.slideredit_scale.maxvalue);
	    newscale=max(newscale,handles.slideredit_scale.minvalue);
	else
	    newscale=1;
	end
else
	maxdata=getallmaxvalue(data);
	if maxdata>0
		newscale=maxscalenumber*multiplier/maxdata;
	    newscale=min(newscale,handles.slideredit_scale.maxvalue);
	    newscale=max(newscale,handles.slideredit_scale.minvalue);
	else
	    newscale=1;
	end
end

handles.slideredit_scale=slidereditcontrol_set_value(handles.slideredit_scale,newscale);
