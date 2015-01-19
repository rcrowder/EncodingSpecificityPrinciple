% procedure for 'aim-mat'
% function namestr=aim_getwindowtitle(handles)
% 
%   INPUT VALUES:
%   RETURN VALUE:
% 	a string with the current graphics window title
%
% plots the current graphic in the GUI or in the current windowhandle
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/03/14 14:15:55 $
% $Revision: 1.1 $

function namestr=aim_getwindowtitle(handles)

modus=[];modul=[];
str=[handles.info.completesoundfilename];
op=handles.info.current_plot;
if op>1
	modus='PCP';
	modul=handles.info.current_pcp_module;
	str=[str ' - '  modus ': ' modul];
end
if op>2
	modus='BMM';
	modul=handles.info.current_bmm_module;
	str=[str ' - '  modus ': ' modul];
end
if op>3
	modus='NAP';
	modul=handles.info.current_nap_module;
	str=[str ' - '  modus ': ' modul];
end
if op>4
	modus='STROBES';
	modul=handles.info.current_strobes_module;
	str=[str ' - '  modus ': ' modul];
end
if op>5
	modus='SAI';
	modul=handles.info.current_sai_module;
	str=[str ' - '  modus ': ' modul];
end
if op>6
	modus='USER';
	modul=handles.info.current_usermodule_module;
	str=[str ' - '  modus ': ' modul];
end

namestr=str;