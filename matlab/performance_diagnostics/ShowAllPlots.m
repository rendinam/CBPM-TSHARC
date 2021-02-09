% File: ShowAllPlots.m
%
% Description: display every matlab figure window currently open
% 
% Parameter(s): void
% 
% Return value: void
% 
% Usage: 
% ShowAllPlots

function [] = ShowAllPlots
    global lastfignum
    if (length(lastfignum) == 0)
        fprintf('No plots to show')
    end
    
    for i = 1:lastfignum
        figure(i)
    end
end
