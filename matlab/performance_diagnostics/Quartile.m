% File: Quartile.m
%
% Create a dataset from input array, and report quartile values
% 
% Parameter(s): 
%     Arr - input array
%     Optional:
%       i - which quartile to return: 1:1st, 2:2nd, 3:median, 4:3rd, 5:4th
% 
% Return value: 
%     quantiles
% 
% Usage: 
% First on a linux terminal, use concat_proc_files.py, to create a concatenated process data file, for this M file to read in.
% Type ./concat_proc_files.py for usage instructions
% 
% Q = Quartile(rand(100,1))

function [val] = Quartile(Arr,i)

    d = dataset(Arr);
    s = summary(d);

    if (nargin == 1)
        val = s.Variables.Data.Quantiles;
    else
        val = s.Variables.Data.Quantiles(i+1);
    end

end
