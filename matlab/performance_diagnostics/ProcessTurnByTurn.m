% File: ProcessTurnByTurn.m
%
% Description: Plot Turn by Turn Data
% 
% Parameter(s): fileid - index number of raw file to read in
% 
% Return value: 
%     C - Cell Array containing all data read in
% 
% Usage: 
% 
% ca;ProcessTurnByTurn;
% ca;ProcessTurnByTurn(3190);

function [C] = ProcessTurnByTurn(fileid)

    SetupCbpmfileioAPI
    
%   Initialize Data cell array
    C = {0,0};    
        
    StartingPath = '/nfs/cesr/online/instr/data/cbpmII/raw/*.dat';
    if (nargin < 1)
        [FILENAME, ~, ~] = uigetfile(StartingPath, 'Select RD file');

%         If not file selected, break loop
        if (length(FILENAME) == 1)
            return
        end

%         Extract raw data file index
        start = strfind(FILENAME,'-') + 1;
        finish = strfind(FILENAME,'.') - 1;
        fileid = str2double(FILENAME(start:finish));        
    end

%         If Index is less than 4080, use old method of file parsing, since
%         some of these files formats aren't supported by
%         ACC_Load_CBPM_input
    if (fileid < 4080 )

        n = floor(fileid/1e3);
        str = sprintf('%3.2f',n/1e2);
        directory= str(3:4);

        str = sprintf('%6.6f',fileid/1e6);
        ID = str(3:8);
        PATHNAME = ['/nfs/cesr/online/instr/data/cbpmII/raw/' directory '/'];
        FILENAME = ['RDe-' ID '.dat'];             

%             open file
        fid = fopen([PATHNAME FILENAME],'r');
        fprintf('%s ',FILENAME)

%             parse file
        line = fgetl(fid);
        fprintf( '%s\n' ,line);
        i = 1;
        while (line~=-1)

            if (strfind(line,'Location') ~= -1)
                fprintf('%s',line(22:length(line)))
            end

            if (strfind(line,'# --- Bunch 1 ---') ~= -1)
                [C{i}] = textscan(fid, '%f%f%f%f%f%f');
                i = i + 1;
            end

            line = fgetl(fid);
        end
        fclose(fid);

        BPM1 = C{1};    
        num_insts = 1;

        isA2ndBPM = (length(C)>1 && length(C{2})>1);   
        if (isA2ndBPM)
            BPM2 = C{2};
            num_insts = 2;
        end

        isAThirdBPM = length(C)>2;      
        if (isAThirdBPM)
            BPM3 = C{3};
            num_insts = 3;
        end

    else

        if (~onLinux)
            error('Must run on linux for file indicies this new')
        end

        fprintf('RD-%.6i.dat \n',fileid)
        
        %             open file
        result = cbpm_read_rawfile_m( fileid );

%             count instruments
        num_insts = cbpm_file_num_insts_m()

%             Collect BPM1 data
        BPM1name = cbpm_file_inst_locs_m()
        fdata = cbpm_file_tbt_data_m( BPM1name);
        BPM1 = {0};
        for i = 1:4
            BPM1{i} = fdata(:,i);
        end

        pos = cbpm_file_pos_data_m(BPM1name);
        for i = 1:2
            BPM1{4+i} = pos(:,i);
        end

        C = {0};
        C{1} = BPM1;

%             Collect BPM2 data            
        if (num_insts > 1)
            BPM2name = cbpm_file_inst_locs_m()
            fdata = cbpm_file_tbt_data_m( BPM2name);
            BPM2 = {0};
            for i = 1:4
                BPM2{i} = fdata(:,i);
            end

            pos = cbpm_file_pos_data_m(BPM2name);
            for i = 1:2
                BPM2{4+i} = pos(:,i);
            end                

            C{2} = BPM2;
        end

%             Collect BPM3 data                
        if (num_insts > 2)
            BPM3name = cbpm_file_inst_locs_m()
            fdata = cbpm_file_tbt_data_m( BPM3name);
            BPM3 = {0};
            for i = 1:4
                BPM3{i} = fdata(:,i);
            end

            pos = cbpm_file_pos_data_m(BPM3name);
            for i = 1:2
                BPM3{4+i} = pos(:,i);
            end                

            C{3} = BPM3;
        end            
    end %if fileid    
    
    fname = sprintf('workspace-%i.mat',fileid);
    save(fname)
    %return
    
%     if num_insts < 3
%        disp('Error!!! Not enough BPMs.')
%        return
%     end      

%     Separate out Buttons
    TI = BPM1{1};
    BI = BPM1{2};
    BO = BPM1{3};
    TO = BPM1{4};

%     Compute position
    X = BPM1{5};
    Y = BPM1{6};
    %SUM = TI + BI + BO + TO;

%     Compute fractional buttons
    %TIF = TI./SUM;
    %BIF = BI./SUM;
    %BOF = BO./SUM;
    %TOF = TO./SUM;

%     Compute button mean
    Buttons = [TI BI BO TO];
    ButtonMean = mean(Buttons);
    for i = 1:4
        fprintf('%6.0f ',ButtonMean(i))
    end
    fprintf('  ')    

%     Compute button std
    ButtonStd = std(Buttons);
    for i = 1:4
        fprintf('%6.0f ',ButtonStd(i))
    end
    fprintf('  ')  

%         Compute mean position
    Pos = [X Y];
    PosMean = mean(Pos);
    for i = 1:2
        fprintf('%6.0f um ',PosMean(i)*1e6)
    end        

%         Compute std position
    PosStd = std(Pos);
    for i = 1:2
        fprintf('%6.0f um ',PosStd(i)*1e6)
    end           

    fprintf('\n')            
    
    Filename = sprintf('%i ',fileid);
    
    %PlotSimple(Filename,'1-X',{},'x','y',[],1e6*(BPM1{5}))
    %PlotSimple(Filename,'1-Y',{},'x','y',[],1e6*(BPM1{6}))
    
    Buttons = [TI BI BO TO];
%     ButtonsFFT = fft(Buttons);
%     ButtonsAmp = abs(ButtonsFFT);
%     ButtonsAngle = angle(ButtonsFFT);
    
    %n = 4; Mn = 0; for i=1:floor(1024/n); Mn(i) = round(mean(TI(1+(i-1)*n:n*i))); end ; GetPlot;plot(Mn,'.')
    
%     PlotSimple(Filename,'Amplitude',{},'frequency','Amplitude',[],ButtonsAmp(length(ButtonsAmp)/2:length(ButtonsAmp),:))
    %PlotSimple(Filename,'Phase',{},'frequency','Phase',[],ButtonsAngle* 360/(2*pi()))  
    
    if (0)
            Xs_old = 1e6*[BPM2{5} BPM3{5} BPM1{5}];
            Ys_old = 1e6*[BPM2{6} BPM3{6} BPM1{6}];
            
            limit = min(2560000/16, length(BPM1{5}));
            
            T = 1:limit;        
            Xs = Xs_old(1:limit,:);
            Ys = Ys_old(1:limit,:);
            
            Xzs = LineUpStarts(Xs);
            Yzs = LineUpStarts(Ys);

            P0x = 1/3 * sum(Xzs,2);
            P0y = 1/3 * sum(Yzs,2);

            l = [-.305 0 .305] * 1e6; % (um/mm)
            P0xPrime = sum(  (Xzs .* (ones(length(Xzs),1)*l)),2  ) / sum(l.^2);
            P0yPrime = sum(  (Yzs .* (ones(length(Ys),1)*l)),2  ) / sum(l.^2);

            PResidx = Xzs - (P0x*ones(1,3)) - (P0xPrime * l);
            PResidy = Yzs - (P0y*ones(1,3)) - (P0yPrime * l);

            ChiSqrx = (1/3 * sum( PResidx.^2,2)).^0.5;
            ChiSqry = (1/3 * sum( PResidy.^2,2)).^0.5;

            FILENAME = sprintf('%i ',fileid);
            BPMNameIn = {'12W2','12W3','12W (4)'};
            TimeLegend = 'Turns';

            [minscl,maxscl] = ComputeYScaleUsingMedian(Yzs);
            PlotSimple(FILENAME, 'Y Position', BPMNameIn, TimeLegend, 'Position(Microns)', T, Yzs, struct('YScale',[minscl maxscl], 'semilogx',1))

            [minscl,maxscl] = ComputeYScaleUsingMedian(PResidy);
            PlotSimple(FILENAME, 'Y Position Linear Residuals', BPMNameIn, TimeLegend, 'Residuals(Microns)', T, PResidy, struct('YScale',[minscl maxscl], 'semilogx',1))

            [minscl,maxscl] = ComputeYScaleUsingMedian(P0yPrime);
            PlotSimple(FILENAME, 'Y Position Linear Slope', [], TimeLegend, 'Slope', T, P0yPrime, struct('YScale',[minscl maxscl], 'semilogx',1))

            [minscl,maxscl] = ComputeYScaleUsingMedian(P0y);
            PlotSimple(FILENAME, 'Y Position Constant', [], TimeLegend, 'Slope', T, P0y, struct('YScale',[minscl maxscl], 'semilogx',1))

            [minscl,maxscl] = ComputeYScaleUsingMedian(ChiSqry);
            PlotSimple(FILENAME, 'Y Linear ChiSqrd', [], TimeLegend, 'ChiSqrd', T, ChiSqry, struct('YScale',[minscl maxscl], 'semilogx',1))
            

            [minscl,maxscl] = ComputeYScaleUsingMedian(Xzs);
            PlotSimple(FILENAME, 'X Position', BPMNameIn, TimeLegend, 'Position(Microns)', T, Xzs, struct('YScale',[minscl maxscl], 'semilogx',1))

            [minscl,maxscl] = ComputeYScaleUsingMedian(PResidx);
            PlotSimple(FILENAME, 'X Position Linear Residuals', BPMNameIn, TimeLegend, 'Residuals(Microns)', T, PResidx, struct('YScale',[minscl maxscl], 'semilogx',1))

            [minscl,maxscl] = ComputeYScaleUsingMedian(P0xPrime);
            PlotSimple(FILENAME, 'X Position Linear Slope', [], TimeLegend, 'Slope', T, P0xPrime, struct('YScale',[minscl maxscl], 'semilogx',1))

            [minscl,maxscl] = ComputeYScaleUsingMedian(P0x);
            PlotSimple(FILENAME, 'X Position Constant', [], TimeLegend, 'Slope', T, P0x, struct('YScale',[minscl maxscl], 'semilogx',1))

            [minscl,maxscl] = ComputeYScaleUsingMedian(ChiSqrx);
            PlotSimple(FILENAME, 'X Linear ChiSqrd', [], TimeLegend, 'ChiSqrd', T, ChiSqrx, struct('YScale',[minscl maxscl], 'semilogx',1))        
            return
    end    
    
    PlotSimple(Filename,[BPM1name '-Buttons'],{'B1','B2','B3','B4'},'x','y',[],[BPM1{1} BPM1{2} BPM1{3} BPM1{4}]);
    PlotSimple(Filename,[BPM2name '-Buttons'],{'B1','B2','B3','B4'},'x','y',[],[BPM2{1} BPM2{2} BPM2{3} BPM2{4}]);
    try; PlotSimple(Filename,[BPM3name '-Buttons'],{'B1','B2','B3','B4'},'x','y',[],[BPM3{1} BPM3{2} BPM3{3} BPM3{4}]); catch; end
    return
    PlotSimple(Filename,'1-X',{},'x','y',[],1e6*(BPM1{5}))
    PlotSimple(Filename,'1-Y',{},'x','y',[],1e6*(BPM1{6}))
    PlotSimple(Filename,'2-X',{},'x','y',[],1e6*(BPM2{5}))
    PlotSimple(Filename,'2-Y',{},'x','y',[],1e6*(BPM2{6}))
    try; PlotSimple(Filename,'3-X',{},'x','y',[],1e6*(BPM3{5})); catch; end
    try; PlotSimple(Filename,'3-Y',{},'x','y',[],1e6*(BPM3{6})); catch; end
    return
    
    if (num_insts > 1)

        PlotSimple(Filename,'1-2-X',{},'x','y',[],1e6*(BPM1{5} - BPM2{5}))
        PlotSimple(Filename,'1-2-Y',{},'x','y',[],1e6*(BPM1{6} - BPM2{6}))
    
    end
        
    if (num_insts > 2)

        PlotSimple(Filename,'2-3-X',{},'x','y',[],1e6*(BPM2{5} - BPM3{5}))            
        PlotSimple(Filename,'2-3-Y',{},'x','y',[],1e6*(BPM2{6} - BPM3{6}))            
        PlotSimple(Filename,'1-3-X',{},'x','y',[],1e6*(BPM1{5} - BPM3{5}))            
        PlotSimple(Filename,'1-3-Y',{},'x','y',[],1e6*(BPM1{6} - BPM3{6})) 
    end

end

% Old Code

%         PlotHist(TI,TIF)
%         PlotHist(BI,BIF)
%         PlotHist(BO,BOF)
%         PlotHist(TO,TOF)
        
%         YC = BPM1(6);
%         Y = YC{1};
%         GetPlot;
%         hist(YY,300)
%         i = 1;
        
%         PlotConvergenceOfStdSqr(FILENAME, 1e6*(BPM1{6}-BPM2{6}));
%         PlotConvergenceOfStdExp(FILENAME, 1e6*(BPM1{6}-BPM2{6}));
%         PlotConvergenceOfStdLin(FILENAME, 1e6*(BPM1{6}-BPM2{6}));

%             PlotTBTData(FILENAME,'1-2_X', BPM1{5}-BPM2{5});
%             PlotTBTData(FILENAME,'1-2_Y', BPM1{6}-BPM2{6});
% 
%             PlotTBTData(FILENAME,'1_X', BPM1{5});
%             PlotTBTData(FILENAME,'1_Y', BPM1{6});       
%             PlotTBTData(FILENAME,'2_X', BPM2{5});
%             pause(1)
%             PlotTBTData(FILENAME,'2_Y', BPM2{6}); 

% function [] = PlotHist(Vals1,Vals2)
%     GetPlot;
%     [N,X] =hist(Vals1,200);
% %     [N,X] = hist(max(-5,min(5,Vals1)),200);
%     plot(X,N);
% %     axis([X(round(length(X)*2/3)) X(length(X)) 0 max(N)*1.1])
% 
%     GetPlot;
%     [N,X] = hist(max(-5,min(5,Vals2)),200);
% %     [N,X] = hist(Vals2,200);
%     plot(X,N)
% end
% 
% function [] = PlotConvergenceOfStdLin(filename,Vals)
%    GetPlot;
%        
%     N = length(Vals);
%     I = Vals;
%     DivN = floor(N/100);
%     for i = 2:DivN
%         try
%             I(i) = std(Vals(1:i*100));
%         catch
%             i = i
%         end
%     end
%     plot(I(10:DivN))
% 
% %     xlabel('Log base 1.1 of Number of Points Included')
%     ylabel('Std Computed')
% 
%     Title = sprintf('%s Convergence of Std, Lin',filename); 
%     title(Title)
%     f = gcf;
%     set(f,'name',Title)
% 
%     fname = ['.\\Saved\\' filename '_LinConv.fig'];
%     saveas(gcf, fname, 'fig')    
% end
% 
% function [] = PlotConvergenceOfStdSqr(filename,Vals)
%    GetPlot;
%        
%     N = length(Vals);
%     I = Vals;
%     SqrtN = floor(sqrt(N));
%     for i = 2:SqrtN
%         try
%             I(i) = std(Vals(1:i^2));
%         catch
%             i = i
%         end
%     end
%     plot(I(10:SqrtN))
% 
% %     xlabel('Log base 1.1 of Number of Points Included')
%     ylabel('Std Computed')
% 
%     Title = sprintf('%s Convergence of Std, Sqr',filename); 
%     title(Title)
%     f = gcf;
%     set(f,'name',Title)
% 
%     fname = ['.\\Saved\\' filename '_SqrConv.fig'];
%     saveas(gcf, fname, 'fig')    
% end
% 
% function [] = PlotConvergenceOfStdExp(filename,Vals)
%    GetPlot;
%        
%     N = length(Vals);
%     I = Vals;
%     B = 1.1;
%     LogN = round(log(N)/log(B));
%     for i = 2:LogN
%         try
%             I(i) = std(Vals(1: min( round(B^i), N)));
%         catch
%             i = i;
%         end
%     end
%     plot(I(10:LogN))
% 
%     xlabel('Log base 1.1 of Number of Points Included')
%     ylabel('Std Computed')
% 
%     Title = sprintf('%s Convergence of Std, 2^x',filename); 
%     title(Title)
%     f = gcf;
%     set(f,'name',Title)
% 
%     fname = ['.\\Saved\\' filename '_ExpConv.fig'];
%     saveas(gcf, fname, 'fig')    
% end
% 
% function [Mean, Stdev] = PlotData(filename,Label,Var1, Var2)
% 
%     GetPlot;
%     X = 1e6*(Var1 - Var2);
% 
%     hist(X,50)
%     xlabel('Position(microns)')
%     ylabel('Count')
% 
%     Mean = mean(X);
%     Stdev = std(X); 
% 
%     Title = sprintf('%s, %s, Mean: %f microns, Std: %f microns',filename, Label, Mean, Stdev); 
%     title(Title)
%     f = gcf;
%     set(f,'name',Title)
% 
%     fname = ['.\\Saved\\' filename '_' Label '.fig'];
%     saveas(gcf, fname, 'fig')
% end
% 
% function [] = PlotTBTData(filename,Label,Var1)
% 
%     GetPlot;
%     X = 1e6*(Var1);
% 
%     plot(X)
%     xlabel('Turn')
%     ylabel('Microns')
% 
%     Title = sprintf('%s, %s',filename, Label); 
%     title(Title)
%     f = gcf;
%     set(f,'name',Title)
% 
%     fname = ['.\\Saved\\' filename '_' Label '_TBT.fig'];
%     saveas(gcf, fname, 'fig')
% end

function [Data, const] = LineUpStarts(DataIn)
% Shift data over so that each variable starts roughly near zero
    sz = size(DataIn);
    const = median(DataIn(1:ceil(0.05*length(DataIn)),:));
    if (sz(2) ~= length(const))
        const = DataIn(1,:);
    end
    Data = (DataIn - ones(sz(1),1) * const);
end
