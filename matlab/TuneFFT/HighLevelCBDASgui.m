
function varargout = HighLevelCBDASgui(varargin)
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @HighLevelCBDASgui_OpeningFcn, ...
                   'gui_OutputFcn',  @HighLevelCBDASgui_OutputFcn, ...
                   'gui_LayoutFcn',  [], ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
   gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end

function HighLevelCBDASgui_OpeningFcn(hObject, eventdata, handles, varargin)
handles.output = hObject;
guidata(hObject, handles);

function varargout = HighLevelCBDASgui_OutputFcn(hObject, eventdata, handles)
varargout{1} = handles.output;

function edit1_Callback(e1, eventdata, handles)

function edit1_CreateFcn(e1, eventdata, handles)
if ispc && isequal(get(e1,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(e1,'BackgroundColor','white');
end

function edit4_Callback(e4, eventdata, handles)

function edit4_CreateFcn(e4, eventdata, handles)
if ispc && isequal(get(e4,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(e4,'BackgroundColor','white');
end

function edit5_Callback(e5, eventdata, handles)

function edit5_CreateFcn(e5, eventdata, handles)
if ispc && isequal(get(e5,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(e5,'BackgroundColor','white');
end

function edit3_Callback(e3, eventdata, handles)

function edit3_CreateFcn(e3, eventdata, handles)
if ispc && isequal(get(e3,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(e3,'BackgroundColor','white');
end

function radiobutton1_CreateFcn(rb1,eventdata,handles)
set(rb1,'Value',1,'Enable','inactive');

function radiobutton1_Callback(rb1,eventdata,handles)
set(handles.radiobutton2,'Value',0,'Enable','on');
set(rb1,'Enable','inactive');

function radiobutton2_CreateFcn(rb2,eventdata,handles)

function radiobutton2_Callback(rb2,eventdata,handles)
set(handles.radiobutton1,'Value',0,'Enable','on');
set(rb2,'Enable','inactive');

function radiobutton5_CreateFcn(rb5, eventdata, handles)
set(rb5,'Value',1,'Enable','inactive');

function edit15_CreateFcn(e15, eventdata, handles)
if ispc && isequal(get(e15,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(e15,'BackgroundColor','white');
end

function edit15_Callback(e15, eventdata, handles)

function checkbox5_CreateFcn(ch5, eventdata, handles)

function checkbox5_Callback(ch5, eventdata, handles)

function checkbox6_CreateFcn(ch6, eventdata, handles)

function checkbox6_Callback(ch6, eventdata, handles)

function edit17_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit17_Callback(e17, eventdata, handles)

function edit18_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function checkbox7_Callback(hObject, eventdata, handles)

function edit20_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit20_Callback(e20, eventdata, handles)

function checkbox8_Callback(hObject, eventdata, handles)

function powerx_Callback(hObject, eventdata, handles)

function powerx_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function checkbox9_Callback(hObject, eventdata, handles)

function checkbox10_Callback(hObject, eventdata, handles)

function edit22_Callback(hObject, eventdata, handles)

function edit22_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit23_Callback(hObject, eventdata, handles)

function edit23_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function radiobutton9_CreateFcn(rb9,eventdata,handles)
set(rb9,'Value',1,'Enable','inactive');

function radiobutton9_Callback(rb9, eventdata, handles)
set(handles.radiobutton10,'Value',0,'Enable','on');
set(rb9,'Enable','inactive');

function radiobutton10_Callback(rb10, eventdata, handles)
set(handles.radiobutton9,'Value',0,'Enable','on');
set(rb10,'Enable','inactive');

function checkbox11_Callback(hObject, eventdata, handles)

function edit24_Callback(hObject, eventdata, handles)

function edit24_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function checkbox12_Callback(hObject, eventdata, handles)

function pushbutton3_Callback(hObject, eventdata, handles)
%The help button
str1='         High Level CESR Beam Dynamics Analysis Software Help';
str2='                Input File(s):  Multiple filenames may be entered in this field, separated by commas. ';
str3='                                         Number of header lines:  Input an integer here representing the number of lines your header takes up (enter 0 if none).  If you are running multiple files, still input one integer. (Additional functionality coming).';
str4='      Fit Data Plot Options:  This software plots the original data in addition to results.  This section of the GUI handles options for these plots.';
str5='           Fix y axis:  Check if you want the y axis for all plots of original data to be fixed.  Enter desired limits in the min and max fields below.  If running multiple files, you must enter limits for all files separated by commas or spaces.';
str6='                                                              Fix time axis: Check if you want the time axis for all plots of original data to be fixed.  Enter desired limits in the min and max fields below.  If running multiple files, you must enter limits for all files separated by commas or spaces.';
str7='                                                      FFT:  This section handles options for the FFT itself.   ';
str8='                    Turns Column: input the first column in which turn number information appears (counting from 1). For multiple files, enter one integer for each file, separated by commas.   ';
str9='                                               Data Column: Input the first column in which information for the parameter you wish to study appears.  For multiple files, enter one integer for each file, separated by commas. ';
str10='                                                 Check the Iterate and save figures for multiple bunches box if you want FFT results for more than one bunch in each file.   ';
str11='                                 Use gaussian window: Check if you want to filter the input data through a gaussian window (reducing the importance of points toward the beginning and end of your dataset).   ';
str12='                                                      If you want to analyze each file in its entirety, select the Analyze entire file option.  If not, select the Choose lines to analyze option.';
str13='                If choosing lines to analyze, enter line numbers in integers, starting from 1, in the Start at line and End at line fields.  Separate numbers for multiple files by commas.    ';
str14='                                                       If you want to analyze each bunch in all files, select the Analyze all bunches option.  If not, select the Choose bunches to analyze option.   ';
str15='                If choosing bunches to analyze, enter bunch numbers, starting from 1, in the Start at bunch and End at bunch fields.  Separate numbers for multiple files by commas.   ';
str16='                                                       Omit bunches: Check if there are bunches you wish to omit from your analysis.  Enter bunch numbers to omit in the field to the right.  Separate bunch numbers with commas.  If running multiple files, enclose a list of bunches to omit for each file in square brackets [].   ';
str17='                    Fit peaks to parabolas: Not yet available.   ';
str18='                                          You must enter limits on the frequencies you wish to consider as valid oscillations.  Unless the inverse plotting mode is toggled, the maximum range is 0 to .5.  Otherwise, the maximum range is .5 to 1.   ';
str19='           Print: Check if you want to print out the most likely oscillations found within the given range.  If checked, specify the number of frequencies you want printed (the software will print frequencies from the highest power to the lowest until it reaches the number you requested).   ';
str20='                             FFT Plot Options:  This section handles options for the plots of FFT results.';
str21='                                                                    Toggle inverse plotting mode:  Check if you want to plot frequency from .5 to 1 rather than 0 to .5.';
str22='                                                                    Fix power axis max: Check if you want the maximum on the power axis to be fixed for all bunches.  Enter the value in the field to the right.  For multiple files, separate inputs by commas.';
str23='                                              Also plot period in turns/cycle:  (RENAME!!) Plot the period in addition to the frequency of oscillation.';
str24='                                          Plot 3D with log scale:  Check if you want the final 3D plot of Bunch Vs. Frequency Vs. Power to be on a logarithmic scale.';
message=strcat(str1,str2,str3,str4,str5,str6,str7,str8,str9,str10,str11,str12,str13,str14,str15,str16,str17,str18,str19,str20,str21,str22,str23,str24);
msgbox(message,'Help');

function checkbox13_Callback(hObject, eventdata, handles)

function edit25_Callback(hObject, eventdata, handles)

function edit25_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit26_Callback(hObject, eventdata, handles)

function edit26_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function checkbox15_Callback(hObject, eventdata, handles)

function edit27_Callback(hObject, eventdata, handles)

function edit27_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit28_Callback(hObject, eventdata, handles)

function edit28_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function checkbox16_Callback(hObject, eventdata, handles)

function checkbox17_Callback(hObject, eventdata, handles)

function save_3d_log_Callback(hObject, eventdata, handles)

function plot_3D_Callback(hObject, eventdata, handles)

function save_3d_Callback(hObject, eventdata, handles)

function plot_full_p_Callback(hObject, eventdata, handles)

function save_power_full_Callback(hObject, eventdata, handles)

function plot_half_p_Callback(hObject, eventdata, handles)

function save_power_half_Callback(hObject, eventdata, handles)

function plot_raw_Callback(hObject, eventdata, handles)

function save_raw_Callback(hObject, eventdata, handles)

function plot_near_Callback(hObject, eventdata, handles)

function save_power_near_Callback(hObject, eventdata, handles)

function plot_tune_bunch_Callback(hObject, eventdata, handles)

function save_tune_bunch_Callback(hObject, eventdata, handles)

function plot_tunesize_bunch_Callback(hObject, eventdata, handles)

function save_tune_sigma_Callback(hObject, eventdata, handles)

function tune_expect_y_Callback(hObject, eventdata, handles)

function tune_expect_y_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function tune_shift_y_Callback(hObject, eventdata, handles)

function tune_shift_y_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function numbunch_Callback(hObject, eventdata, handles)

function numbunch_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function checkbox36_Callback(hObject, eventdata, handles)

function emptyb_Callback(hObject, eventdata, handles)

function emptyb_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function tuneVcurrent_Callback(hObject, eventdata, handles)

function saveTuneVcurrent_Callback(hObject, eventdata, handles)

function tune_expect_x_Callback(hObject, eventdata, handles)

function tune_expect_x_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function tune_shift_x_Callback(hObject, eventdata, handles)

function tune_shift_x_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function multf_Callback(hObject, eventdata, handles)

function startfile_Callback(hObject, eventdata, handles)

function startfile_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function endfile_Callback(hObject, eventdata, handles)

function endfile_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function powery_Callback(hObject, eventdata, handles)

function powery_CreateFcn(hObject, eventdata, handles)

if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function numturns_Callback(hObject, eventdata, handles)

function numturns_CreateFcn(hObject, eventdata, handles)

if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function print_Callback(hObject, eventdata, handles)

function printnum_Callback(hObject, eventdata, handles)

function printnum_CreateFcn(hObject, eventdata, handles)

if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function bpmnum_Callback(hObject, eventdata, handles)

function bpmnum_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
 
end

function checkbox99_Callback(hObject, eventdata, handles)

function checkbox100_Callback(hObject, eventdata, handles)

function pushbutton1_Callback(hObject, eventdata, handles)

addpath([getenv('ACC_RELEASE_DIR') '/cbpmfio/matlab']);                 %establish access to accelerator file access libraries
load_CBPM_input();
%addpath('/home/mcr/tmp/testing/cbpmfio/matlab');
%load_CBPM_input('/home/mcr/tmp/testing');
  
if (get(handles.multf,'Value')==1)                                          %get whether or not multiple files will be entered    

    startfl= str2double(char(get(handles.startfile,'string')));             %get the name of the starting file
    endfl=str2double(char(get(handles.endfile,'string')));                  %get the name of the end file

    numfiles=endfl-startfl;
    filem=startfl:endfl;
    infiles='';

    for i=1:numfiles
        
        infiles=strcat(infiles,num2str(filem(i)),',');                      %sets string of input filenames separated by ','
    
    end
    
    infiles=strcat(infiles,num2str(filem(end)));
    
else
    
    infiles = char(get(handles.edit1,'string'));                            %get user specified input filename(s)

end 

numturns = str2double(get(handles.numturns,'string'));                      %get number of turns in file

chooseturns = get(handles.radiobutton2,'Value');                            %get whether the user wants to choose turns in the file to analyze

if chooseturns                                                              %If the user wants to choose amount of turns...
    
    startt = str2double((get(handles.edit4,'string')));    
    endt = str2double((get(handles.edit5,'string')));
    
    turnm=startt:endt;                                                      %sets turn number matrix

else                                                                        %If the user isn't choosing a number of turns...
    
    turnm=1:numturns;                                                       %sets turn number matrix

end

numBunch=str2double(get(handles.numbunch,'string'));                        %get number of bunches

choosebunches = get(handles.radiobutton10,'Value');                         %get whether the user wants to choose specific bunches to analyze

if choosebunches                                                            %If the user wants to choose a range of bunches...
    
    startb = str2double((get(handles.edit22,'string')));  
    endb = str2double((get(handles.edit23,'string')));
    bunchvec=startb:endb;
    
    
else                                                                        %If the user isn't choosing a range of bunches...

    startb=1;
    endb=numBunch;
    bunchvec=startb:endb;

end

print=get(handles.print,'Value');                                           %get whether the user wants to print values to file

if print                                                                    %If the user wants to print periods/frequencies to file...
    
    printnum=str2double(get(handles.printnum,'string'));                    %get how many they want to print (in descending order from the highest power)

else                                                                        %If the user doesn't want to print any periods/frequencies to file...

    printnum=0;                                                             %set the number to print to 0

end

if (get(handles.checkbox8,'Value')==1)                                      %If the user wants to set a maximum on the power axis...
    
    powaxmaxstr=strcat(get(handles.powerx,'string'),',',...                 %get the maximum to set it at
                       get(handles.powery,'string'));

else                                                                        %If the user did not set a power axis maxinum...
    
    powaxmaxstr='0';                                                        %set it to 0 as a placeholder    

end

gausswindow=get(handles.checkbox16,'Value');                                %use gaussian window
parabfit=get(handles.checkbox17,'Value');                                   %fits to parabala

tune_expect_x=(get(handles.tune_expect_x,'string'));                        %finds expected tune and shift
tune_shift_x1=(get(handles.tune_shift_x,'string'));

if isempty(tune_expect_x) == 1
    
    tune_expect_x=0;
    tune_shift_x=[0 0];
    
else
    
    tune_shift1=str2double(tune_shift_x1);
    tune_expect_x=str2double(tune_expect_x);    
    
    for i=1:2
        
        tune_shift_x(i)= tune_expect_x + (-1)^i*tune_shift1;
           
    end
    
end

tune_expect_y=(get(handles.tune_expect_y,'string'));
tune_shift_y1=(get(handles.tune_shift_y,'string'));

if isempty(tune_expect_y) == 1
    
    tune_expect_y=0;
    tune_shift_y=[0 0];
    
else
    
    tune_shift1=str2double(tune_shift_y1);
    tune_expect_y=str2double(tune_expect_y);
    
    
    for i=1:2
        
        tune_shift_y(i)= tune_expect_y + (-1)^i*tune_shift1;
        
    
    end
    
end

tune_expect=[tune_expect_x tune_expect_y];                                  %expected tunes
tune_shift=[tune_shift_x; tune_shift_y];                                    %expected tune limits

plot_raw=get(handles.plot_raw,'Value');                                     %get whether to plot and save raw data
save_raw=get(handles.save_raw,'Value');
plot_full_p=get(handles.plot_full_p,'Value');                               %get whether to plot and save full power spectrum
save_power_full=get(handles.save_power_full,'Value');
plot_half_p=get(handles.plot_half_p,'Value');                               %get whether to plot and save power over nyquist
save_power_half=get(handles.save_power_half,'Value');
plot_3D=get(handles.plot_3D,'Value');                                       %get whether to plot and save 3D plot
save_3d=get(handles.save_3d,'Value');   
plot_near=get(handles.plot_near,'Value');                                   %get whether to plot and save plot near tune
save_power_near=get(handles.save_power_near,'Value');
plot_tune_bunch=get(handles.plot_tune_bunch,'Value');                       %get whether to plot and save tune vs. bunch
save_tune_bunch=get(handles.save_tune_bunch,'Value');
plot_phase=get(handles.checkbox100,'Value');
save_phase=get(handles.checkbox99,'Value');

plotvec=[plot_raw plot_full_p  plot_half_p...                               %array of which graphs to plots
         plot_3D plot_near plot_tune_bunch plot_phase];

saveplotvec=[save_raw save_power_full save_power_half...                    %array of which plots to save
             save_3d save_power_near save_tune_bunch save_phase];

fixy=get(handles.checkbox13,'Value');                                       %fixes vertical axis

if fixy

    ymin=(get(handles.edit25,'string'));                                    %sets minimim on vertical axis
    ymax=(get(handles.edit26,'string'));                                    %sets maximum on vertical axis

else
    
    ymin='none';
    ymax='none';

end


fixt=get(handles.checkbox15,'Value');                                       %fixes horizontal time axis

if fixt
    
    tmin=str2double(get(handles.edit27,'string'));                          %sets minimum on time axis
    tmax=str2double(get(handles.edit28,'string'));                          %sets maximum on time axis

else
    
    tmin=-1;
    tmax=-1;

end

if (get(handles.checkbox11,'Value')==1)                                     %If the user wants to omit specific bunches...
    
    omitstr=get(handles.edit24,'string'); omitnum=1;                        %get the string containing the list of bunches to omit
    
    while ~isempty(omitstr)
        
        [omitm1,omitstr]=strtok(omitstr,',');                               %sets the array of omitted bunches
        omitbunch(omitnum)=str2double(omitm1);
        omitnum=omitnum+1;
        
    end
    
    for i=1:(omitnum-1)
        
        A=find(bunchvec==omitbunch(i));
        bunchvec(A)=[];

    end
 
end

bpmnum=str2double(get(handles.bpmnum,'string'));                            %get bpm number

FFTfilehandler2(infiles,turnm,numturns,ymin,ymax,tmin,tmax,powaxmaxstr,...
               bunchvec,gausswindow,parabfit,plotvec,saveplotvec,...
               tune_expect,tune_shift,numBunch,printnum,bpmnum,1)
 
