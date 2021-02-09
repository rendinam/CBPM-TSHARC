function [] = Test_Everything(InputStruct)
 
    clc

%     If no InputStruct parameter, create a blank one
    if (nargin <1)
        InputStruct = [];
    end
     
     
    %ca;ProcessProcData('U:\Concatenated_Data\3BPMs\92724-93427-tripletN.txt');
    
    %ca;ProcessTurnByTurn('U:\MATLAB Workspaces\workspace-9212.mat');
    
    %ca;ProcessTSAndCCScan('U:\tsharc\tscan\41\TS-041913-038.dat');
    %ca;ProcessTScan('U:\tsharc\tscan\41\TS-041913-038.dat'); 

    %Disp20110505

    %Disp20110509E

    %Disp20110509P

    %Disp20110510

    %DisplayFigOnebyOne

    DisplayTripletCCData(41900)

    DispSpecialFFT

    DispSpecialHist(1:3)

    DispSpecialOrbit(12686,'Synch AC OFF',struct('DiffFileId',12688,'Diff2FileId',12678));place=[3 4;4 10;5 5;6 11];for i = 1:length(place);figure(place(i,1));PutPlotAtIndex(place(i,2));end

    disp('Test Complete')
end

function [testEnabled,i] = DetermineEnableStatus(i,InputStruct)

    ca
    
%     If no InputStruct parameter, create a blank one
    if (nargin <2)
        InputStruct = [];
    end

    testEnabled = 1;
    i = i+1;
    
    if isfield(InputStruct,'SkipTo')
        SkipTo = InputStruct.SkipTo;
        
        testEnabled = i>= SkipTo;
    end
    
    if testEnabled
        z = 1;
    end
end
