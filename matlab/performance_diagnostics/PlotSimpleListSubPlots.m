% File: PlotSimpleListSubPlots.m
%
% Description: Plot X vs. Y's: Y{1}, Y{2}, ...
% 
% Parameter(s): 
%     Filename - filename to save figure to
% fname = ['.\\Saved\\' Filename '_' Label '-' shortxlabel '.fig'];
%     Label - title for plot
%     LegendText1 - text for the legend, example: {'12W','12W2','12W3'}
%     LegendText2 - text for the legend, example: {'TO','TI','BI','BO'}
%     xLabel - x label for plot
%     yLabel - y label for plot
%     X - optional, X values, array must be same length as Y, or else []
%     Ys - Y values in array
%     TitleIn - common title used in each subplot
%     InputStruct - optional struct to contain options and flags
% 
% Return value: void
% 
% Usage: 
% PlotSimpleListSubPlots('1stTest-3rdBunch', 'Y Position', {'12W','12W2','12W3'}, {'TO','TI','BI','BO'}, 'x', 'y', [1:5], {rand(5), 10*rand(5), -10*rand(5)}, 'Common subplot title', struct('YScale',[-12 12]))

function [] = PlotSimpleListSubPlots(filename, Label, LegendText1, LegendText2, xLabel, yLabel, X, Ys, TitleIn, InputStruct)

%     If no InputStruct parameter, create a blank one
    if (nargin <10)
        InputStruct = [];
    end
    
%     if Scale specified, use it, otherwise determine a good min and max,
%     using the same min/max for each subplot, to aid in comparison
    if isfield(InputStruct,'YScale')
        YScale = InputStruct.YScale;
        minscl = YScale(1);
        maxscl = YScale(2);
    else
        [minscl,maxscl] = ComputeBiggestYScaleUsingMedian(Ys);  
    end 
    
    numYs = length(Ys);

%     Open a blank figure
    GetPlotInputStruct = [];
    if isfield(InputStruct,'GetPlot')
        GetPlotInputStruct = InputStruct.GetPlot;
    end 
    GetPlot(GetPlotInputStruct);
    
%     Set DefaultAxesColorOrder if specied: the order the colors are used
%     when plotting
    if isfield(InputStruct,'DefaultAxesColorOrder')
        DefaultAxesColorOrder = InputStruct.DefaultAxesColorOrder;
        set(gcf,'DefaultAxesColorOrder',DefaultAxesColorOrder);
    end
        
    [dim] = GetOptimalSubPlotDimensions(numYs);

    for i = 1:numYs
        
        if ~isfield(InputStruct,'Rotate')
            subplot(dim(1),dim(2),i)
        else
            subplot(dim(2),dim(1),i)
        end 
%          
        Y = (Ys{i});
        
        SetAxis([X(1) X(length(X)) minscl maxscl])
        DetectOverUnderFlow(X, Y)
        
%         try
        H = plot(X, Y,'.');
%         catch ME
%             rethrow(ME)
%         end
        AddMarkerSymbols(H)
        legend(LegendText2)
        hold off

        ylabel(yLabel)
        Title = sprintf([TitleIn ' %s'], LegendText1{i}); 
        title(Title)
    end
    
    xlabel(xLabel)
    shortxlabel = xLabel(1:min(length(xLabel),4));
    Title = sprintf('%s %s %s vs. %s', filename, TitleIn, Label,shortxlabel);
    f = gcf;
    set(f,'name',Title)    

%    SaveFigure([filename '_' Label TitleIn '-' shortxlabel])       
end
