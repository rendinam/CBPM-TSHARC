function [] = UpdateRawFileList(directory,FileList,CheckAnyway)

% Internal use function
% Function used to add to the file: RawFiles.mat,
% which stores which BPMs have data stored in which RawData files

if nargin < 3
    CheckAnyway = 0;
end

% RawFilesValid key
%     -1: too big
%     0: not checked yet
%     1: valid
%     2: bad text format
%     3: file missing

WorkSpaceArguments = {'RawFiles.mat','RawFiles','RawFilesChecked','RawFilesValid','RawFilesNumBunches','RawFilesNumTurns'};
% load(WorkSpaceArguments{:})
% lookat = find(RawFilesValid == -1);
% UpdateRawFileList('/nfs/cesr/offline/instr/data/cbpmII/raw', fliplr(lookat')')

    ca
    start = now;
    t2 = start;
    SetupCbpmfileioAPI
    ResetPause
    
    try
        
%         if(1)
            load(WorkSpaceArguments{:})
            [RawFiles,RawFilesChecked] = CosolidateData(RawFiles,RawFilesChecked);
%             
%             minR = RawFilesChecked(1);
%             maxR = RawFilesChecked(length(RawFilesChecked));
%              
%             for i = 1:length(RawFiles)
%                 Array = RawFiles{i,2};
%                 if isempty(Array);continue;end
%                 minR = min(minR,Array(1));
%                 maxR = max(maxR,Array(length(Array)));
%             end

             PlotUpdatedFiles(RawFiles)
             GetPlot;plot(RawFilesValid,'.')     
             pause(0.1)
%         else
% 
%             L{122} = {};
%             RawFiles = L';
%             MapBPMIndexAndName
%             for i = 1:length(RawFiles)
%                 try
%                     RawFiles{BPMIndexAndName{i,1},1} = BPMIndexAndName{i,2};
%                     RawFiles{BPMIndexAndName{i,1},2} = [];
%                 catch
%                 end
%             end
%             RawFilesChecked = [];
%         end
        
%      PlotRawFiles(RawFiles)
    
    BPMtouched = zeros(1,122)';

    for fileid = FileList'
        
        save(WorkSpaceArguments{:})        
        
        fprintf('File id:%i, ',fileid)
        
        % RawFilesValid key
%     -1: too big
%     0: not checked yet
%     1: valid
%     2: bad text format
%     3: file missing

        if ~CheckAnyway
            if fileid < length(RawFilesValid)
                dontcheckFile = (RawFilesValid(fileid) == 1) || (RawFilesValid(fileid) == 2);
                if dontcheckFile
                      fprintf('File already updated.\n')
                    continue
                end
            end
        end
        
        ConditionalPause;
        [Filename] = CreateFileName(directory,fileid);
        s = dir(Filename);
        
        if isempty(s)
            if (RawFilesValid(fileid) == 3)
                RawFilesValid(fileid) = 3;
                fprintf('File missing, V stays 3.\n')
            else
                fprintf('File missing, V stays %i\n', RawFilesValid(fileid))
            end
             continue
        end
        threshold = 4000e6; %8*1e8 / 16; %4;
        t1 = t2;
        t2 = now;
        fprintf('Size: %7.3f (Mb), SizeT:%7.3f (Mb), %s, split:%s, total:%s, ',s.bytes/1e6,threshold/1e6, datestr(now,'HH:MM:SS'), datestr(t2-t1,'HH:MM:SS'), datestr(t2-start,'HH:MM:SS'))
%         [vmrss, vmsize] = getmemusage; fprintf('%i,%i\n',vmrss, vmsize)
        
        DeleteThisScript
        
        names{1} = '';        

        if (fileid < 4080 )
            
            if s.bytes >=3000e6
                RawFilesValid(fileid) = -1;
                fprintf('File too large: 6 MB.\n')
                continue
            end   
            
            [valid] = CheckRawDataFileValidity(Filename);
            RawFilesValid(fileid) = valid;
            if valid ~= 1
                fprintf('bad text format. \n')
                continue
            end            
             
             try
                 
                 num_instruments = 0;
                 
%                     open file
                fid = fopen(Filename,'r');
%                 fprintf('%s ',FILENAME)

%                     parse file
                line = fgetl(fid);
%                 fprintf( '%s\n' ,line);
                i = 1;
                while (line~=-1)

                    if (strfind(line,'Location') ~= -1)
%                         fprintf('%s',line(23:length(line)))
                        names{i} = line(23:length(line));
                        num_instruments = i;
                    end

                    if (strfind(line,'# --- Bunch 1 ---') ~= -1)
                        [C{i}] = textscan(fid, '%f%f%f%f%f%f');
                        i = i + 1;
                    end

                    line = fgetl(fid);
                end
                
                fclose(fid); 
             catch me
                 try; fclose(fid); catch; end
                 rethrow(me)
             end
             
             num_instruments = i-1;
                            
        else
             
            if s.bytes >=threshold
                RawFilesValid(fileid) = -1;
                fprintf('File too large.\n')
                continue
            end

            [valid] = CheckRawDataFileValidity(Filename);
            
%             if (fileid == 8765)
%                 valid = 2;
%             end
            
            RawFilesValid(fileid) = valid;
            if valid ~= 1
                fprintf('bad text format. \n')
                continue
            end            
        
            %             open file
            result = cbpm_read_rawfile_m( fileid );      
            
%              [vmrss, vmsize] = getmemusage; fprintf('%i, %i,%i\n',fileid,
%              vmrss, vmsize);
    %         namecount = 1;

            num_instruments = cbpm_file_num_insts_m();

             RawFilesNumBunches(fileid) =  cbpm_file_num_bunches_m();
             RawFilesNumTurns(fileid) = cbpm_file_num_turns_m();
             
            for i = 1:num_instruments
                name = cbpm_file_inst_locs_m(  );
    %         while ( strcmp(name, '') == 0 ) 
                names{i} = name ;
                
            end
         end
        
%             namecount = namecount + 1;
%             name = cbpm_file_inst_locs_m(  );
            
        try

            fprintf('#Inst: %i, ',num_instruments)
            for i = 1:num_instruments
            
                [index] = FindBPMIndex(names{i},struct('IfErrorThrowIt',1));
                if (index == -1)
                    break;
                end
                RawFiles{index,2} = [RawFiles{index,2}; fileid];
                BPMtouched(index) = 1;            
            end
        
        catch me
            fprint('Error: %s\n', me.message)
            continue
        end            
        
        fprintf('Success.\n')

    end
    
    RawFilesChecked = union(RawFilesChecked,FileList)';
    
%     BPMtouched
    for i = 1:length(RawFiles)
%         if BPMtouched(i)
%             RawFiles{i,2} = sort(RawFiles{i,2});
            RawFiles{i,2} = union(RawFiles{i,2},[]);
            RawFilesChecked =  union(RawFilesChecked, RawFiles{i,2});
%         end
    end

    save(WorkSpaceArguments{:})
    
    catch me
        rethrow(me)
    end
    
    fprintf('Done.\n')
    ca
 PlotUpdatedFiles(RawFiles)
 GetPlot;plot(RawFilesValid,'.')     
 pause(0.1)    

end

function [Filename] = CreateFileName(directory,fileid)

    directoryNumber = floor(fileid/1e3);
    
    if directoryNumber<100
        dir = sprintf('%.2i',directoryNumber);
    else
        dir = sprintf('%.3i',directoryNumber);
    end
    
    if fileid <= 134
        strFileId = sprintf('%.5i',fileid);
    else
        strFileId = sprintf('%.6i',fileid);
    end

%     strFileId = sprintf('%.6i',fileid);
%     dir = sprintf('%.2i',str2num(strFileId(1:3)));
    
    Filename = [directory '/' dir '/RD-' strFileId '.dat'];      
end

function [RawFiles,RawFilesChecked] = CosolidateData(RawFiles,RawFilesChecked)
    for i = 1:length(RawFiles)
%         if BPMtouched(i)
%             RawFiles{i,2} = sort(RawFiles{i,2});
            RawFiles{i,2} = union(RawFiles{i,2},[]);
            RawFilesChecked =  union(RawFilesChecked, RawFiles{i,2});
%         end
    end        
end

% function [] = PlotRawFiles(RawFiles)
% 
%     GetPlot;
% 
%     hold on
%     for i = 1:length(RawFiles)
%         Array = RawFiles{i,2};
%         if ~isempty(Array)
% %             plot(i*ones(length(Array)),Array,'.')
%             plot(Array,i*ones(length(Array)),'.')
%         end
%     end
%     axis([0 14000 0 130])
% end

% function [] = LoadWorkspace()
% end
% 
% function [] = SaveWorkspace()
% end