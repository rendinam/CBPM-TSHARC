function [names] = NumInstrumentsInFile(fileid)

    SetupCbpmfileioAPI

    fprintf('RD-%.6i.dat \n',fileid)

    %             open file
    result = cbpm_read_rawfile_m( fileid );

%             count instruments
    num_insts = cbpm_file_num_insts_m()
    
  names{1} = '';
  namecount = 1;
  name = cbpm_file_inst_locs_m(  );
  while ( strcmp(name, '') == 0 ) 
    names{namecount} = name ;
    namecount = namecount + 1;
    name = cbpm_file_inst_locs_m(  );
  end 

end
