function [num_insts,names] = ReadInRawDataFile(fileid,bpmid)

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
  
  fprintf('Testing cbpm_file_timestamp_:\n');
  timestamp2 = cbpm_file_timestamp_m(  );
  fprintf('  timestamp = %s\n', timestamp2 );


  num_instruments = cbpm_file_num_insts_m();
  fprintf('Testing cbpm_file_num_instruments_:\n');
  fprintf('  num_instruments = %d\n', num_instruments );

  
  fprintf('Testing cbpm_file_num_bunches_:\n');
  fprintf('  num_bunches     = %d\n', cbpm_file_num_bunches_m() );


  fprintf('Testing cbpm_file_num_turns_:\n');
  fprintf('  num_turns       = %d\n', cbpm_file_num_turns_m() );


  fprintf('Testing cbpm_file_condx_number_:\n');
  fprintf('  CONDX number    = %d\n', cbpm_file_condx_m() );  

  location = names{bpmid};
  fprintf('\n\nLocation = %s\n', location);
  
  idata = cbpm_file_raw_data_m( location  );
  %fprintf('raw :\n'); %d %d %d %d \n', idata);
  
  fdata = cbpm_file_tbt_data_m( location);
  %fprintf('TBT:\n'); %f %f %f %f \n', fdata );
  
  pos = cbpm_file_pos_data_m( location);
  %fprintf('X pos:\n');  %f, Y pos:  %f\n', pos );
  
  phase = cbpm_file_shaker_phase_m( location);
  %fprintf('Phase word:\n'); %x %x\n', phase );
  
  fname = sprintf('workspace-%i-bpm%i.mat',fileid,bpmid);
  save(fname)
  fprintf('Saved to: %s\n',fname)
end