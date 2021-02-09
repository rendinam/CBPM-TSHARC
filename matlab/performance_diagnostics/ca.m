% File: ca.m
%
% Description: A script to close all open figure windows
% 
% Parameter(s): void
% 
% Return value: void
% 
% Usage: 
% ca

%see GetPlot for explanation of these globals

global lastfignum
lastfignum = 0;

global SpecialFigureStruct
SpecialFigureStruct = [];

global position_index
position_index = [];

close all