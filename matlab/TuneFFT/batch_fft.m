function [response] = batch_fft()
  response = 0;
  current_directory = pwd;
  
  file_name = '';
  
  while isempty(regexpi(file_name,'done'))
    while isempty(file_name)
      prompt = 'Enter batch input file name: ';
      file_name = input(prompt, 's');
      if isempty(file_name)
        file_name = 'batch_input_file.txt';
      end
      if ~isempty(regexpi(file_name,'done'))
%         addpath('/home/gar39/in_closing');
%         GTFO;
          disp('Have a nice day!!! :)')
        return
      end
      [batch_input_file_ID,err_msg] = fopen(file_name,'r');
      if ~isempty(err_msg)
        disp(['Failed to open file. Make sure that it exists in the '...
          'directory: '])
        disp(current_directory);
        disp('or that the file exists in the file path.')
        file_name = '';
      end
      if isempty(err_msg)
        disp('Batch input file accepted, attempting to run batch file...')
      end
    end
    
    file_name = '';

    file_string = fgets(batch_input_file_ID);
    count = 0;

    while(isempty(regexpi(file_string,'!END!')))

      if ~isempty(regexp(file_string, '\S', 'match'))

        count = count + 1;
        disp(['Running batch input file block ' num2str(count)])
        disp(['This may take a while and the program may look frozen'...
          ' during this time.'])
        disp('Just relax. It''s all part of the plan... I hope.')

        % GET INPUT FILES
        file_string = file_string(1:length(file_string)-1);
        comment = regexpi(file_string,'!');
        input_file_str = file_string(1:comment-1);
        file_start_str = input_file_str(1:regexpi(input_file_str,'-')-1);
        file_start_num = str2double(file_start_str);
        file_stop_str = input_file_str(regexpi(input_file_str,'-')+1:...
          comment-1);
        file_stop_num = str2double(file_stop_str);
        infiles = '';
        for i=file_start_num:file_stop_num-1
          infiles=strcat(infiles,num2str(i),',');
        end
        infiles=strcat(infiles,num2str(file_stop_num));

        % GET BUNCH VECTOR AND NUMBUNCH
        file_string = fgets(batch_input_file_ID);
        file_string = file_string(1:length(file_string)-1);
        comment = regexpi(file_string,'!');
        numBunch_str = file_string(1:comment-1);
        numBunch = str2double(numBunch_str);
        bunchvec = zeros(1,numBunch);
        for i=1:numBunch
          bunchvec(i) = i;
        end

        % GET TURNM AND NUMTURNS
        file_string = fgets(batch_input_file_ID);
        file_string = file_string(1:length(file_string)-1);
        comment = regexpi(file_string,'!');
        numturns_str = file_string(1:comment-1);
        numturns = str2double(numturns_str);
        turnm = zeros(1,numturns);
        for i=1:numturns
          turnm(i) = i;
        end

        % GET TUNE_EXPECT AND TUNE_SHIFT
        file_string = fgets(batch_input_file_ID);
        file_string = file_string(1:length(file_string)-1);
        comment = regexpi(file_string,'!');
        tune_expect_x_str = file_string(1:comment-1);
        tune_expect_x = str2double(tune_expect_x_str);
        file_string = fgets(batch_input_file_ID);
        file_string = file_string(1:length(file_string)-1);
        comment = regexpi(file_string,'!');
        tune_shift_x_str = file_string(1:comment-1);
        tune_shift_x = zeros(1,2);
        tune_shift_x(1) = tune_expect_x-str2double(tune_shift_x_str);
        tune_shift_x(2) = tune_expect_x+str2double(tune_shift_x_str);
        file_string = fgets(batch_input_file_ID);
        file_string = file_string(1:length(file_string)-1);
        comment = regexpi(file_string,'!');
        tune_expect_y_str = file_string(1:comment-1);
        tune_expect_y = str2double(tune_expect_y_str);
        file_string = fgets(batch_input_file_ID);
        file_string = file_string(1:length(file_string)-1);
        comment = regexpi(file_string,'!');
        tune_shift_y_str = file_string(1:comment-1);
        tune_shift_y = zeros(1,2);
        tune_shift_y(1) = tune_expect_y-str2double(tune_shift_y_str);
        tune_shift_y(2) = tune_expect_y+str2double(tune_shift_y_str);
        tune_expect = zeros(1,2);
        tune_expect(1) = tune_expect_x;
        tune_expect(2) = tune_expect_y;
        tune_shift = zeros(2,2);
        tune_shift(1,:) = tune_shift_x;
        tune_shift(2,:) = tune_shift_y;

        % GET PARABFIT
        file_string = fgets(batch_input_file_ID);
        file_string = file_string(1:length(file_string)-1);
        comment = regexpi(file_string,'!');
        parabfit = str2double(file_string(1:comment-1));

        % GET PRINTNUM
        file_string = fgets(batch_input_file_ID);
        file_string = file_string(1:length(file_string)-1);
        comment = regexpi(file_string,'!');
        printnum = str2double(file_string(1:comment-1));

        % GET SAVEPLOTVEC
        saveplotvec = zeros(1,7);
        file_string = fgets(batch_input_file_ID);
        file_string = file_string(1:length(file_string)-1);
        comment = regexpi(file_string,'!');
        saveplotvec(1) = str2double(file_string(1:comment-1));
        file_string = fgets(batch_input_file_ID);
        file_string = file_string(1:length(file_string)-1);
        comment = regexpi(file_string,'!');
        saveplotvec(4) = str2double(file_string(1:comment-1));
        file_string = fgets(batch_input_file_ID);
        file_string = file_string(1:length(file_string)-1);
        comment = regexpi(file_string,'!');
        saveplotvec(5) = str2double(file_string(1:comment-1));

        % GET BPMNUM
        file_string = fgets(batch_input_file_ID);
        file_string = file_string(1:length(file_string)-1);
        comment = regexpi(file_string,'!');
        bpmnum = str2double(file_string(1:comment-1));

        % GET REMAINING FFTfilehandler2 ARGUMENTS
        ymin = 'none';
        ymax = 'none';
        tmin = -1;
        tmax = -1;
        powaxmaxstr = '0';
        gausswindow = 0;
        plotvec = zeros(1,7);

        % CALL FFTfilehandler2 AND RUN BATCH
        FFTfilehandler2(infiles,turnm,numturns,ymin,ymax,tmin,tmax,...
          powaxmaxstr,bunchvec,gausswindow,parabfit,plotvec,saveplotvec,...
          tune_expect,tune_shift,numBunch,printnum,bpmnum,0)

      end

      % GET NEXT TEST STRING
      file_string = fgets(batch_input_file_ID);

    end

    fclose(batch_input_file_ID);
  end
%   addpath('/home/gar39/in_closing');
%   GTFO;
  disp('Have a nice day!!! :)')
end
