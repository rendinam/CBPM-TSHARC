% File: SaveFigure.m
%
% Description: Save a figure to a fild
% 
% Parameter(s): 
%     Filename - file name to save to, will be saved in a .\Saved\ directory
% 
% Return value: void
% 
% Usage: 
% GetPlot;plot(1:3);SaveFigure('test')

function [] = SaveFigure(Filename)

    try
    saveas(gcf, ['.\\Saved\\' Filename], 'fig')  
    catch ME
        try
            saveas(gcf, ['\Saved\' Filename], 'fig')  
        catch ME
        mkdir('Saved')
        saveas(gcf, Filename, 'fig') 
        end
    end
    
end
