function varargout = length_questionbox(varargin)
% LENGTH_QUESTIONBOX M-file for length_questionbox.fig
%      LENGTH_QUESTIONBOX, by itself, creates a new LENGTH_QUESTIONBOX or raises the existing
%      singleton*.
%
%      H = LENGTH_QUESTIONBOX returns the handle to a new LENGTH_QUESTIONBOX or the handle to
%      the existing singleton*.
%
%      LENGTH_QUESTIONBOX('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in LENGTH_QUESTIONBOX.M with the given input arguments.
%
%      LENGTH_QUESTIONBOX('Property','Value',...) creates a new LENGTH_QUESTIONBOX or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before length_questionbox_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to length_questionbox_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help length_questionbox

% Last Modified by GUIDE v2.5 23-Apr-2003 09:32:26

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @length_questionbox_OpeningFcn, ...
                   'gui_OutputFcn',  @length_questionbox_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin & isstr(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before length_questionbox is made visible.
function length_questionbox_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to length_questionbox (see VARARGIN)

% Choose default command line output for length_questionbox
handles.output = hObject;

old_start_time=varargin{1};
old_duration=varargin{2};
handles.old_start_time=old_start_time;
handles.old_duration=old_duration;
handles.max_duration=varargin{3};
text=varargin{4};
set(handles.text1,'String',text{1});
set(handles.text3,'String',text{2});

handles.output(1)=old_start_time;
handles.output(2)=old_duration;

set(handles.edit1,'String',fround(old_start_time*1000,0));
set(handles.edit2,'String',fround(old_duration*1000,0));


% Update handles structure
guidata(hObject, handles);

% do modal
% UIWAIT makes length_questionbox wait for user response (see UIRESUME)
uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = length_questionbox_OutputFcn(hObject, eventdata, handles)
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output(1);
varargout{2} = handles.output(2);
close(handles.figure1);


function edit1_CreateFcn(hObject, eventdata, handles)
if ispc    set(hObject,'BackgroundColor','white');else    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));end
function edit2_CreateFcn(hObject, eventdata, handles)
if ispc    set(hObject,'BackgroundColor','white');else    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));end





%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%Start time
function edit1_Callback(hObject, eventdata, handles)
duration=str2num(get(handles.edit2,'String'))/1000;
start_time=str2num(get(handles.edit1,'String'))/1000;
if isempty(start_time)
	start_time=handles.old_start_time;
end
if start_time+duration>handles.max_duration
	start_time=handles.max_duration-duration;
end
set(handles.edit1,'String',num2str(fround(start_time*1000,0)));

handles.output(1)=start_time;
guidata(hObject, handles);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Duration
function edit2_Callback(hObject, eventdata, handles)
duration=str2num(get(handles.edit2,'String'))/1000;
start_time=str2num(get(handles.edit1,'String'))/1000;
if isempty(duration)
	duration=handles.old_duration;
end
if duration+start_time>handles.max_duration
	duration=handles.max_duration-start_time;
end
set(handles.edit2,'String',num2str(fround(duration*1000,0)));

handles.output(2)=duration;
guidata(hObject, handles);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
handles.output(1)=str2num(get(handles.edit1,'String'))/1000;
handles.output(2)=str2num(get(handles.edit2,'String'))/1000;
guidata(hObject, handles);
uiresume;

% --- Executes on button press in pushbutton2.
function pushbutton2_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
handles.output(1)=handles.old_start_time;
handles.output(2)=handles.old_duration;
guidata(hObject, handles);
uiresume;


