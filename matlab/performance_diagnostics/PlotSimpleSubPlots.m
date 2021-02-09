% File: PlotSimpleSubPlots.m
%
% Description: Plot X vs. Y's: Y(:,1), Y(:,2), ...
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
% PlotSimpleSubPlots('1stTest-3rdBunch', 'Y Position', {'TO','TI','BI','BO'}, 'x', 'y', [1:10], [10:-1:1; 11:-1:2; 12:-1:3; 13:-1:4]', 'Common title for subplot', struct('YScale',[0 12]))

function [] = PlotSimpleSubPlots(filename, Label, LegendText, xLabel, yLabel, X, Ys, TitleIn, InputStruct)

%     If no InputStruct parameter, create a blank one
    if (nargin <9)
        InputStruct = [];
    end
    
%     if Scale specified, use it, otherwise determine a good min and max,
%     using the same min/max for each subplot, to aid in comparison
    if isfield(InputStruct,'YScale')
        YScale = InputStruct.YScale;
        minscl = YScale(1);
        maxscl = YScale(2);
    else
        [minscl,maxscl] = ComputeYScaleUsingMedian(Ys);  
    end 
    
    numYs = size(Ys,2);

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
        SetDefaultAxesColorOrder(gcf,DefaultAxesColorOrder);
    end
        
    [dim] = GetOptimalSubPlotDimensions(numYs);

    for i = 1:numYs
        
        if ~isfield(InputStruct,'Rotate')
            subplot(dim(1),dim(2),i)
        else
            subplot(dim(2),dim(1),i)
        end 
%          
        Y = Ys(:,i);
        
        SetAxis([X(1) X(length(X)) minscl maxscl])
        DetectOverUnderFlow(X, Y)
        
%         try
        H = plot(X, Y,'.');
%         catch ME
%             rethrow(ME)
%         end
        AddMarkerSymbols(H)
        hold off

        ylabel(yLabel)
        Title = sprintf([TitleIn ' %s'], LegendText{i}); 
        title(Title)
    end
    
    xlabel(xLabel)
    shortxlabel = xLabel(1:min(length(xLabel),4));
    Title = sprintf('%s %s %s vs. %s', filename, TitleIn, Label,shortxlabel);
    f = gcf;
    set(f,'name',Title)    

%    SaveFigure([filename '_' Label TitleIn '-' shortxlabel]) 
end
