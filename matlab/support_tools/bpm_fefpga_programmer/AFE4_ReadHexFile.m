function Fbuf = AFE4_ReadHexFile(Fname)
    
    fid = fopen(Fname);                 %open the file
    Fbuf = fread(fid,inf,'uint16');     %grab the image
    fclose(fid);                        %close the file
    return;
end