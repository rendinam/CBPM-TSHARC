function [] = PlotUpdatedFiles(Files)

%     GetPlot;
% 
%     hold on
%     for i = 1:length(Files)
%         Array = Files{i,2};
%         if ~isempty(Array)
% %             plot(i*ones(length(Array)),Array,'.')
%             plot(Array,i*ones(length(Array)),'.')
%         end
%     end
% %     axis([0 14000 0 130])
% end

% try
%     GetPlot; hold on
%     num_instr = [];
%     for i = 1:length(Files)
%         Array = Files{i,2};
%         num_instr(i) = length(Array);
%         if ~isempty(Array)
% %             plot(Array,i* ([1:length(Array)] ./ [1:length(Array)]) ,'.')
%             plot(Array,i*ones(length(Array)),'.')
%         end
%     end
% catch me
    try
        GetPlot; hold on
        num_instr = [];
        for i = 1:length(Files)
            Array = Files{i,2};
            num_instr(i) = length(Array);
            if ~isempty(Array)
                plot(Array,i* ([1:length(Array)] ./ [1:length(Array)]) ,'.')
    %             plot(Array,i*ones(length(Array)),'.')
            end
        end
    catch me
        rethrow(me)
    end
% end



end