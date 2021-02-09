function [] = UpdateProcFileList(directory, FileList)

% Internal use function
% Function used to add to the file: ProcFiles.mat,
% which stores which BPMs have data stored in which Process Data files

% RawFilesValid key
%     -1: too big
%     0: not checked yet
%     1: valid
%     2: bad text format
%     4: file missing
%     5: file empty

% UpdateProcFileList('U:\tsharc_offline\proc\', 142076)
% UpdateProcFileList('U:\tsharc_online\proc\', 142076)

% load('ProcFiles.mat','ProcFiles','ProcFilesChecked','ProcFilesValid')
% UpdateProcFileList('/nfs/cesr/online/instr/data/cbpmII/proc', find(ProcFilesValid == -1))

    ResetPause
    ConditionalPause;
    
    try
    
    if (~exist('CheckAnyway'))
    CheckAnyway = 0;
    end

    load('ProcFiles.mat','ProcFiles','ProcFilesChecked','ProcFilesValid')

%     L{122} = {};
%     ProcFiles = L';
%     MapBPMIndexAndName
%     for i = 1:length(ProcFiles)
%         try
%             ProcFiles{BPMIndexAndName{i,1},1} = BPMIndexAndName{i,2};
%             ProcFiles{BPMIndexAndName{i,1},2} = [];
%         catch
%         end
%     end
%     ProcFilesChecked = [];
    BPMtouched = zeros(1,122)';
    
    ca
    PlotUpdatedFiles(ProcFiles)
    GetPlot;plot(ProcFilesValid,'.')  
    pause(0.1)  

    for fileid = FileList
        try
            
            if (ProcFilesValid(fileid) == 1)
                fprintf('%i: file already updated.\n',fileid)
                continue
            end
            
            save('ProcFiles.mat','ProcFiles','ProcFilesChecked','ProcFilesValid')            
%         fileid
            [Filename] = CreateFileName(directory,fileid);
            
            s = dir(Filename);
            if isempty(s)
                if (ProcFilesValid(fileid) == 3)
%                     ProcFilesValid(fileid) = 3;
                    fprintf('%i: Dir: Missing file, V stays 3.\n',fileid)
                else
                    fprintf('%i: Dir: Missing file, V stays %i.\n',fileid,ProcFilesValid(fileid))
                end                
                 
%                  ProcFilesValid(fileid) = 3;
                 continue
            end
            
            if s.bytes == 0
                ProcFilesValid(fileid) = 4;
                fprintf('%i: ReadIn: Empty file, V stays 4.\n',fileid)
                continue
            end
        
            [~,~,BPMIndices] = ReadInProcessFile(Filename,struct('DontWarn',1));
            if isempty(BPMIndices)
%                 if ( ProcFilesValid(fileid) == 3 ||ProcFilesValid(fileid) == 4)
                    ProcFilesValid(fileid) = 4;
                    fprintf('%i: ReadIn: Empty file.\n',fileid)
%                 else
%                     fprintf('%i: ReadIn missing file, V stays %i.\n',fileid,ProcFilesValid(fileid))
%                 end                
                 
                 continue
            end
            for i = 1:length(BPMIndices)
%                 if isempty(find(ProcFiles{BPMIndices(i),2} == fileid, 1))
                    ProcFiles{BPMIndices(i),2} = [ProcFiles{BPMIndices(i),2}; fileid];
                    BPMtouched(BPMIndices(i)) = 1;
%                 end
            end
            
            ProcFilesValid(fileid) = 1;
            fprintf('%i: %i instrument(s).\n',fileid,length(BPMIndices))
            z = 1;
        catch me
            if strcmp(me.message,'unrecognized text structure')
                fprintf('%i: unrecognized text structure in file.\n',fileid)
                ProcFilesValid(fileid) = 2;
            else
                fprintf('%i: unknown error.\n',fileid)
                z = 1;
            end
            continue
        end
    end
    
    ProcFilesChecked = union(ProcFilesChecked,FileList)';
    
    for i = 1:length(ProcFiles)
        if BPMtouched(i)
%             ProcFiles{i,2} = sort(ProcFiles{i,2});
            ProcFiles{i,2} = union(ProcFiles{i,2},[]);
            ProcFilesValid(ProcFiles{i,2}) = 1;
        end
    end

    save('ProcFiles.mat','ProcFiles','ProcFilesChecked','ProcFilesValid')
    
    ca
    PlotProcFiles(ProcFiles)
    GetPlot;plot(ProcFilesValid,'.')       
    pause(0.1)  
    
    catch me
        rethrow(me)
    end

end

function [Filename] = CreateFileName(directory,fileid)

    directoryNumber = floor(fileid/1e3);
    
    if directoryNumber<100
        dir = sprintf('%.2i',directoryNumber);
    else
        dir = sprintf('%.3i',directoryNumber);
    end
    
    if fileid <= 2032
        strFileId = sprintf('%.5i',fileid);
    else
        strFileId = sprintf('%.6i',fileid);
    end

    
%     dir = sprintf('%.2i',str2num(strFileId(1:3)));
    
    Filename = [directory '/' dir '/PROC-' strFileId '.dat'];      
end

function [] = PlotProcFiles(ProcFiles)

try
    GetPlot;

    hold on
%     load('Data\ProcFiles.mat','ProcFiles','ProcFilesChecked','ProcFilesValid')
    for i = 1:length(ProcFiles)
        Array = ProcFiles{i,2};
%         ProcFilesValid(Array) = 1;
        if ~isempty(Array)
%             plot(i*ones(length(Array)),Array,'.')

%             if (length(Array) > 30000)
%                 z = 1;
%             end
            plot(Array,i* ([1:length(Array)] ./ [1:length(Array)]) ,'.')
        end
    end
%     axis([0 14000 0 130])
% save('Data\ProcFiles.mat','ProcFiles','ProcFilesChecked','ProcFilesValid')
    
catch me
    rethrow(me)
end

end