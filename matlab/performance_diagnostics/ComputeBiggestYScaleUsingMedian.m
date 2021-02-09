% File: ComputeBiggestYScaleUsingMedian.m
%
% Description: % % Compute a Min and Max for data list, using the median and quartiles of the of each array in the data list
% 
% Parameter(s):
%     List - List of data arrays
%     Optional:
%         FactorIn - if you want to scale by a number of than 4
% 
% Return value: 
%     minscl,maxscl - computed min and max for plot scale
% 
% Usage: 
% [minscl,maxscl] = ComputeBiggestYScaleUsingMedian({-10.*rand(1,6), 20.*rand(1,6), 30.*rand(1,6)}, 2)

function [minscl,maxscl] = ComputeBiggestYScaleUsingMedian(List, FactorIn)


    if nargin <2
        FactorIn = [];
    end

    len = length(List);
    try
        minsclA = zeros(1,len);
        maxsclA = zeros(1,len);
        for i = 1:len
            [minsclA(i),maxsclA(i)] = ComputeYScaleUsingMedian(List{i},FactorIn);
        end
        minscl = min(minsclA);
        maxscl = max(maxsclA);    
    catch ME
        disp(ME)
        [minscl,maxscl] = ComputeYScaleUsingMedian(List,FactorIn);
    end

end
