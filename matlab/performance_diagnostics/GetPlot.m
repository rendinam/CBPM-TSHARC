% File: GetPlot.m
%
% Description: open a figure
%
% Note: Function also sets up some globals to allow other functions to do fancier stuff with plots.
% returns a handle to it.
% 
% Parameter(s): InputStruct - optional struct to contain options and flags
% 
% Return value: handle to the figure opened
% 
% Usage: 
% fig = GetPlot(struct('PlotIsyy',1));

function [f] = GetPlot(InputStruct)

%     If no parameter, create a blank one
    if (nargin==0)
        InputStruct = [];
    end
    
%     Whether or not to spread plots around on the screen
    global spreadoutplots
    if (isempty(spreadoutplots))
        spreadoutplots = 1;
    end

%     The index of the last figure created. (will be incremented by this function
    global lastfignum
    if (isempty(lastfignum))
        lastfignum = 0;
    end
    if lastfignum == 100 %30
        fprintf('*** Too many plots ***\n')
        error('Too many plots')
    end
    lastfignum = lastfignum + 1;
        
    global SpecialFigureStruct
    if (isfield(InputStruct,'PlotIsyy'))
        SpecialFigureStruct(lastfignum).PlotIsyy = 1;
    end

    f = figure(lastfignum); 
    
%     Index of position of last plot spread around screen
    [position_index] = SetPositionForNextPlot(InputStruct);

%     global position_index
%     PositionIndexWasEmpty = 0;
%     if (isempty(position_index))
%         position_index = 1;
%         PositionIndexWasEmpty = 1;
%     end
%     
%     if (isfield(InputStruct,'AbsolutePositionIndex'))
%         position_index = InputStruct.AbsolutePositionIndex;
%     elseif (isfield(InputStruct,'RelativePositionIndex'))
%         position_index = position_index + InputStruct.RelativePositionIndex;
%     elseif (isfield(InputStruct,'PutNextPlotOnTopOfCurrentPlot'))
% %         do nothing
%     else
%         if ~PositionIndexWasEmpty
%             position_index = position_index + 1;
%         end
%     end    
    
    PositionsX = [0 570 1140 2000 2570 3140 ];     
    PositionsY = [700 202];
    
    numX = 3;
    numY = 2;
    x = mod(position_index-1,numX)+1;
    y = mod(floor((position_index-1)/numX),2)+1;
    set(gcf,'Position',[PositionsX(x) PositionsY(y) 560 420])
end
