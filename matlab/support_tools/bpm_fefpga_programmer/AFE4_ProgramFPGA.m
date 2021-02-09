function AFE4_ProgramFPGA(handles,Fname,Socket)
    %some bpm constants
    ControlLoc0 = 150994944;
    ControlLoc1 = 152043520;
    ControlLoc2 = 153092096;
    ControlLoc3 = 154140672;

    FileBuffer = uint16(AFE4_ReadHexFile(Fname));
    Flength = length(FileBuffer);
    pause(0.5)
    
    %**********************************************************************
    %  program card0
    %**********************************************************************
    %write the data via cbi net
    set(handles.LogBox,'String','Writing SRAM for Card 0....');
    pause(0.5)
    i = 1;
    do_once0 = 1;
    do_once1 = 1;
    while (i < Flength)
        %check to see if the default increment is too large
        if (Flength - i) >= 1023
            Increment = 1023;
        else
            Increment = Flength - i;
        end
        %grab a chunk of the file and buffer it
        DataBuf = FileBuffer(i:i+Increment);
        %check to see if we need to move to the next sram
        if ((i+Increment) <= 524288)
           %set the sram0 enable
           if (do_once0 == 1)
               matlab_cbi_net_wr_mem(Socket, ControlLoc0+59, uint32(0));
               matlab_cbi_net_wr_mem(Socket, ControlLoc0+59, uint32(1));
               do_once0 = 2;
           end
        else
           %set the sram1 enable
           if (do_once1 == 1)
               matlab_cbi_net_wr_mem(Socket, ControlLoc0+59, uint32(0));
               matlab_cbi_net_wr_mem(Socket, ControlLoc0+59, uint32(2));
               do_once1 = 2;
           end
        end
        %send it down to the front end
        matlab_cbi_net_wr_mem(Socket, ControlLoc0+4096, uint32(DataBuf));
        i = i + Increment + 1;
    end

    %clear the control bit
    matlab_cbi_net_wr_mem(Socket, ControlLoc0+59, uint32(0));
    set(handles.LogBox,'String','Done');

    set(handles.LogBox,'String','Starting ACE Player....');
    matlab_cbi_net_wr_mem(Socket, ControlLoc0+60, uint32(1));
    pause(1);
    prom_prog_status = (matlab_cbi_net_rd_mem(Socket, (ControlLoc0 + 60), 1, 12));
    while (prom_prog_status ~= 14352)
        set(handles.LogBox,'String',['Card0 ACE Player Busy: ',num2str(prom_prog_status)]);
        pause(1);
        prom_prog_status = (matlab_cbi_net_rd_mem(Socket, (ControlLoc0 + 60), 1, 12));
        if(prom_prog_status == 11789)
            set(handles.LogBox,'String',['Card0 ACE Player Error: ',num2str(prom_prog_status)]);
            return;
        end
    end

    %**********************************************************************
    %  program card1
    %**********************************************************************
    %write the data via cbi net
    set(handles.LogBox,'String','Writing SRAM for Card 1....');
    pause(0.5)
    i = 1;
    do_once0 = 1;
    do_once1 = 1;
    while (i < Flength)
        %check to see if the default increment is too large
        if (Flength - i) >= 1023
            Increment = 1023;
        else
            Increment = Flength - i;
        end
        %grab a chunk of the file and buffer it
        DataBuf = FileBuffer(i:i+Increment);
        %check to see if we need to move to the next sram
        if ((i+Increment) <= 524288)
           %set the sram0 enable
           if (do_once0 == 1)
               matlab_cbi_net_wr_mem(Socket, ControlLoc1+59, uint32(0));
               matlab_cbi_net_wr_mem(Socket, ControlLoc1+59, uint32(1));
               do_once0 = 2;
           end
        else
           %set the sram1 enable
           if (do_once1 == 1)
               matlab_cbi_net_wr_mem(Socket, ControlLoc1+59, uint32(0));
               matlab_cbi_net_wr_mem(Socket, ControlLoc1+59, uint32(2));
               do_once1 = 2;
           end
        end
        %send it down to the front end
        matlab_cbi_net_wr_mem(Socket, ControlLoc1+4096, uint32(DataBuf));
        i = i + Increment + 1;
    end

    %clear the control bit
    matlab_cbi_net_wr_mem(Socket, ControlLoc1+59, uint32(0));
    set(handles.LogBox,'String','Done');

    set(handles.LogBox,'String','Starting ACE Player....');
    matlab_cbi_net_wr_mem(Socket, ControlLoc1+60, uint32(1));
    pause(1);
    prom_prog_status = (matlab_cbi_net_rd_mem(Socket, (ControlLoc1 + 60), 1, 12));
    while (prom_prog_status ~= 14352)
        set(handles.LogBox,'String',['Card1 ACE Player Busy: ',num2str(prom_prog_status)]);
        pause(1);
        prom_prog_status = (matlab_cbi_net_rd_mem(Socket, (ControlLoc1 + 60), 1, 12));
        if(prom_prog_status == 11789)
            set(handles.LogBox,'String',['Card1 ACE Player Error: ',num2str(prom_prog_status)]);
            return;
        end
    end

    %**********************************************************************
    %  program card2
    %**********************************************************************
    %write the data via cbi net
    set(handles.LogBox,'String','Writing SRAM for Card 2....');
    pause(0.5)
    i = 1;
    do_once0 = 1;
    do_once1 = 1;
    while (i < Flength)
        %check to see if the default increment is too large
        if (Flength - i) >= 1023
            Increment = 1023;
        else
            Increment = Flength - i;
        end
        %grab a chunk of the file and buffer it
        DataBuf = FileBuffer(i:i+Increment);
        %check to see if we need to move to the next sram
        if ((i+Increment) <= 524288)
           %set the sram0 enable
           if (do_once0 == 1)
               matlab_cbi_net_wr_mem(Socket, ControlLoc2+59, uint32(0));
               matlab_cbi_net_wr_mem(Socket, ControlLoc2+59, uint32(1));
               do_once0 = 2;
           end
        else
           %set the sram1 enable
           if (do_once1 == 1)
               matlab_cbi_net_wr_mem(Socket, ControlLoc2+59, uint32(0));
               matlab_cbi_net_wr_mem(Socket, ControlLoc2+59, uint32(2));
               do_once1 = 2;
           end
        end
        %send it down to the front end
        matlab_cbi_net_wr_mem(Socket, ControlLoc2+4096, uint32(DataBuf));
        i = i + Increment + 1;
    end

    %clear the control bit
    matlab_cbi_net_wr_mem(Socket, ControlLoc2+59, uint32(0));
    set(handles.LogBox,'String','Done');

    set(handles.LogBox,'String','Starting ACE Player....');
    matlab_cbi_net_wr_mem(Socket, ControlLoc2+60, uint32(1));
    pause(1);
    prom_prog_status = (matlab_cbi_net_rd_mem(Socket, (ControlLoc2 + 60), 1, 12));
    while (prom_prog_status ~= 14352)
        set(handles.LogBox,'String',['Card2 ACE Player Busy: ',num2str(prom_prog_status)]);
        pause(1);
        prom_prog_status = (matlab_cbi_net_rd_mem(Socket, (ControlLoc2 + 60), 1, 12));
        if(prom_prog_status == 11789)
            set(handles.LogBox,'String',['Card2 ACE Player Error: ',num2str(prom_prog_status)]);
            return;
        end
    end

    %**********************************************************************
    %  program card3
    %**********************************************************************
    %write the data via cbi net
    set(handles.LogBox,'String','Writing SRAM for Card 3....');
    pause(0.5)
    i = 1;
    do_once0 = 1;
    do_once1 = 1;
    while (i < Flength)
        %check to see if the default increment is too large
        if (Flength - i) >= 1023
            Increment = 1023;
        else
            Increment = Flength - i;
        end
        %grab a chunk of the file and buffer it
        DataBuf = FileBuffer(i:i+Increment);
        %check to see if we need to move to the next sram
        if ((i+Increment) <= 524288)
           %set the sram0 enable
           if (do_once0 == 1)
               matlab_cbi_net_wr_mem(Socket, ControlLoc3+59, uint32(0));
               matlab_cbi_net_wr_mem(Socket, ControlLoc3+59, uint32(1));
               do_once0 = 2;
           end
        else
           %set the sram1 enable
           if (do_once1 == 1)
               matlab_cbi_net_wr_mem(Socket, ControlLoc3+59, uint32(0));
               matlab_cbi_net_wr_mem(Socket, ControlLoc3+59, uint32(2));
               do_once1 = 2;
           end
        end
        %send it down to the front end
        matlab_cbi_net_wr_mem(Socket, ControlLoc3+4096, uint32(DataBuf));
        i = i + Increment + 1;
    end

    %clear the control bit
    matlab_cbi_net_wr_mem(Socket, ControlLoc3+59, uint32(0));
    set(handles.LogBox,'String','Done');

    set(handles.LogBox,'String','Starting ACE Player....');
    matlab_cbi_net_wr_mem(Socket, ControlLoc3+60, uint32(1));
    pause(1);
    prom_prog_status = (matlab_cbi_net_rd_mem(Socket, (ControlLoc3 + 60), 1, 12));
    while (prom_prog_status ~= 14352)
        set(handles.LogBox,'String',['Card3 ACE Player Busy: ',num2str(prom_prog_status)]);
        pause(1);
        prom_prog_status = (matlab_cbi_net_rd_mem(Socket, (ControlLoc3 + 60), 1, 12));
        if(prom_prog_status == 11789)
            set(handles.LogBox,'String',['Card3 ACE Player Error: ',num2str(prom_prog_status)]);
            return;
        end
    end
    set(handles.LogBox,'String','ACE Player Done');
end