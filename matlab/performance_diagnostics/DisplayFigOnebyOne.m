% File: DisplayFigOnebyOne.m
%
% Description: Display each figure, then pause and wait for a key to be
% press to display the next one
% 
% Parameter(s): Optional
%     Start - the index to start showing from until the end
% 
% Return value: void
% 
% Usage: 
% DisplayFigOnebyOne


function [] = DisplayFigOnebyOne(start)
    global lastfignum
    if (isempty(lastfignum))
        fprintf('No plots to show')
    end
    
    if (nargin == 0)
        start = 1;
    end
    
    global SpecialFigureStruct    
    
    for i = start:lastfignum
        if (length(SpecialFigureStruct) >= i &&...
                isfield(SpecialFigureStruct(i),'IsClosed') && ...
                ~isempty(SpecialFigureStruct(i).IsClosed) && ...
                SpecialFigureStruct(i).IsClosed == 1)
            continue
        end  
    
        figure(i)
        fprintf('%i ',i)
        pause
    end
    fprintf('\n')
end
