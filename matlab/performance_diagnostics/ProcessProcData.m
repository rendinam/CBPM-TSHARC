% File: ProcessProcData.m
%
% Description: Plot Concantenated CBPM Process Data files
% 
% Parameter(s): - all optional
%     FULLFILENAME, 
%     InputToCallee - optional struct to contain options and flags
%     StartMin, 
%     FinishMin, 
%     OptionalTimeScanTimesFileName
% 
% Return value: 
%     Trange - Time range of read in files
%     C - Cell Array containing all data read in
% 
% Usage: 
% First on a linux terminal, use concat_proc_files.py, to create a concatenated process data file, for this M file to read in.
% Type ./concat_proc_files.py for usage instructions
% 
% ca;ProcessProcData;
% ca;ProcessProcData('',struct('doPlot', true, 'ChangeTtoP', true));


function [] = ProcessProcData(FULLFILENAME, InputToCallee)

% Position coefficients
    y_coeff = 0.028800 * 1e6;
    x_coeff = 0.022300 * 1e6;

%   Save original Default color order
    origDefault = GetDefaultAxesColorOrder;
    
%     If no InputStruct parameter, create a blank one
    if (nargin <2)
        InputToCallee = [];
    end    
    
%     Sometimes don't plot some of the plots
    doPlot = 1;
    if (isfield(InputToCallee,'doPlot'))
        doPlot = InputToCallee.doPlot;
    end

%    Initialize Variables
    Trange = []; % Range of time values read in from file to be specified
    
%     if no input parameters, or specified fild name is empty, the query user for file to read in
    if ((nargin == 0) || (isempty(FULLFILENAME)))
        [FILENAME, PATHNAME, ~] = uigetfile('*.txt', 'Select Data File');
        
%         if no file returned, assume user pressed cancel and exit function
        if (length(FILENAME) == 1)
            return
        end
        FULLFILENAME = [PATHNAME FILENAME];
        fprintf('%s\n', FULLFILENAME);
    end
    
%     Open file
    fid = fopen(FULLFILENAME,'r');
    f = strfind(FULLFILENAME,'.txt');
    if ~isempty(f)
        FILENAME = FULLFILENAME(1:f-1);
    else
        FILENAME = FULLFILENAME;
    end
    
%     Trim file name down to relevant part to show in plot titles
    g = strfind(FILENAME,'\');
    if ~isempty(g)
        FILENAME = FILENAME(g(length(g))+1:length(FILENAME));
    end
    g = strfind(FILENAME,'/');
    if ~isempty(g)
        FILENAME = FILENAME(g(length(g))+1:length(FILENAME));
    end

%     Read in file
    line = fgetl(fid);
    len = length(line);
    try
        if (len >8) && strcmp(line(1:8), 'Version-')
            Version = str2double(line(9:length(line)));
            line = fgetl(fid);
        elseif strcmp(line(1:5), 'BPM1-')
            Version = str2double(line(6:length(line)));
        else
            Version = 1;
        end
    catch ME
        disp(ME)
        Version = 1;
    end
    
%     Before version 4 of concat files, X, and Y position were calculated incorrectly
    if (Version < 4)
        fprintf( '******** WARNING, Position will not be calculated correctly ************* \n\n\n\n\n **************************************\n');
    end
    
%     Parse rest of file
    i = 1;
    C = {0};
    BPMNameIn = {0};
    while (line~=-1)
        if Version == 1
            [C{i}] = textscan(fid, '%f%f%f%f%f%f%f%f%f');
        elseif Version == 2
            [C{i}] = textscan(fid, '%f%f%f%f%f%f%f%f%f%f%f%f%f');
        elseif Version <= 5
            [C{i}] = textscan(fid, '%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f');        
        else
            [C{i}] = textscan(fid, '%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f');        
        end
        i = i + 1;
        if (Version > 4)
            BPMNameIn{i-1} = line(5: length(line));
        end
        line = fgetl(fid);
    end
    fclose(fid);    
    
% Parse out the BPMNames
    if (Version > 4) && (length(BPMNameIn{length(BPMNameIn)}) > 10)
        newBPMNameIn{length(BPMNameIn)-1} = '';
        newC{length(BPMNameIn)-1} = [];
        for i = 1:length(BPMNameIn)-1
            newBPMNameIn{i} = BPMNameIn{i};
            newC{i} = C{i};
        end
        C = newC;
        BPMNameIn = newBPMNameIn;
    end      
    
    C = TrimCellArray(C);

    T = C{1}{1};
    [T,~] = ZeroTheArray(T); % shift time array, so that first point is T=0
    
% If no BPMNames to read in, create some names
    if (Version <= 4)
        len = length(C);
        BPMNameIn{len} = '';
        for i = 1:len
            BPMNameIn{i} = sprintf('BPM%i',i);
        end
    end       
    numBPMsReadIn = length(BPMNameIn);    
    
%     Extract separate out each BPM, and the X and Y position values from data
    BPM{1} = C{1};
    % Xs or Ys = 1e6 um/mm * BPM1(mm)
    [T,Xs] = TrimArrays(T,              1e6* BPM{1} {2});
    [T,Ys] = TrimArrays(T,              1e6* BPM{1} {3});    
    
    BPM{numBPMsReadIn} = ones(1);
    for i = 2:numBPMsReadIn
        if (length(C)>1)
            BPM{i} = C{i};
            if ~isempty(BPM{i}{1})
                % Xs or Ys = 1e6 um/mm * BPM{i}(mm)
                [T,Xs] = TrimAndCombineArrays(T, Xs,1e6* BPM{i} {2});
                [T,Ys] = TrimAndCombineArrays(T, Ys,1e6* BPM{i} {3});
            end
        else
            numBPMsReadIn = i;
            break;
        end
    end    
    
    Current = BPM{1}{9};
    Index = BPM{1}{8};
    [Current, Ys] = TrimArrays(Current, Ys);
    [Current, Index] = TrimArrays(Current, Index);    
    
%     Parse Button RMSs
     RMSss = {0};
     for i = 1:numBPMsReadIn
        RMSss{i} = ([BPM{i}{4} BPM{i}{5} BPM{i}{6} BPM{i}{7}]);
        [Index,RMSss{i}] = TrimArrays(Index, RMSss{i});       
     end
     
%      Parse Channel delays
     if Version>1
        ChDelayss = {0};
        for i = 1:numBPMsReadIn
             ChDelayss{i} = ([BPM{i}{10} BPM{i}{11} BPM{i}{12} BPM{i}{13}]);
        end
     end    
     
%      Parse button values
    if Version >2
        ButtonValss{length(BPMNameIn)} = 0;
        ButtonVals1 = ([BPM{1}{14} BPM{1}{15} BPM{1}{16} BPM{1}{17}]);
        [~,ButtonVals1] = TrimArrays(Index, ButtonVals1);
        ButtonValss{1} = ButtonVals1;
        [T,ButtonsSum] = TrimArrays(T, sum(ButtonVals1,2));
        [T,ButtonsCrossDiff] = TrimArrays(T, (ButtonVals1(:,1) + ButtonVals1(:,3)) - (ButtonVals1(:,2) + ButtonVals1(:,4)));
        
        for i = 2:numBPMsReadIn
            ButtonValss{i} = ([C{i}{14} C{i}{15} C{i}{16} C{i}{17}]);
            [~,ButtonValss{i}] = TrimArrays(Index, ButtonValss{i});
            
            if (~isfield(InputToCallee,'DontApplyGainCorrection'))
                [ButtonValss{i},~] = ApplyGainCorrectionToButtons(BPMNameIn{i},ButtonValss{i});
            end   
        
            [T,ButtonsSum] = TrimAndCombineArrays(T, ButtonsSum,sum(ButtonValss{i},2));
            [T,ButtonsCrossDiff] = TrimAndCombineArrays(T, ButtonsCrossDiff, (ButtonValss{i}(:,1) + ButtonValss{i}(:,3)) - (ButtonValss{i}(:,2) + ButtonValss{i}(:,4)));            
        end
        
        InitArray = zeros(length(ButtonValss{1}),numBPMsReadIn);
        TI = InitArray;
        BI = InitArray;
        BO = InitArray;
        TO = InitArray;
        for i = 1:numBPMsReadIn
            ButtonsSum(:,i) = sum(ButtonValss{i},2);

            TI(:,i) = ButtonValss{i}(:,1);
            BI(:,i) = ButtonValss{i}(:,2);
            BO(:,i) = ButtonValss{i}(:,3);
            TO(:,i) = ButtonValss{i}(:,4);

            ButtonsCrossDiff(:,i) = (TI(:,i)+BO(:,i)) - (BI(:,i)+TO(:,i));

            Xs(:,i) = x_coeff * (-TI(:,i)+TO(:,i)-BI(:,i)+BO(:,i)) ./ ButtonsSum(:,i);
            Ys(:,i) = y_coeff * ( TI(:,i)+TO(:,i)-BI(:,i)-BO(:,i)) ./ ButtonsSum(:,i);
        end         
        
    end
    
%     X/Y zeroed s
    Xzs = LineUpStarts(Xs);
    Yzs = LineUpStarts(Ys);      
    
%     Parse Temperatures
    if Version >5
        TempValss = {0};
        for i = 1:numBPMsReadIn
            TempValss{i} = ([C{i}{18} C{i}{19} C{i}{20} C{i}{21} C{i}{22}]);
            [Index,TempValss{i}] = TrimArrays(Index, TempValss{i});
            AvgTemp = mean(TempValss{i});
        end

        NumTempSensors = 5;
        TempSensorName = {'Digital', 'C1', 'C2', 'C3', 'C4'};%, 'Timing'};

        ZeroedTempValss{length(BPMNameIn)} = 0;
        TempDiffValss{length(BPMNameIn)} = 0;
        MeanishValss{length(BPMNameIn)} = 0;
        for i = 1:numBPMsReadIn
            ZeroedTempValss{i} = [TempValss{i}] - (TempValss{i}(:,1) * ones(1,NumTempSensors));
            ZeroedTempValss{i} = LineUpMeans(ZeroedTempValss{i},1);
            TempDiffValss{i} = LineUpMeans(TempValss{i},1);
        end    
    end     
     
    ProcDataFileNum = 'Process Datafile Index';
    % Choose Time Legend  
    ChangeTtoP = 0;
    if (isfield(InputToCallee,'ChangeTtoP'))
        ChangeTtoP = InputToCallee.ChangeTtoP;
    end    

    if (~ChangeTtoP)
        TimeLegend = 'Time(minutes)';
        TSetToProcFileNum = 0;
    else
        T = Index;
        TimeLegend = ProcDataFileNum;
        TSetToProcFileNum = 1;
    end
    
    Trange = [T(1) T(length(T))];   
    
    
%     PLOTTING SECTION OF FILE ***********************************************
    
    if (doPlot); PlotSimple(FILENAME, 'Time', {}, ProcDataFileNum, TimeLegend, Index, T); end    
    
    if Version >1
        
    %To compare to of the BPMS 2 the third
    IndexOfBPMtoSubtract = 2;
    BPMSubtractedTitlePiece = sprintf(' withBPM%iSubtracted',IndexOfBPMtoSubtract);        
        
% ******************* Y *****************************        
        [minscl,maxscl] = ComputeYScaleUsingMedian(Yzs);
        if (~TSetToProcFileNum && doPlot)       
            PlotSimple(     FILENAME, 'Y',         BPMNameIn, ProcDataFileNum, 'Position(Microns)', Index, Yzs, struct('YScale', [minscl maxscl], ...
                'GetPlot',struct('PutNextPlotOnTopOfCurrentPlot', 1)))
            
            PlotSimpleYYPPD(FILENAME, 'Y and Current',         BPMNameIn, ProcDataFileNum, 'Current(mA)', 'Position(Microns)', Index, Current, Yzs, struct('YScale1', [minscl maxscl], ...
                'GetPlot',struct('PutNextPlotOnTopOfCurrentPlot', 1)))
        end
        
        PlotSimpleSubPlots(FILENAME, 'Y Position by BPM', BPMNameIn, TimeLegend, 'Pos(um)', T, Yzs, '', ...
            struct('RelativePositionIndex', 0, 'DefaultAxesColorOrder', 'b', 'GetPlot', struct('PutNextPlotOnTopOfCurrentPlot', 1)))

        if (numBPMsReadIn >1)
            ZeroedY = Yzs - (Yzs(:,IndexOfBPMtoSubtract) * ones(1,numBPMsReadIn));
            [Zminscl,Zmaxscl] = ComputeYScaleUsingMedian(ZeroedY);

                    PlotSimple(     FILENAME, ['Y' BPMSubtractedTitlePiece],         BPMNameIn, TimeLegend, 'Position(Microns)', T, ZeroedY, struct('YScale', [Zminscl Zmaxscl], ...
            'GetPlot',struct('PutNextPlotOnTopOfCurrentPlot', 1)))
        end
        
        PlotSimple(     FILENAME, 'Y',         BPMNameIn, TimeLegend, 'Position(Microns)', T, Yzs, struct('YScale', [minscl maxscl], ...
            'GetPlot',struct('PutNextPlotOnTopOfCurrentPlot', 1)))

        PlotSimpleYYPPD(FILENAME, 'Y and Current',         BPMNameIn, TimeLegend, 'Current(mA)', 'Position(Microns)', T, Current, Yzs, struct('YScale1', [minscl maxscl], ...
            'GetPlot',struct('PutNextPlotOnTopOfCurrentPlot', 1)))
            
% ******************* X *****************************   
        [minscl,maxscl] = ComputeYScaleUsingMedian(Xzs);

        SetPositionForNextPlot;
        
        if (~TSetToProcFileNum && doPlot)       
            PlotSimple(     FILENAME, 'X',         BPMNameIn, ProcDataFileNum, 'Position(Microns)', Index, Xzs, struct('XScale', [minscl maxscl], ...
                'GetPlot',struct('PutNextPlotOnTopOfCurrentPlot', 1)))
            
            PlotSimpleYYPPD(FILENAME, 'X and Current',         BPMNameIn, ProcDataFileNum, 'Current(mA)', 'Position(Microns)', Index, Current, Xzs, struct('XScale1', [minscl maxscl], ...
                'GetPlot',struct('PutNextPlotOnTopOfCurrentPlot', 1)))
        end
        
        PlotSimpleSubPlots(FILENAME, 'X Position bX BPM', BPMNameIn, TimeLegend, 'Pos(um)', T, Xzs, '', ...
            struct('RelativePositionIndex', 0, 'DefaultAxesColorOrder', 'b', 'GetPlot', struct('PutNextPlotOnTopOfCurrentPlot', 1)))
        
        if (numBPMsReadIn >1)
            ZeroedX = Xzs - (Xzs(:,IndexOfBPMtoSubtract) * ones(1,numBPMsReadIn));
            [Zminscl,Zmaxscl] = ComputeYScaleUsingMedian(ZeroedX);

                    PlotSimple(     FILENAME, ['X' BPMSubtractedTitlePiece],         BPMNameIn, TimeLegend, 'Position(Microns)', T, ZeroedX, struct('YScale', [Zminscl Zmaxscl], ...
            'GetPlot',struct('PutNextPlotOnTopOfCurrentPlot', 1)))
        end        

        PlotSimple(     FILENAME, 'X',         BPMNameIn, TimeLegend, 'Position(Microns)', T, Xzs, struct('XScale', [minscl maxscl], ...
            'GetPlot',struct('PutNextPlotOnTopOfCurrentPlot', 1)))

        PlotSimpleYYPPD(FILENAME, 'X and Current',         BPMNameIn, TimeLegend, 'Current(mA)', 'Position(Microns)', T, Current, Xzs, struct('XScale1', [minscl maxscl], ...
            'GetPlot',struct('PutNextPlotOnTopOfCurrentPlot', 1)))
    end
    
%     Change Color order back to default
    SetDefaultAxesColorOrder(0,origDefault);
end

% ************************ Associated functions ******************************************************************

% function [minscl,maxscl] = ComputeYScaleUsingMedian(Y, FactorIn)
% % Compute a Min and Max for data, using the median and quartiles of the data
% 
%         Q = Quartile(Y);
%         
%         q2 = min(Q(2,:));
%         medianval = mean(Q(3,:));
%         q3 = max(Q(4,:));
%         Meanq2q3 = mean([q2 q3]);
%         
%         DefaultFactor = 4;
%         Factor = DefaultFactor;
%         if ((nargin >1) && (~isempty(FactorIn)))
%             Factor = FactorIn;
%             if (FactorIn == -1)
%                 Factor = DefaultFactor;
%             end
%         end
%         
%         minscl = medianval - (Meanq2q3-q2)*Factor;
%         maxscl = medianval + (q3-Meanq2q3)*Factor;
% end
% 
% function [minscl,maxscl] = ComputeBiggestYScaleUsingMedian(List, FactorIn)
% % Compute a Min and Max for data list, using the median and quartiles of the of each array in the data list
% 
%     if nargin <2
%         FactorIn = [];
%     end
% 
%     len = length(List);
%     try
%         minsclA = zeros(1,len);
%         maxsclA = zeros(1,len);
%         for i = 1:len
%             [minsclA(i),maxsclA(i)] = ComputeYScaleUsingMedian(List{i},FactorIn);
%         end
%         minscl = min(minsclA);
%         maxscl = max(maxsclA);    
%     catch ME
%         disp(ME)
%         [minscl,maxscl] = ComputeYScaleUsingMedian(List,FactorIn);
%     end
% 
% end

function [Arr1, Arr2] = TrimArrays(Arr1in, Arr2in)
% Trim both arrays so they are the same length
    Arr1 = Arr1in;
    Arr2 = Arr2in;
    length1 = length(Arr1);
    length2 = length(Arr2);
    
    if length1 == length2
%         i = 1;
    elseif length1 < length2
        Arr2 = Arr2(1:length1,:);
    else
        Arr1 = Arr1(1:length2,:);
    end
end

function [ArrayOut] = TrimCellArray(Array)
% Trim Cell Array down to length of shortest array, in a cell
    try
        minlen = length(Array{1}{1});
        len = length(Array);
        for i = 1:len
            lenj = length(Array{i});
            for j = 1:lenj
                newminlen = min(minlen,length(Array{i}{j}));
                if (newminlen > 0)
                    minlen = newminlen;
                end
            end
        end

        for i = 1:len
            lenj = length(Array{i});
            if (~isempty(Array{i}{j}))
                for j = 1:lenj
                        ArrayRow{j}(:,:) = Array{i}{j}(1:minlen,:);
                end
                ArrayOut{i} = ArrayRow;
            end
        end    
    catch ME
        rethrow(ME)
    end
end

function [T,Result] = TrimAndCombineArrays(Tin, Arr1, Arr2)
% Trim both arrays so they are the same length then combine them
    length1 = length(Arr1);
    length2 = length(Arr2);
    
    if length1 < length2
        Arr2 = Arr2(1:length1,:);
        T = Tin;
    else
        Arr1 = Arr1(1:length2,:);
        T = Tin(1:length2);
    end
    Result = [Arr1 Arr2];
end

function [List] = TrimList(start,finish,ListIn)
% Trim all array in list so they are the same length
    if finish == -1
        finish = length(ListIn{1});
    end
    len = length(ListIn);
    List = ListIn;
    for i = 1:len
        Arr = ListIn{i};
        List{i} = Arr(start:min(finish,length(Arr)));
    end
end

function [Data, const] = ZeroTheArray(DataIn)
% Shift data over so that each variable starts at 0
    len = length(DataIn);
    const = DataIn(1,:);
    Data = (DataIn - ones(len,1) * const);
end

function [Data, const] = LineUpStarts(DataIn)
% Shift data over so that each variable starts roughly near zero
    len = length(DataIn);
    const = median(DataIn(1:ceil(0.05*length(DataIn)),:));
    if (len < length(const))
        const = DataIn(1,:);
    end
    Data = (DataIn - ones(len,1) * const);
end

function [Data, const] = LineUpMeans(DataIn,OptionalDoRoundMean)
% Shift data over so that the means for all variables are near zero
    len = length(DataIn);
    const = mean(DataIn);
    if (length(const) < len)
        const = DataIn(1,:);
    end
    if (nargin > 1)
        if (OptionalDoRoundMean)
            const = round(const);
        end
    end
    Data = (DataIn - ones(len,1) * const);
end

function [] = PlotSimpleYYPPD(filename, Label, LegendText, xLabel, yLabel1, yLabel2, X, Y1, Y2, InputStruct)

%     If no InputStruct parameter, create a blank one
    if (nargin <10)
        InputStruct = [];
    end

    origDefault = GetDefaultAxesColorOrder;
%                                   put another blue at beginning
    SetDefaultAxesColorOrder(0,['b' GetDefaultAxesColorOrder]);

    InputStruct.AddMarkerSymbolsForY2 = 1;
    PlotSimpleYY(filename, Label, {'Current'}, LegendText, xLabel, yLabel1, yLabel2, X, Y1, Y2, InputStruct)
    
    %                                  r     b   g       c         m        y          k  
    SetDefaultAxesColorOrder(0,origDefault);
    
end
