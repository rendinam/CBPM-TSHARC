% File: SetPositionForNextPlot.m
%
% Description: change the position index for the next figure to be created
% by GetPlot
% 
% Parameter(s): InputStruct - optional struct to contain options and flags
% 
% Return value: next position index to use
% 
% Usage: 
% position_index = SetPositionForNextPlot(struct('RelativePositionIndex', 2));


function [position_indexOut] = SetPositionForNextPlot(InputStruct)

    %     If no parameter, create a blank one
    if (nargin==0)
        InputStruct = [];
    end

    global position_index
    PositionIndexWasEmpty = 0;
    if (isempty(position_index))
        position_index = 1;
        PositionIndexWasEmpty = 1;
    end

    global position_index
    if (isfield(InputStruct,'AbsolutePositionIndex'))
        position_index = InputStruct.AbsolutePositionIndex;
    elseif (isfield(InputStruct,'RelativePositionIndex'))
        position_index = position_index + InputStruct.RelativePositionIndex;
    elseif (isfield(InputStruct,'PutNextPlotOnTopOfCurrentPlot'))
%         do nothing
    else
        if ~PositionIndexWasEmpty
            position_index = position_index + 1;
        end
    end 
    
    position_indexOut = position_index;

end
