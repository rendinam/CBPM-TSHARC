% File: PlotSimpleSurface.m
%
% Description: Plot a 2D function as a surface
% 
% Parameter(s): 
%     Filename - filename to save figure to
% fname = ['.\\Saved\\' Filename '_' Label '-' shortxlabel '.fig'];
%     Label - title for plot
%     LegendText - text for the legend, example: {'12W','12W2','12W3'}
%     xLabel - x label for plot
%     yLabel - y label for plot
%     zLabel - z label for plot
%     X - optional, X values, array must be same length as Y, or else []
%     Y - Y values
%     Z - Z values
%     InputStruct - optional struct to contain options and flags
% 
% Return value: void
% 
% Usage: 
% ca;AX = PlotSimpleSurface('fname', 'Label', 'x', 'y', 'z', [], [], rand(7));
% ca;AX = PlotSimpleSurface('fname', 'Label', 'x', 'y', 'z', 1:7, 14:-1:8, rand(7));

function [AX] = PlotSimpleSurface(Filename, Label, xLabel, yLabel, zLabel, X, Y, Z, InputStruct)

    AX = [];
%     If no InputStruct parameter, create a blank one
    if (nargin <9)
        InputStruct = [];
    end
    
%     Open a blank figure
    GetPlotInputStruct = [];
    if isfield(InputStruct,'GetPlot')
        GetPlotInputStruct = InputStruct.GetPlot;
    end 
    
    GetPlot(GetPlotInputStruct);%plot(rand(4))
    AX = axes;

%     If no data, close the figure
%     You may wonder why I would open one and then close it. It is because
%     of legacy compatibility with how I place them on the screen - TODO fix this
    if (isempty(Z))
        ClosePlot()
        return
    end
    
    if (isempty(X) || isempty(Y))
        surf(Z);
    else
        surf(X,Y,Z);
    end
    
    hold off

    ylabel(yLabel)
    xlabel(xLabel)
    zlabel(zLabel)
    
%    add title to plot
    Title = sprintf('%s %s',Filename, Label);
    title(Title)
    
%    title the figure as well    
    f = gcf;
    set(f,'name',Title)    
    rotate3d on
    
    if isfield(InputStruct,'PlotTitle')
        PlotTitle = InputStruct.PlotTitle;
        A = axis;
        
        Zmin = A(5);
        Zrange = A(6)-A(5);
        
        textX = A(1); %0.025*(A(2)-A(1)) + A(1);
        textY = A(4); %0.95*(A(4)-A(3)) + A(3);
        textZ = Zmin + Zrange*1.2;
        output = sprintf('%s',PlotTitle);
%         text(textX,textY,output,'FontSize',24,'FontWeight','Bold')  
        text(textX,textY,textZ,output,'FontSize',24,'FontWeight','Bold')
    end      
end