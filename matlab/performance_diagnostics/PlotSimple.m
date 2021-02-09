% File: PlotSimple.m
%
% Description: Plot a X,Y, or just Y function
% 
% Parameter(s): 
%     Filename - filename to save figure to
% fname = ['.\\Saved\\' Filename '_' Label '-' shortxlabel '.fig'];
%     Label - title for plot
%     LegendText - text for the legend, example: {'12W','12W2','12W3'}
%     xLabel - x label for plot
%     yLabel - y label for plot
%     X - optional, X values, array must be same length as Y, or else []
%     Y - Y values
%     InputStruct - optional struct to contain options and flags
% 
% Return value: void
% 
% Usage: 
% PlotSimple('1stTest-3rdBunch', 'Y Position', {'12W','12W2','12W3'}, 'x', 'y', [1:10], [10:-1:1; 11:-1:2; 12:-1:3], struct('YScale',[0 12]))

function [] = PlotSimple(Filename, Label, LegendText, xLabel, yLabel, X, Y, InputStruct)

%     If no InputStruct parameter, create a blank one
    if (nargin <8)
        InputStruct = [];
    end
    
%     Open a blank figure
    GetPlotInputStruct = [];
    if isfield(InputStruct,'GetPlot')
        GetPlotInputStruct = InputStruct.GetPlot;
    end 
    GetPlot(GetPlotInputStruct);

%     If No data, close the figure
%     You may wonder why I would open one and then close it. It is because
%     of legacy compatibility with how I place them on the screen - TODO fix this
    if (isempty(Y))
        ClosePlot()
        return
    end
    
%     If just one line to plot, plot it black
    sz = size(Y);
    if (sz(2) == 1)
        %              Colors selected: b = blue, see: help plot
        SetDefaultAxesColorOrder(gcf,'b');
    end

    plottedData = 0; 
%     if isfield(InputStruct,'semilogx')        
%         AX = semilogx(X,Y);
%          plottedData = 1;
%     elseif isfield(InputStruct,'semilogy')     
%         AX = semilogy(X,Y);
%          plottedData = 1;
%     else
%         plottedData = 0;        
%     end
    
    hold on    
    

%     if Scale specified, change axis scale and detect over/under flow for plot
    if isfield(InputStruct,'YScale')
        YScale = InputStruct.YScale;
        SetAxis([min(X) max(X) YScale(1) YScale(2)])
        DetectOverUnderFlow(X, Y)
    end 

    if (~plottedData)        
        if (isempty(X))
            AX = plot(Y);
            
        else
            AX = plot(X,Y);
        end
    end
    
    AddMarkerSymbols(AX,InputStruct)

    hold off

    ylabel(yLabel)
    xlabel(xLabel)
    if (~isempty(LegendText))
        legend(LegendText)
        if isfield(InputStruct,'LegendLocation')
            LegendLocation = InputStruct.LegendLocation;
            legend('Location',LegendLocation)
        end
    end
    
    shortxlabel = xLabel(1:min(length(xLabel),4));
    
%    add title to plot
    Title = sprintf('%s %s vs. %s',Filename, Label, shortxlabel);
    title(Title)
    
%    title the figure as well    
    f = gcf;
    set(f,'name',Title)    

%    SaveFigure([Filename '_' Label '-' shortxlabel])
end
