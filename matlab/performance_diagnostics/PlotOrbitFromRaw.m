function [] = PlotOrbitFromRaw(fileid, InputStruct)

%     If no InputStruct parameter, create a blank one
    if (nargin <2)
        InputStruct = [];
    end
    
    doPlot = 1;
    if isfield(InputStruct,'doPlot') 
        doPlot = InputStruct.doPlot;
    end     

    SetupCbpmfileioAPI

    fprintf('RD-%.6i.dat \n',fileid)
    
    BPMInd = 1;
    BPMnames = {};
    X = [];
    for i = 2:-1:1
        fprintf('Reading %i...\n',fileid+i-1)
        result = cbpm_read_rawfile_m( fileid+i-1 );
        
        names{1} = ''; namecount = 1; name = cbpm_file_inst_locs_m(  );
            while ( strcmp(name, '') == 0 ) 
            names{namecount} = name ;
            namecount = namecount + 1;
            name = cbpm_file_inst_locs_m(  );
            end        

        num_instruments = cbpm_file_num_insts_m();
        for j= 1:num_instruments
            pos(BPMInd,:) = mean(cbpm_file_pos_data_m(  names{j})); 
            err(BPMInd,:) = std(cbpm_file_pos_data_m(  names{j}));
            BPMnames{BPMInd} = names{j};
            X(BPMInd) = FindBPMIndex(names{j});
            BPMInd = BPMInd+1;
        end          

    end

    %GetPlot;plot(pos,'.-');
    %X = 1:BPMInd-1;
    if (doPlot)
        GetPlot;errorbar([X; X]',pos,err,'.-');
        Title = sprintf('%i-%i, Position',fileid,fileid+1);
        title(Title)
        legend('X','Y')

        GetPlot;plot(X, err,'.-');
        Title = sprintf('%i-%i, RMS',fileid,fileid+1);
        title(Title)
        legend('X','Y')    
    end
    
    pos = double(pos);
    err = double(err);
    
    fname = sprintf('workspace-%i-%i-orbit.mat',fileid,fileid+1);
    save(fname,'X','pos','err','BPMnames')
    fprintf('Saved to: %s\n',fname)
end