% File: GetDefaultAxesColorOrder.m
%
% Description: uses MATLAB's DefaultAxesColorOrder - to get color for plot lines
% 
% Parameter(s): 
%           Optionalfig - figure to set
%               if no parameters, then use fig number 0: for global info
% 
% Return value(s):
%             OrderString - string representation (see help plot) i.e.  bgrcmyk
%             OrderArray - array representation, using rgb, like red = [1 0 0]
% 
% Usage: 
% 
% OrderString = GetDefaultAxesColorOrder;

function [OrderString,OrderArray] = GetDefaultAxesColorOrder(Optionalfig)
    
    if (nargin == 0)
        OrderArray = get(0,'DefaultAxesColorOrder');
    else
        OrderArray = get(Optionalfig,'DefaultAxesColorOrder');
    end

    sz = size(OrderArray);
    len = sz(1);
    
    OrderString = '';
    for i = 1:len
        switch OrderArray(i,1)
            case 0
                switch OrderArray(i,2)
                    case 0
                        OrderString = [OrderString 'b'];
                    case 0.5
                        OrderString = [OrderString 'g'];
                    case 0.75
                        OrderString = [OrderString 'c'];
                end
            case 0.25
                OrderString = [OrderString 'k'];
            case 0.75
                switch OrderArray(i,2)
                    case 0
                        OrderString = [OrderString 'm'];
                    case 0.75
                        OrderString = [OrderString 'y'];
                end
            case 1
                switch OrderArray(i,2)
                    case 0
                        OrderString = [OrderString 'r'];
                    case 1
                        OrderString = [OrderString 'w'];
                end
        end
    end

end
