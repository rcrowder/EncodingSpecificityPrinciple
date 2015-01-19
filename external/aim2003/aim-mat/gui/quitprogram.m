% procedure for 'aim-mat'
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% my close function 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/04/23 10:24:33 $
% $Revision: 1.6 $

function quitprogram(obj,eventdata,handles)
% selection = questdlg('Close AIM?',...
% 	'Close Request Function',...
% 	'Yes','No','Yes');
% switch selection,
% 	case 'Yes',
		try
			% first delete possible children
			if isfield(handles.info,'children')
				single_channel_gui('close');
				delete(handles.info.children.single_channel.windowhandle)
			end
			% then delete all graphic windows associated:
			try 
				close(handles.info.current_figure);
			end
			
			% the standart closing routine (first show window)
			shh = get(0,'ShowHiddenHandles');
			set(0,'ShowHiddenHandles','on');
			currFig = get(0,'CurrentFigure');
			set(0,'ShowHiddenHandles',shh);
			delete(currFig);
			% delete(gcf);
% 			close(handles.info.current_figure);
		end
% 	case 'No'
% 		return
% end