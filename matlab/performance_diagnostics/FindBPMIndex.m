function [index] = FindBPMIndex(name,InputStruct)

%     If no InputStruct parameter, create a blank one
    if (nargin <2)
        InputStruct = [];
    end
    
    MapBPMIndexAndName

    index = -1;
    foundit = 0;
    for i = 1:length(BPMIndexAndName); 
        foundit = strcmp(name,BPMIndexAndName{i,2});
        if foundit
            index = BPMIndexAndName{i,1};
            break
        end
    end
    if (~foundit)
        if isfield(InputStruct,'IfErrorThrowIt')
            try;error(sprintf(1,'Didn''t find a BPM by the name %s.\n',name));catch;end
        else
            fprintf(1,'Didn''t find a BPM by the name %s.\n',name);
        end
    end

end