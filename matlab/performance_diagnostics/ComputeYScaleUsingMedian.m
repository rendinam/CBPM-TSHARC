% File: ComputeYScaleUsingMedian.m
%
% Description: % Compute a Min and Max for data, using the median and quartiles of the data
% 
% Parameter(s):
%     Y - data array
%     Optional:
%         FactorIn - if you want to scale by a number of than 4
% 
% Return value: 
%     minscl,maxscl - computed min and max for plot scale
% 
% Usage: 
% [minscl,maxscl] = ComputeYScaleUsingMedian(30.*rand(1,6), 2)


function [minscl,maxscl] = ComputeYScaleUsingMedian(Y, FactorIn)


        Q = Quartile(Y);
        
        q2 = min(Q(2,:));
        medianval = mean(Q(3,:));
        q3 = max(Q(4,:));
        Meanq2q3 = mean([q2 q3]);
        
        DefaultFactor = 4;
        Factor = DefaultFactor;
        if ((nargin >1) && (~isempty(FactorIn)))
            Factor = FactorIn;
            if (FactorIn == -1)
                Factor = DefaultFactor;
            end
        end
        
        minscl = medianval - (Meanq2q3-q2)*Factor;
        maxscl = medianval + (q3-Meanq2q3)*Factor;
end
