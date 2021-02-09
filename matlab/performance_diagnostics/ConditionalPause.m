function [] = ConditionalPause
    fid = fopen('DoPause.txt','r');
    line = fgetl(fid);
    fclose(fid);
    if (~isempty(strfind(line,'1')))
        i = 1;
        pause;
        ResetPause
    end
end