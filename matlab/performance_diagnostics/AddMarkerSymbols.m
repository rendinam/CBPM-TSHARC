% File: AddMarkerSymbols.m
%
% Description: add symbols to plot markers
% 
% Parameter(s):
%     AX - axes to add symbols to
%     InputStruct - optional struct to contain options and flags
% 
% Return value: void
% 
% Usage: 
% X = 1:10;
% Y = 11:20;
% AX = plot(X,Y);
% AddMarkerSymbols(AX)

function [] = AddMarkerSymbols(AX,InputStruct)

%     If no InputStruct parameter, create a blank one
    if (nargin <2)
        InputStruct = [];
    end
    
    if isfield(InputStruct,'LineStyle')
        LineStyle = InputStruct.LineStyle;
    else
        LineStyle = 'none';
    end     

    Markers = {'.','x','+','^','h','s'};
    for i = 1:length(AX)
        ind = mod(i-1,6)+1; % cycle through the six selected as clearly visible
        set(AX(i),'Marker', Markers{ind});
        set(AX(i),'LineStyle', LineStyle);
    end   
    
end
