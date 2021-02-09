% File: ClosePlot.m
%
% Description: close a figure
%
% Note: Function also sets up some globals to allow other functions to do fancier stuff with plots.
% returns a handle to it.
% 
% Parameter(s): fig - optional - to specify figure index
%               InputStruct - unused currently, place holder
% 
% Return value: handle to the figure opened
% 
% Usage: 
% f = ClosePlot(17);

function [f] = ClosePlot(fig,InputStruct)

%     If no InputStruct parameter, create a blank one
    if (nargin<2)
        InputStruct = [];
    end
    
%     If no input paramenter, gcf (Get Current Figure) to close
    if (nargin == 0)
        fig = gcf;
    end
            
%     Signify that this figure has been closed, so when calling
%     ShowAllPlots, for instance, a blank figure is not opened for this
%     index number
    global SpecialFigureStruct
    SpecialFigureStruct(fig).IsClosed = 1;
    
    close(fig)
end
