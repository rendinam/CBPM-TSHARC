function varargout = bpm_fefpga_prog(varargin)
% BPM_FEFPGA_PROG M-file for bpm_fefpga_prog.fig
%      BPM_FEFPGA_PROG, by itself, creates a new BPM_FEFPGA_PROG or raises the existing
%      singleton*.
%
%      H = BPM_FEFPGA_PROG returns the handle to a new BPM_FEFPGA_PROG or the handle to
%      the existing singleton*.
%
%      BPM_FEFPGA_PROG('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in BPM_FEFPGA_PROG.M with the given input arguments.
%
%      BPM_FEFPGA_PROG('Property','Value',...) creates a new BPM_FEFPGA_PROG or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before bpm_fefpga_prog_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to bpm_fefpga_prog_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help bpm_fefpga_prog

% Last Modified by GUIDE v2.5 16-Jan-2012 14:58:13

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @bpm_fefpga_prog_OpeningFcn, ...
                   'gui_OutputFcn',  @bpm_fefpga_prog_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before bpm_fefpga_prog is made visible.
function bpm_fefpga_prog_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to bpm_fefpga_prog (see VARARGIN)

% Choose default command line output for bpm_fefpga_prog
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes bpm_fefpga_prog wait for user response (see UIRESUME)
% uiwait(handles.figure1);

set(handles.figure1,'CloseRequestFcn',@closeGUI);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% PROGRAM SPECIFIC STARTUP
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%load the cbi net libraries
addpath([getenv('ACC_RELEASE_DIR') '/cbi_net/matlab']);
ACC_Load_CBInet();

%initialize globals
global Hostname;
Hostname = 0;
global Programming;
Programming = 0;
global Hex_fname;
Hex_fname = 0;
global Socket;
Socket = 0;

%update log
log_string = 'Enter hostname to begin';
set (handles.LogBox,'String',log_string);


function closeGUI(src,evnt)
%src is the handle of the object generating the callback (the source of the event)
%evnt is the The event data structure (can be empty for some callbacks)
global Programming;

if (Programming ~= 0)
    selection = questdlg('CONNECTION STILL ACTIVE!!! Do you really want to close the gui?',...
                     'Close Request Function',...
                     'Yes','No','Yes');
    switch selection,
    case 'Yes',
        delete(gcf)
    case 'No'
        return
    end
else
    selection = questdlg('Do you really want to close the gui?',...
                     'Close Request Function',...
                     'Yes','No','Yes');
    switch selection,
    case 'Yes',
        delete(gcf)
    case 'No'
        return
    end
end


% --- Outputs from this function are returned to the command line.
function varargout = bpm_fefpga_prog_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in LoadHexButton.
function LoadHexButton_Callback(hObject, eventdata, handles)
% hObject    handle to LoadHexButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
%menu select the file to load
global Hex_fname;

[filename,pathname] = uigetfile('*.ace' );
Hex_fname = [pathname filename]
log_string = ['Using hex file: ',filename];
set (handles.LogBox,'String',log_string);
set (handles.LoadedFileBox,'String',filename);



% --- Executes on button press in ProgramFPGAButton.
function ProgramFPGAButton_Callback(hObject, eventdata, handles)
% hObject    handle to ProgramFPGAButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global Socket;
global Hostname;
global Hex_fname;
global Programming;

if (Hostname == 0)
    log_string = 'You must enter a hostname to begin';
    set (handles.LogBox,'String',log_string);
    return;
end

if (Hex_fname == 0)
    log_string = 'You must select a hex file to begin';
    set (handles.LogBox,'String',log_string);
    return;
end

if (Socket ~= 0)
    log_string = 'There is already a cbi port open!';
    set (handles.LogBox,'String',log_string);
    return;
end

Programming = 1;

log_string = 'Opening connection...!';
set (handles.LogBox,'String',log_string);

Socket = OpenPort_linux(Hostname);

AFE4_ProgramFPGA(handles, Hex_fname, Socket);

Socket = ClosePort_linux(Socket);

log_string = 'Programming complete, connection closed';
set (handles.LogBox,'String',log_string);

Programming = 0;


function LoadedFileBox_Callback(hObject, eventdata, handles)
% hObject    handle to LoadedFileBox (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of LoadedFileBox as text
%        str2double(get(hObject,'String')) returns contents of LoadedFileBox as a double


% --- Executes during object creation, after setting all properties.
function LoadedFileBox_CreateFcn(hObject, eventdata, handles)
% hObject    handle to LoadedFileBox (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function LogBox_Callback(hObject, eventdata, handles)
% hObject    handle to LogBox (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of LogBox as text
%        str2double(get(hObject,'String')) returns contents of LogBox as a double


% --- Executes during object creation, after setting all properties.
function LogBox_CreateFcn(hObject, eventdata, handles)
% hObject    handle to LogBox (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function HostnameBox_Callback(hObject, eventdata, handles)
% hObject    handle to HostnameBox (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of HostnameBox as text
%        str2double(get(hObject,'String')) returns contents of HostnameBox as a double
global Hostname;
Hostname = get(hObject,'String')
log_string = ['bpm hostname is ', Hostname];
set (handles.LogBox,'String',log_string);


% --- Executes during object creation, after setting all properties.
function HostnameBox_CreateFcn(hObject, eventdata, handles)
% hObject    handle to HostnameBox (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
