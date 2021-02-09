% File: GetOptimalSubPlotDimensions.m
%
% Description: Return optimal subplot dimensions, give a certain number of
% plots to plot
% 
% Parameter(s):
%     NumPanes - number of panes to plot 
% 
% Return value: 
%     a dimension array
% 
% Usage: 
% [dim] = GetOptimalSubPlotDimensions(7)


function [dim] = GetOptimalSubPlotDimensions(NumPanes)

    switch NumPanes
        case {1,2,3,4}
            dim = [NumPanes 1];
        case {5,6}
            dim = [3 2];
        case {7,8,9}
            dim = [3 3];
        case {10,11,12}
            dim = [4 3];
        case {13,14,15,16}
            dim = [4 4];
        otherwise
            d = ceil(sqrt(NumPanes));
            dim = [d d];
    end
    
end
