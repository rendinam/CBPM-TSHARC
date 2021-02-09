% File: SetDefaultAxesColorOrder.m
%
% Description: uses MATLAB's DefaultAxesColorOrder - to set color for plot lines
% 
% Parameter(s): 
%           fig - figure to set, can be 0 for global
%           OrderString - example: 'rgbk'
% 
% Return value(s):
%             OrderArray - array representation, using rgb, like red = [1 0 0]
%             OrderString - string representation (see help plot) i.e.  bgrcmyk
% 
% Usage: 
% 
% OrderArray = GetDefaultAxesColorOrder(0,'bgrcmyk');

function [OrderArray,OrderString] = SetDefaultAxesColorOrder(fig,OrderString)
    len = length(OrderString);
    
    OrderArray = zeros(len,3);
    for i = 1:len
        switch lower(OrderString(i))    %r g b
            case 'b'
                OrderArray(i,:) = [0 0 1];
            case 'g'
                OrderArray(i,:) = [0 .5 0];
            case 'r'
                OrderArray(i,:) = [1 0 0];
            case 'c'
                OrderArray(i,:) = [0 .75 .75];
            case 'm'
                OrderArray(i,:) = [.75 0 .75];
            case 'y'
                OrderArray(i,:) = [.75 .75 0];
            case 'k'
                OrderArray(i,:) = [0.25 0.25 0.25];
            case 'w'
                OrderArray(i,:) = [1 1 1];
        end
    end
    
    set(fig,'DefaultAxesColorOrder',OrderArray);
end
