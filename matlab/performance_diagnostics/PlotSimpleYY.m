% File: PlotSimpleYY.m
%
% Description: Plot a 2 X,Y functions on the same plot
% 
% Parameter(s): 
%     Filename - filename to save figure to
% fname = ['.\\Saved\\' Filename '_' Label '-' shortxlabel '.fig'];
%     Label - title for plot
%     LegendText - text for the legend, example: {'12W','12W2','12W3'}
%     xLabel - x label for plot
%     yLabel1 - y label for function 1
%     yLabel2 - y label for function 2
%     X - X values, array must be same length as Y
%     Y1 - Y values for function 1
%     Y2 - Y values for function 2
%     InputStruct - optional struct to contain options and flags
% 
% Return value: void
% 
% Usage: 
% PlotSimpleYY('1stTest-3rdBunch', 'Y Position', {'12W','12W2','12W3'}, {'13W','14W','15W'}, 'X', 'y1', 'y2', 1:10, ...
%     [10:-1:1; 11:-1:2; 12:-1:3]', [1:10; 2:11; 3:12]', struct('YScale',[0 12], 'AddMarkerSymbolsForY2',1))

function [] = PlotSimpleYY(filename, Label, LegendText, LegendText2, xLabel, yLabel1, yLabel2, X, Y1, Y2, InputStruct)

    if ~isMonotonicallyIncreasing(X)
        fprintf('Didn''t plot with this X array, since it was not monotonically increasing.\n');
        return;
    end
    
%     If no InputStruct parameter, create a blank one
    if (nargin <11)
        InputStruct = [];
    end

%     Open a blank figure
    GetPlotInputStruct.PlotIsyy = 1;
    if isfield(InputStruct,'GetPlot')
        GetPlotInputStruct = InputStruct.GetPlot;
    end 
    GetPlot(GetPlotInputStruct);

    if (isempty(Y1) || isempty(Y2))
        ClosePlot
        return
    end
    
    [AX, H1, H2] = plotyy(X,Y1,X,Y2);
    legend(LegendText{:}, LegendText2{:})
    
    if isfield(InputStruct,'AddMarkerSymbolsForY1')
        AddMarkerSymbols(H1)
    end
    
    if isfield(InputStruct,'AddMarkerSymbolsForY2')
        AddMarkerSymbols(H2)
    end
    
    if isfield(InputStruct,'YScale1')
        YScale1 = InputStruct.YScale1;
        A = axis(AX(1));
        SetAxis([A(1:2) YScale1(1:2)],struct('SpecifiedAxes', AX(2)))
        DetectOverUnderFlow(X,Y2,struct('SpecifiedAxes',AX(2), 'PlottingAxes', AX(1)))
    end

    if isfield(InputStruct,'YScale2')
        YScale2 = InputStruct.YScale2;
        A = axis(AX(2));
        SetAxis([A(1:2) YScale2(1:2)],struct('SpecifiedAxes', AX(2)))
        DetectOverUnderFlow(X,Y2,struct('SpecifiedAxes',AX(2), 'PlottingAxes', AX(1)))
    end

    xlabel(AX(1),xLabel)
    ylabel(AX(1),yLabel1)
    ylabel(AX(2),yLabel2)
    
    shortxlabel = xLabel(1:min(length(xLabel),4));
    Title = sprintf('%s %s vs. %s',filename, Label, shortxlabel);
    title(Title)    
    f = gcf;
    set(f,'name',Title)    

%	SaveFigure([filename '_' Label '-' shortxlabel])           
end

function [itIs] = isMonotonicallyIncreasing(X)
    dX = ceil(diff(X));
    itIs = isempty(find(dX<-1,1));
%     if (~itIs)
%         i = 1;
%     end
end
