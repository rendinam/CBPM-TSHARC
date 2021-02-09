% File: SetAxis.m
%
% Description: call matlab function "axis" to set xmin,xmax,ymin,ymax
% 
% Parameter(s): 
%     AxisValues
%     InputStruct - optional struct to contain options and flags
% 
% Return value: void
% 
% Usage: 
% X = 1:10;
% Y = 11:20;
% GetPlot;plot(X,Y,'.')
% SetAxis([min(X) max(X) min(Y) max(Y)])

function [] = SetAxis(AxisValues,InputStruct)

%     If no InputStruct parameter, create a blank one
    if (nargin <2)
        InputStruct = [];
    end
    
    xmin = AxisValues(1);
    xmax = AxisValues(2);
    xrange = xmax-xmin;
    ymin = AxisValues(3);
    ymax = AxisValues(4);
    yrange = ymax-ymin;
    
    if (xrange > 0) && (yrange > 0)
        
%         if InputStruct has a field 'SpecifiedAxes', then call the axis command with those axes
        if (isfield(InputStruct,'SpecifiedAxes'))
            axis(InputStruct.SpecifiedAxes,AxisValues);
            set(InputStruct.SpecifiedAxes,'YTickMode','auto')
        else
            axis(AxisValues);
        end
        
    end
end
