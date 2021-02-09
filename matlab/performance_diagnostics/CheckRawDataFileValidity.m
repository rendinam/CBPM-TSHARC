function [valid] = CheckRawDataFileValidity(FullFileName)

    valid = 2;
    fid = fopen(FullFileName);
    line = fgetl(fid);if (line == -1);valid = 2;return;end;%fprintf('%s\n',line)
    while (1)
        if strfind(line,'--BEGIN DATA--')
            line = fgetl(fid);if (line == -1);break;end;%fprintf('%s\n',line)
            if ~strcmp(line(1:1),'0')
                valid = 2;
                return
            else
                valid = 1;
            end
        end
        line = fgetl(fid);if (line == -1);break;end;%fprintf('%s\n',line)
        
    end
    
    fclose(fid);

end