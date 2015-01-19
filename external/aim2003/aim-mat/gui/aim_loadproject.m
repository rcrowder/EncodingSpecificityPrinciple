% procedure for 'aim-mat'
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% load the signal file and all files, that are in this directory
% set the project variables accordingly.
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/03/10 13:42:38 $
% $Revision: 1.15 $


function handles=aim_loadproject(handles,signame);


signalname=handles.info.signalname;
signalwavename=handles.info.signalwavename;
oldsignalwavename=handles.info.oldsignalwavename;
pcpname=handles.info.pcpname;
bmmname=handles.info.bmmname;
napname=handles.info.napname;
strobesname=handles.info.strobesname;
thresholdsname=handles.info.thresholdsname;
sainame=handles.info.sainame;
usermodulename=handles.info.usermodulename;


was_conflict=0;
% load the parameterfile (and apply the parameters)
if fexist(handles.info.parameterfilename)
	workdir=pwd;
	cd(handles.info.directoryname);
	[a,parfile,c,d]=fileparts(handles.info.parameterfilename);
	clear all_options;
	new_options=handles.all_options;
	
	try % desperate...
		eval(parfile); % this produces all_options as 'all_options'
		[new_options,conflicts]=aim_mixstruct(all_options,new_options);
		
		handles.info.conflicts=conflicts;
		handles.all_options=new_options;
		cd(workdir);
		if ~structisequal(new_options,all_options)
			aim_saveparameters(handles,handles.info.parameterfilename);	
		end
	catch
		disp('problems with reading in old parameter file. Check parameters manually!');
		% 		new_options=all_options;
		conflicts='problem reading in parameter file. Check parameters!';
		cd(workdir);
		was_conflict=1;
	end
else
	was_conflict=1;
end

if fexist(signalname)
	[sig,type,sigoptions]=aim_loadfile(signalname);
 	handles.data.signal=sig;
	if ~isempty(sigoptions)
		handles.all_options.signal=sigoptions;
		handles.data.original_signal=loadwavefile(signal,handles.info.signalwavename);
	else
		handles.all_options.signal.start_time=0;
		handles.all_options.signal.duration=getlength(sig);
		handles.all_options.signal.samplerate=getsr(sig);
		handles.all_options.signal.original_start_time=0;
		handles.all_options.signal.original_duration=getlength(sig);
		handles.all_options.signal.original_samplerate=getsr(sig);
		% for old projects
		if exist(handles.info.originalwavename)
			handles.data.original_signal=loadwavefile(signal,handles.info.originalwavename);
		else
			handles.data.original_signal=loadwavefile(signal,handles.info.signalwavename);
		end
	end		
	
	% put the original signal in its place:
else   
	% cant continue without signal!
	str=sprintf('No signal-object found in the project %s',signalname);
	er=errordlg(str,'File Error');
	set(er,'WindowStyle','modal');
	handles.error=1;
	return
end

if fexist(pcpname)
	[pcp,type,options]=aim_loadfile(pcpname);
	handles.data.pcp=pcp;
	handles.info.pcp_loaded=1;
	handles.info.calculated_pcp_module=type;	% this one is really calculated
% 	select(handles.listbox0,type);
	if ~isempty(type)
		str=sprintf('handles.all_options.pcpoptions.%s=options;',type);
		eval(str);
	else    % old style
		type='ELC';
		handles.all_options.pcpoptions=options;
		sai_savefile(pcp,pcpname,type,options,handles.all_options);
	end
	handles.info.init.calculated_pcp_module=type;
else
	handles.info.pcp_loaded=0;
end

if fexist(bmmname)
	[bmm,type,options]=aim_loadfile(bmmname);
	handles.data.bmm=bmm;
	handles.info.bmm_loaded=1;
	handles.info.calculated_bmm_module=type;	% this one is really calculated
			
% 	select(handles.listbox1,type);
	handles.info.init.calculated_bmm_module=type;

else
	handles.info.bmm_loaded=0;
end
if fexist(napname)
	[nap,type,options]=aim_loadfile(napname);
	handles.data.nap=nap;
	handles.info.nap_loaded=1;
% 	select(handles.listbox2,type);
	handles.info.init.calculated_nap_module=type;

else
	handles.info.nap_loaded=0;
end
if fexist(strobesname)
	[strobes,type,options]=aim_loadfile(strobesname);
	if fexist(thresholdsname)
		thresholds=aim_loadfile(thresholdsname);
		handles.data.thresholds=thresholds;
	end
	handles.data.strobes=strobes;
	handles.info.strobes_loaded=1;
	handles.info.calculated_strobes_module=type;	% this one is really calculated
% 	select(handles.listbox3,type);
	handles.info.init.calculated_strobes_module=type;
else
	handles.info.strobes_loaded=0;
end

if fexist(sainame)
	[sai,type,options]=aim_loadfile(sainame);
	handles.data.sai=sai;
	handles.info.sai_loaded=1;
	handles.info.calculated_sai_module=type;	% this one is really calculated
% 	select(handles.listbox4,type);
	handles.info.init.calculated_sai_module=type;

	nr_frames=length(sai);
	handles.slideredit_frames.minvalue=1;
	handles.slideredit_frames.maxvalue=nr_frames;
	% set the framecounter
% 	handles.slideredit_frames=slidereditcontrol_set_value(handles.slideredit_frames,nr_frames);	% set to the end
% 	handles.slideredit_frames=slidereditcontrol_set_range(handles.slideredit_frames,nr_frames);	% the duration
	
else
	handles.info.sai_loaded=0;
end

if fexist(usermodulename)
	[usermodule,type,options]=aim_loadfile(usermodulename);
	handles.data.usermodule=usermodule;
	handles.info.usermodule_loaded=1;
	handles.info.calculated_usermodule_module=type;	% this one is really calculated
	handles.info.init.calculated_usermodule_module=type;

% 	select(handles.listbox6,type);
	handles.info.usermodule_loaded=1;
else
	handles.info.usermodule_loaded=0;
end

% TODO: only the first available movie is loaded. The others not
handles.info.movie_loaded=0;
% if length(moviename)>1
% 	for i=1:length(moviename)
% 		cname=moviename{i};
% 		if fexist(cname)
% 			handles.info.movie_loaded=1;
% 			handles.info.calculated_movie_module=cname;	% this one is really calculated
% 			handles.info.init.calculated_movie_module=type;
% 			break
% 		end
% 	end
% end


if was_conflict==1
	disp('parameter file does not exist. Taking standart parameters. Check parameters manually!');
	disp('saving standart parameters as new parameter file');
	handles=aim_saveparameters(handles,handles.info.parameterfilename,1);
end


function select(hand,what)
str=get(hand,'String');
for i=1:length(str)
	if strcmp(str,what)
		set(hand,'Value',i);
		return
	end
end
return

