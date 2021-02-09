function [] = ResetPause
    fid = fopen('DoPause.txt','w');
    fprintf(fid,'0\n');
    fclose(fid);
end