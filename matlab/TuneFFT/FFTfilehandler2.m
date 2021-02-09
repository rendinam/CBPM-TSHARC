function FFTfilehandler2(infiles,turnm,numturns,ymin,ymax,tmin,tmax,...
  powermaxstr,bunchvec,gauss,parabfit,plotvec,saveplotvec,tune_expect,...
  tune_shift,numbun,printnum,bpmnum,show_progress_bar)
  if ispc
      slashtype='\'; %#ok<NASGU>
  else
      slashtype='/'; %#ok<NASGU>
  end
  
  % Obtain the list of files to process
  numfiles=1;                                                                 %sets the number of files to 1 initially
  [token,remain]=strtok(infiles,',');                                         %after it to remain
  fileno{1}=str2num(token);
  
  filenum=fix(fileno{1}/1000);                                                % sorts into proper directory

  if filenum < 10                                                              % directory is two digits
      filedir=strcat('0',num2str(filenum,'%1.1f'));
      filedir=filedir(1:2);
  else
      filedir=num2str(filenum,'%2.2f');
      filedir=filedir(1:2);
  end

  while ~isempty(remain)                                                      %Until remain is empty... (no file names are left in the list)                  
      [token,remain]=strtok(remain,',');                                      %set the next filename to token and everything afterward to remain
      numfiles=numfiles+1;                                                    %increment the number of files
      fileno{numfiles}=str2num(token);                                        %creates file array
  end
  
  % Set parameters for plotting
  if (str2double(powermaxstr))~=0
      [powx,powermaxstr]=strtok(powermaxstr,',');                         %sets maximum vertical power axis
      powmax(1)=str2double(powx);
      [powy,powermaxstr]=strtok(powermaxstr,',');
      powmax(2)=str2double(powy);
  else
      powmax=[0 0];                                                       %if no maximum power is set
  end
  if strcmp(ymin,'none') ~= 1                                                 %sets vertical axes for x and y
      [ymin1,ymin]=strtok(ymin,',');
      yminx=str2double(ymin1);
      [ymin2,ymin]=strtok(ymin,',');
      yminy=str2double(ymin2);
      ymin=[yminx,yminy];
      
      [ymax1,ymax]=strtok(ymax,',');
      ymaxx=str2double(ymax1);
      [ymax2,ymax]=strtok(ymax,',');
      ymaxy=str2double(ymax2);
      ymax=[ymaxx,ymaxy];
  end
  
  % Get parameters and initialize the working arrays
  [A,bunches]=size(bunchvec);                                                 %gets number of bunches
  turnnum=turnm(end)-turnm(1)+1;                                              %gets number of turns
  ypos=zeros(bunches,turnnum);                                                %sets array for xpos
  xpos=zeros(bunches,turnnum);                                                %sets array for ypos
  fftx=zeros(bunches,turnnum);                                                %sets array for fftx
  ffty=zeros(bunches,turnnum);                                                %sets array for ffty
  time=(turnm(1):turnm(end)).*2.5633;                                         %sets time array (each turn is 2.5633 microseconds)
    
  % Loop over the files to be processed
  for jfil=1:numfiles
      % Check that GUI inputs are consistent with the file
      cbpm_read_rawfile_m(fileno{jfil});
      filename=strcat('RD-00',num2str(fileno{jfil}));
      if (fileno{jfil} > 9999)
         filename=strcat('RD-0',num2str(fileno{jfil}));
      end
      disp(['reading file ',filename]);
      turncount=cbpm_file_num_turns_m();
      if (turnm(end) ~= turncount)
          disp(['Specified turn count ',num2str(turnm(end)),' inconsistent with file ',num2str(turncount)]);
      end
      bunchcount=cbpm_file_num_bunches_m();
      if (bunches ~= bunchcount)
          disp(['Specified bunch count ',num2str(bunches),' inconsistent with file ',num2str(bunchcount)]);
      end
      bpmcount=cbpm_file_num_insts_m();
      if (bpmnum > bpmcount)
          disp(['You have specified ',num2str(bpmnum), ' BPMs but there are only ',num2str(bpmcount)]);
      else
          bpmcount=bpmnum;                                                 % Process only bpmnum of the bpmcount BPMs recorded
      end
      
      % Get the BPM names
      bpm_names = {};
      for kbpm=1:bpmcount
          bpm_names=[bpm_names cbpm_file_inst_locs_m()];
      end
      
      % Create a subdirectory for the image outputs if necessary
      if strcmp(pwd,'/media/usbdisk/dlk_tunes') || strcmp(pwd,'/media/usbdisk1/dlk_tunes') || strcmp(pwd,'/home/mjrandaz/TuneFFT/')
          newdir=strcat(pwd,'/',filename,'_Images');
      else
          %online          newdir=strcat('/nfs/cesr/online/instr/anal/cbpmII/tune/',filedir,'/',filename,'_Images');
          %Offline         newdir=strcat('/nfs/cesr/Offline/instr/anal/cbpmII/tune/',filedir,'/',filename,'_Images');
          %Test
          newdir=strcat('/nfs/ilc/sim2/dlk8/TuneFFTRD/TuneFFT/',filedir,'/',filename,'_Images');
      end
      
      if exist(newdir,'dir') ~= 7
          disp(['Creating directory ',newdir]);
          mkdir(newdir);
      end
      
      % Loop over BPMs
      results = [];
      for kbpm=1:bpmcount
          counter=1;                                                        % counter counts used bunch numbers
          positions=cbpm_file_pos_data_m(bpm_names{kbpm});                  % Read turn-by-turn positions by bunch for x and y
                                                                            % The returned array is dimensioned (bunches*turnnum,2)
                                                                            % (x-bunch1-turn1, y-bunch1-turn1)
                                                                            % (x-bunch1-turn2, y-bunch1-turn2)
                                                                            % (x-bunch1-turn3 ...
                                                                            ...
                                                                            % (x-bunch45-turn2048, y-bunch45-turn2048)

          % Loop over bunches
          for ibun=1:numbun

              % Is this one of the bunches we wanted?
              if find(bunchvec == ibun) > 0
                  xpos(counter,:)=positions((ibun-1)*turnnum+1:ibun*turnnum,1);
                  ypos(counter,:)=positions((ibun-1)*turnnum+1:ibun*turnnum,2);
                  fftx(counter,:)=fft(xpos(counter,:));
                  ffty(counter,:)=fft(ypos(counter,:));
                  counter=counter+1;
              end

              if ibun <= bunchvec(end) && ~isempty(find(bunchvec==ibun)) && show_progress_bar
                [stopBar,stopper] = progressbar((counter-1)/bunches,0,filename,ibun,numbun,bpm_names{kbpm});
              end
          end
          [raw_results] = plotoptions(filename,bunches,bunchvec,...
              plotvec,xpos,ypos,time,ymin,ymax,tmin,tmax,turnnum,...
              saveplotvec,powmax,fftx,ffty,gauss,tune_expect,...
              tune_shift,parabfit,printnum,newdir,bpm_names{kbpm},kbpm);
          results = cat(3,results,raw_results);
      end
    
    % Publish the results if print is requested  
    if printnum == 4 || printnum == 5
        x_freqs = [];
        y_freqs = [];
        x_sigmas = [];
        y_sigmas = [];
        for i=1:length(results(1,1,:))
            x_freqs = [x_freqs results(:,1,i)]; %#ok<AGROW>
            x_sigmas = [x_sigmas results(:,2,i)]; %#ok<AGROW>
            y_freqs = [y_freqs results(:,3,i)]; %#ok<AGROW>
            y_sigmas = [y_sigmas results(:,4,i)]; %#ok<AGROW>
        end
        mean_x_freqs = zeros(length(results(:,1,1)),1);
        mean_y_freqs = zeros(length(results(:,1,1)),1);
        mse_x_freqs = zeros(length(results(:,1,1)),1);
        mse_y_freqs = zeros(length(results(:,1,1)),1);
        mae_x_freqs = zeros(length(results(:,1,1)),1);
        mae_y_freqs = zeros(length(results(:,1,1)),1);
        for i=1:length(results(:,1,1))
            mean_x_freqs(i) = nanmean(x_freqs(i,:));
            mean_y_freqs(i) = nanmean(y_freqs(i,:));
            mse_x_freqs(i) = nanstd(x_freqs(i,:))/...
                sum(~isnan(x_freqs(i,:)));
            mse_y_freqs(i) = nanstd(y_freqs(i,:))/...
                sum(~isnan(y_freqs(i,:)));
            for k = 1:length(x_freqs(1,:))
                if ~isnan(x_freqs(i,k))
                    mae_x_freqs(i) = mae_x_freqs(i) +...
                        abs(mean_x_freqs(i) - x_freqs(i,k));
                end
                if ~isnan(y_freqs(i,k))
                    mae_y_freqs(i) = mae_y_freqs(i) +...
                        abs(mean_y_freqs(i) - y_freqs(i,k));
                end
            end
            mae_x_freqs(i) = mae_x_freqs(i)/sum(~isnan(x_freqs(i,:)));
            mae_y_freqs(i) = mae_y_freqs(i)/sum(~isnan(y_freqs(i,:)));
        end
        textfile=strcat(newdir,'/',filename,'_summary.dat');
        textid=fopen(textfile,'wt');
        form = '%6s %14.4f %14.4f %14.4f %s\n';
        headertext = ' Bunch      mean freq            MSE            MAE   ';
        for k = 1:bpmcount
            headertext = sprintf('%s  sigma %s   ',headertext,char(bpm_names(k)));
            form = strcat(form,'%14.10f');
        end
        fprintf(textid,'Frequency for most likely point for file %s in the x direction.\n',filename);
        fprintf(textid,'%s\n', headertext);
        for k = 1:length(mean_x_freqs)
            strx = sprintf('%14.10f',x_sigmas(k,:));
            bunch_str = num2str(k);
            if(k/10<1)
                bunch_str = ['0' bunch_str]; %#ok<AGROW>
            end
            fprintf(textid,form,bunch_str,mean_x_freqs(k),mse_x_freqs(k),mae_x_freqs(k),strx);
        end
        fprintf(textid,'Frequency for most likely point for file %s in the y direction.\n',filename);
        fprintf(textid,'%s\n', headertext);
        for k = 1:length(mean_y_freqs)
            strx = sprintf('%14.10f',y_sigmas(k,:));
            bunch_str = num2str(k);
            if(k/10<1)
                bunch_str = ['0' bunch_str]; %#ok<AGROW>
            end
            fprintf(textid,form,bunch_str,mean_y_freqs(k),mse_y_freqs(k),mae_y_freqs(k),strx);
        end
        fclose(textid);
    end
  end
end
