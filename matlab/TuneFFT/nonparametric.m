function [sigma_values] = nonparametric(powerx,powery,frequency,...
                          tuneshift,interval_size,current_bpm,...
                          printnum)
  % powerfullx is a matrix with rows equal to the number of bunches and
  %   columns equal to the number of turns for the horizontal
  
  % powerfully is a matrix with rows equal to the number of bunches and
  %   columns equal to the number of turns for the vertical
  
  % frequency is a vector conataining the frequencies corresponding to
  %   powerfullx and powerfully
  
  % start_frequency is a scalar containing the frequency of which the tune
  %   shift is expected to occur after
  % stop_frequency is a scalar containing the frequency of which the tun
  %   shift is expected to occur before
  % in other words, the tune shift should occur between the start and stop
  %   frequency values. This is not needed for the algorithm to work, but
  %   it reduces the resource intensity of the algorithm and saves time
  
  % interval_size is a scalar that contains the number of points to be used
  %   to fit the notch or peak. The notch/peak (NP) does not usually have
  %   too many points, so making this number large will increase the
  %   reliability of positives but increases the number of false negatives.
  %   In other words, you can be sure the ones you find are real but you
  %   may miss some that are real. Making this number too small, will
  %   artificially inflate the p-value and increase number of false
  %   positives.
  
  % frequency vector is strictly decreasing. Algorithm and plotting
  % require frequency vector to be strictly increasing. Therefore,
  % frequency vector, powerx matrix, and powery matrix must be "flipped"
  temp_freq = frequency;
  temp_powx = powerx; 
  temp_powy = powery;
  for j=1:length(frequency)
    frequency(j) = temp_freq(length(temp_freq)-j+1);
    powerx(:,j) = temp_powx(:,length(temp_freq)-j+1);
    powery(:,j) = temp_powy(:,length(temp_freq)-j+1);
  end
  powerx = powerx./max(max(powerx))^2;
  powery = powery./max(max(powery))^2;
  [numb numturns] = size(powerx);
  num_peaks = 1;

%   for k = 1:numb
%     figure(k+2*numb*(current_bpm-1))
%     plot(frequency,powerx(k,:))
%   end
%   for k = 1:numb
%     figure(k+numb+2*numb*(current_bpm-1))
%     plot(frequency,powery(k,:))
%   end
  
%   powerx = log(powerx);
%   powery = log(pwoery);
  
  swarn = warning;
  warning off %#ok<WNOFF,*WNOFF>
  
  if(mod(interval_size,2) == 1)
    interval = interval_size - 1;
  else
    interval = interval_size;
  end
  half_interval = interval/2;
  
  start_freq_x = tuneshift(1,1);
  stop_freq_x = tuneshift(1,2);
  start_freq_y = tuneshift(2,1);
  stop_freq_y = tuneshift(2,2);
  [start_freq stop_freq] = find_freq(frequency,start_freq_x,stop_freq_x);
  start_freq_x = start_freq(1);
  stop_freq_x = stop_freq(1);
  [start_freq stop_freq] = find_freq(frequency,start_freq_y,stop_freq_y);
  start_freq_y = start_freq(1);
  stop_freq_y = stop_freq(1);
  x_ind = zeros(numb,1);
  y_ind = zeros(numb,1);
  
  % for each bunch
  for j = 1:numb
    % test inteveral is the interval that is being tested for the 
    % existance of the most likely point
    % FOR X VALUES
    
    possible_peaks = [];
    delta = 20;
    delta_increment = 0.5;
    x_peaks = powerx(j,start_freq_x:stop_freq_x);
    x_x = 1:length(x_peaks);
    while length(possible_peaks)<num_peaks
      [maxtab, mintab]=peakdet(x_peaks, delta, x_x); %#ok<NASGU>
      possible_peaks = maxtab;
      delta = delta-delta_increment;
      if delta <= 0
        delta = 20;
        delta_increment = delta_increment/2;
      end
    end
    possible_peaks = possible_peaks(1:num_peaks,:)+start_freq_x;
%     possible_peaks(:,1) = possible_peaks(1:5,1)+start_freq_x;
    interval_p_values = zeros(length(possible_peaks(:,1)),2);
    % for each point in the accepted range
    count_p_vals = 0;
    for k = possible_peaks(:,1)'
      count_p_vals = count_p_vals + 1;
      % index 
      % where test interval starts (inclusive)
      start = k-half_interval;
      % index where test interval stops (inclusive)
      stop = k+half_interval;
      % define the interval
      x_test_interval = powerx(j,start:stop);
      % declare a vector to contain points not in the test interval
      x_interval = [];
      % if points exist with index less than start, then add them to the
      % x_interval vector
      if(start>1)
        x_interval = powerx(j,1:start-1);
      end
      % if points exist with index greater than stop, then add them to
      % the x_interval vector
      if(stop<numturns)
        x_interval = [x_interval powerx(j,stop+1:end)]; %#ok<AGROW>
      end
      % save the index of the center point
      interval_p_values(count_p_vals,1) = k;
      % calculate p value for the center point
      [p_value, wilcoxon, expectation, variance, z] =...
        rank_sum_test(x_test_interval,...
        x_interval);%#ok<NASGU,NASGU,NASGU,*NASGU> %NASGU
      % save the p value for the center point
      interval_p_values(count_p_vals,2) = p_value;
    end
    
    % sort the p values for x
    sorted_x_p_values = mergeSortWithIndex(interval_p_values);
    len = length(sorted_x_p_values(:,1));
    [temp_val,temp_ind] = max(powerx(j,...
      sorted_x_p_values(len,1)-half_interval:...
      sorted_x_p_values(len,1)+half_interval));
    x_ind(j) = sorted_x_p_values(len,1)-half_interval-1+temp_ind;

    % FOR Y VALUES
    possible_peaks = [];
    delta = 20;
    y_peaks = powery(j,start_freq_y:stop_freq_y);
    y_y = 1:length(y_peaks);
    while length(possible_peaks)<num_peaks
      [maxtab, mintab]=peakdet(y_peaks, delta, y_y); %#ok<NASGU>
      possible_peaks = maxtab;
      delta = delta-0.25;
      if delta <= 0
        delta = 20;
        delta_increment = delta_increment/2;
      end
    end
    
    possible_peaks = possible_peaks(1:num_peaks,:)+start_freq_y;
%     possible_peaks(:,1) = possible_peaks(:,1)+start_freq_y;
    interval_p_values = zeros(length(possible_peaks(:,1)),2);
    % for each point in the accepted range
    count_p_vals = 0;
    % for each point in the accepted range
    for k = possible_peaks(:,1)'   
      count_p_vals = count_p_vals + 1;
      % index where test interval starts (inclusive)
      start = k-half_interval;
      % index where test interval stops (inclusive)
      stop = k+half_interval;
      % define the interval
      y_test_interval = powery(j,start:stop);
      % declare a vector to contain points not in the test interval
      y_interval = [];
      % if points exist with index less than start, then add them to the
      % x_interval vector
      if(start>1)
        y_interval = powery(j,1:start-1);
      end
      % if points exist with index greater than stop, then add them to
      % the x_interval vector
      if(stop<numturns)
        y_interval = [y_interval powery(j,stop+1:end)]; %#ok<AGROW>
      end
      % save the index of the center point
      interval_p_values(count_p_vals,1) = k;
      % calculate p value for the center point
      [p_value, wilcoxon, expectation, variance, z] =...
              rank_sum_test(y_test_interval,...
              y_interval); %#ok<NASGU,NASGU> %NASGU
      % save the p value for the center point
      interval_p_values(count_p_vals,2) = p_value;
    end
    
    % sort the p values
    sorted_y_p_values = mergeSortWithIndex(interval_p_values);
    len = length(sorted_y_p_values(:,1));
    [temp_val,temp_ind] = max(powery(j,...
      sorted_y_p_values(len,1)-half_interval:...
      sorted_y_p_values(len,1)+half_interval));
    y_ind(j) = sorted_y_p_values(len,1)-half_interval-1+temp_ind;
  end
  
  % fit to Gaussian
  [A_x,A_y,mu_x,mu_y,sigma_x,sigma_y] =...
    fit_to_gaussian(frequency,powerx,powery,x_ind,y_ind);
  sigma_values = [mu_x sigma_x mu_y sigma_y];
  
  if printnum == 5
    for k = 1:numb
      figure(k+2*numb*(current_bpm-1))
      [y_test] = gaussian(frequency,A_x(k,1),mu_x(k,1),sigma_x(k,1));
      y_comp = powerx(k,:) + abs(mean(powerx(k,:)));
      amp = y_comp(x_ind(k));
      loc = frequency(x_ind(k));
      lat(1,1:2) = [frequency(1) frequency(end)];
      lat(2,1:2) = [amp amp];
      lon(1,1:2) = [loc loc];
      lon(2,1:2) = [min(y_comp)-1 max(y_comp)+1];
      plot(frequency,y_comp,'b-',frequency,y_test,'r*',...
        lat(1,:),lat(2,:),'k-',lon(1,:),lon(2,:),'k-',loc,amp,'mo')
      title_line1 = ['Bunch: ' num2str(k)];
      title_line2 = 'X Direction';
      title({title_line1;title_line2})
      axis([frequency(1),frequency(end),1.2*min(y_comp),1.4*max(y_comp)])
    end
    for k = 1:numb
      figure(k+numb+2*numb*(current_bpm-1))
      [y_test] = gaussian(frequency,A_y(k),mu_y(k),sigma_y(k));
      y_comp = powery(k,:) + abs(mean(powery(k,:)));
      amp = y_comp(y_ind(k));
      loc = frequency(y_ind(k));
      lat(1,1:2) = [frequency(1) frequency(end)];
      lat(2,1:2) = [amp amp];
      lon(1,1:2) = [loc loc];
      lon(2,1:2) = [min(y_comp)-1 max(y_comp)+1];
      plot(frequency,y_comp,'b-',frequency,y_test,'r*',...
        lat(1,:),lat(2,:),'k-',lon(1,:),lon(2,:),'k-',loc,amp,'mo')
      title_line1 = ['Bunch: ' num2str(k)];
      title_line2 = 'Y Direction';
      title({title_line1;title_line2})
      axis([frequency(1),frequency(end),1.2*min(y_comp),1.4*max(y_comp)])
    end
  end
  warning(swarn);
end
function [A_x,A_y,mu_x,mu_y,sigma_x,sigma_y] =...
  fit_to_gaussian(frequency,powerx,powery,x_ind,y_ind)
  % INPUT ARGUMENTS
  % frequency - vector containing frequency values
  % powerx - contains x power spectrum
  %          matrix in the form (bunch number,spectrum)
  % powery - contains x power spectrum
  %          matrix in the form (bunch number,spectrum)
  % half_interval - scalar containing the size of the half interval
  %                 length of interval = 2*half_interval+1
  % x_ind - contians the index of most likely points for x direction
  %         matrix in the form (bunch number,most likely points in
  %         decreasing order)
  % y_ind - contians the index of most likely points for y direction
  %         matrix in the form (bunch number,most likely points in
  %         decreasing order)
  
  % determine number of bunches stored in powerx and powery matrix
  numb = length(powerx(:,1));
  
  % declare vectors to store Gaussian fit parameters
  A_x = zeros(numb,1);
  mu_x = zeros(numb,1);
  sigma_x = zeros(numb,1);
  A_y = zeros(numb,1);
  mu_y = zeros(numb,1);
  sigma_y = zeros(numb,1);
  
  % for each bunch the matrices
  for i = 1:numb
    % for each most likely point for each bunch
    xk = powerx(i,:);
    xk = xk+abs(mean(xk));
    yk = powery(i,:);
    yk = yk+abs(mean(yk));
    fx = frequency(:);
    fy = frequency(:);
    [A_x(i), mu_x(i), sigma_x(i)] = fitgauss(xk, fx,...
      powerx(i,x_ind(i)), frequency(x_ind(i)), 1.5);
    sigma_x(i) = abs(sigma_x(i));
    [A_y(i), mu_y(i), sigma_y(i)] = fitgauss(yk, fy,...
      powery(i,y_ind(i)), frequency(y_ind(i)), 1.5);
    sigma_y(i) = abs(sigma_y(i));
    if sigma_x(i) > 10
      A_x(i) = NaN;
      mu_x(i) = NaN;
      sigma_x(i) = NaN;
    end
    if sigma_y(i) > 10
      A_y(i) = NaN;
      mu_y(i) = NaN;
      sigma_y(i) = NaN;      
    end
  end
end
function y = mergeSortWithIndex(x)
  % x is an Nx2 matrix
  %  column 1 contains the index of each value
  % y is an Nx2 matrix
  %  consists of the values in the 2nd column of x sorted 
  %  from smallest to largest.
  [n val] = size(x); %#ok<NASGU>
  if n==1
    y = x;
  else
    m = floor(n/2);
    % Sort the first half..
    y1 = mergeSortWithIndex(x(1:m,:));
    % Sort the second half...
    y2 = mergeSortWithIndex(x(m+1:n,:));
    % Merge...
    y = MergeWithIndex(y1,y2); 
  end
end
function z = MergeWithIndex(x,y)
  % x is a row n-vector with x(1) <= x(2) <= ... <= x(n)
  % y is a row m-vector with y(1) <= y(2) <= ... <= y(m)
  % z is a row (m+n)-vector comprised of all the values in x and
  % y and sorted so that z(1) <= ... <= z(m+n)
  n = length(x(:,1)); m = length(y(:,1)); z = zeros(n+m,length(x(1,:)));
  ix = 1; % The index of the next x-value to select.
  iy = 1; % The index of the next y-value to select.
  for iz=1:(n+m)
    % Deteremin the iz-th value for the merged array...
    if ix > n
    % All done with x-values. Select the next y-value.
    z(iz,:) = y(iy,:);
    iy = iy+1;
    elseif iy>m
    % All done with y-values. Select the next x-value.
    z(iz,:) = x(ix,:);
    ix = ix + 1;
    elseif x(ix,2) <= y(iy,2)
    % The next x-value is less than or equal to the next y-value
    z(iz,:) = x(ix,:);
    ix = ix + 1;
    else
    % The next y-value is less than the next x-value
    z(iz,:) = y(iy,:);
    iy = iy + 1;
    end
  end
end
function [start_freq stop_freq] = find_freq(frequency,start_frequency,...
  stop_frequency)
% Returns two vectors
% start_freq is a two column vector. First element is the frequency index.
%   Second element is the actual frequency value.
% stop_freq is a two column vector. First element is the frequency index.
%   Second element is the actual frequency value.

  % make vec a row vector first
  [rows cols] = size(frequency);
  if(rows > cols)
    vec = frequency;
  elseif(cols > rows)
    vec = frequency';
  end
  % add a second column to vec to track the index each value
  %   corresponds to in the original frequency vector
  vec = [(1:length(vec))' vec];
  vec2 = vec;
  for k = 1:2
    % use bisection method to quickly find the nearest start and stop
    %   frequncies in the frequency vector. Find frequency just below the
    %   start frequency and find frequency just after the stop frequency
    %   to ensure entire frequency interval is captured.
    found = 0;
    vec = vec2;
    m = ceil(length(vec)/2);
    if(k==1) % search for start frequency
      while(~found)
        if(length(vec(:,1)) <= 2)
          start_freq = vec(1,:);
          found = 1;
        elseif(vec(m,2) > start_frequency)
          vec = vec(1:m,:);
          m = ceil(length(vec(:,1))/2);
        elseif(vec(m,2) < start_frequency)
          vec = vec(m:end,:);
          m = ceil(length(vec(:,1))/2);
        elseif(vec(m,2) == start_frequency)
          start_freq = vec(m,:);
          found = 1;
        end
      end
    elseif(k==2) % search for stop frequency      
      while(~found)
        if(length(vec(:,1)) <= 2)
          stop_freq = vec(end,:);
          found = 1;
        elseif(vec(m,2) > stop_frequency)
          vec = vec(1:m,:);
          m = ceil(length(vec(:,1))/2);
        elseif(vec(m,2) < stop_frequency)
          vec = vec(m:end,:);
          m = ceil(length(vec(:,1))/2);
        elseif(vec(m,2) == stop_frequency)
          stop_freq = vec(m,:);
          found = 1;
        end
      end      
    end
  end
end
function [p_value, wilcoxon, expectation, variance, z] =...
              rank_sum_test(x_input,y_input)
  error = 0;
  x = x_input;
  y = y_input;
  [m n] = size(x);
  if(m<n) % Make x a column vector
    x = x';
    [m n] = size(x);
  end
  if(~(m>1 && n==1)) % Check that x is a vector
    disp(' ');
    disp('Error in function rank_sum_test');
    disp('The first input argument is not a vector');
    disp(['size: rows = ' num2str(m) ' cols = ' num2str(n)]);
    disp(' ');
    error = 1;
  end
  [m n] = size(y);
  if(m<n) % Make y a column vector
    y = y';
    [m n] = size(y);
  end
  if(~(m>1 && n==1)) % Check that y is a vector
    disp(' ');
    disp('Error in function rank_sum_test');
    disp('The second input argument is not a vector');
    disp(['size: rows = ' num2str(m) ' cols = ' num2str(n)]);
    disp(' ');
    error = 1;
  end
  if(error) % If the input arguments were not vectors, kill the subroutine
    disp('Error occured in function rank_sum_test');
    disp(' ');
    return
  end
  m = length(x); % Store size of population 1 vector
  n = length(y); % Store size of population 2 vector
  % make x the test interval
  if(m>n)
    temp = x;
    x = y;
    y = temp;
  end
  m = length(x); % Store size of population 1 vector
  n = length(y); % Store size of population 2 vector
  x = [ones(m,1) x]; % Add a second column to track indices of pop 1
  y = [2*ones(n,1) y]; % Add a second column to track indices of pop 2
  z = [x;y];
  % sort the matrix
  sorted = mergeSortWithIndex(z);
  % deal with ties
  size_of_tie = [];
  size_of_tie(1,1) = 1;
  sorted = [sorted (1:m+n)'];
  count = 1;
  last = sorted(1,2);
  for j = 2:m+n
    if(sorted(j,2) == last)
      size_of_tie(count,1) = size_of_tie(count,1) + 1; %#ok<AGROW>
    else
      num = size_of_tie(count,1);
      s = sum(sorted(j-num:j-1,3));
      s = s/num;
      sorted(j-num:j-1,3) = s;
      count = count + 1;
      size_of_tie(count,1) = 1; %#ok<AGROW>
    end
    last = sorted(j,2);
  end
  wilcoxon = 0;
  for j = 1:m+n
    if(sorted(j,1) == 1)
      wilcoxon = wilcoxon + sorted(j,3);
    end
  end
  expectation = m*(m+n+1)/2;
  variance = m*n*(m+n+1)/12;
  summation = 0;
  for j = 1:length(size_of_tie)
    summation = summation + (size_of_tie(j)-1)*size_of_tie(j)*...
                                                       (size_of_tie(j)-1);
  end
  variance = variance - m*n/(12*(m+n)*(m+n-1))*summation;
  z = (wilcoxon-expectation)/sqrt(variance);
  % calculate p value
  p_value =  normcdf(z,0,1);
end
function output = gaussian(freq,A,mu,sigma)
  output = zeros(length(freq),1);
  for j = 1:length(freq)
    expo = -(freq(j)-mu)^2/(2*(sigma^2));
    output(j) = A*exp(expo);
  end
end
function [A, mu, sigma] = fitgauss(yk, xk, guess_A, guess_mu,...
  guess_sigma)
  % [A, MU, SIGMA] = FITGAUSS(Y, X, GUESS_A, GUESS_MU, GUESS_SIGMA)
  %
  % Fits a Normal Distribution, N(x), to the data in Y and X.
  %
  % Y           - Y-Data (1xM)
  % X           - X-Data (1xM)
  % GUESS_A     - (Optional) Initial guess for Area
  % GUESS_MU    - (Optional) Initial guess for Mean
  % GUESS_SIGMA - (Optional) Initial guess for Standard deviation
  %
  % A           - Fitted Area
  % MU          - Fitted Mean
  % SIGMA       - Fitted Standard Deviation
  % 
  % If an optional parameter is not specified or is [], an initial guess
  % will automatically be calculated for that parameter.
  %
  % N(x) = A * exp(-0.5*((x-mu)/sigma)^2) / sqrt(2*pi*sigma^2);
  %
  % See also EXP, ERF, ERFC, RANDN.

  % Mar 2005 James R. Blake
  % Updated Apr 2005 JRB
  % Updated Jul 2005 JRB, sigma could be equal to zero fix
  %                       bug if no maxima found fixed
  % Updated Sep 2005 MJK, code cleanup (no change to algorithm)

  MAX_ITERATIONS = 100;
  CHI2_REL_TOL   = 1e-3;
  
  % Check input arguments
  error(nargchk(2,5,nargin));
  if ~isnumeric(yk), error('Y must be numeric.'); end
  if ~isnumeric(xk), error('X must be numeric.'); end
  if length(xk) ~= length(yk), error('X and Y must have same length.');end
  
  % === clear any warnings ===
  swarn = warning;
  warning off %#ok<WNOFF,*WNOFF>

  mu_save = 0; %#ok<NASGU>
  
  % Initialise variables
  if exist('guess_mu', 'var') && ~isempty(guess_mu)
    if ~isnumeric(guess_mu) || length(guess_mu) ~= 1
      error('GUESS_MU must be a numeric scalar.');
    end
    mu = guess_mu;
    mu_save = mu; %#ok<NASGU>
  else
    % Guess mean (MU)
    idx = findmaxima(yk);
    if isempty(idx)
      % No peak (probably a peak off the edge). Poor chance of convergence
      % The best we can do is to start MU at highest the location of the
      % maximum.
      [dummy, idx2] = max(yk); %#ok<*ASGLU>
      mu = xk(idx2);
    else
      % We have peak(s). Set to highest peak.
      [dummy, idx2] = max(yk(idx));
      mu = xk(idx(idx2));
%       mu_save = mu;
    end
  end

  if exist('guess_sigma', 'var') && ~isempty(guess_sigma)
    if ~isnumeric(guess_sigma) || length(guess_sigma) ~= 1
      error('GUESS_SIGMA must be a numeric scalar.');
    end
    sigma = guess_sigma;
  else
    % Guess SIGMA to be about halfway down from peak.
    mk = yk > (max(yk)/2 + max(yk)/length(yk));
    tophalf = yk(mk);                   % top half values
    st = find(yk==tophalf(1));          % only use first value at top half
    sigma = abs(mu-xk(st(1)));          % now SD=mu-1st value over FWHM
    % Guess for sigma might be zero !!
    if abs(sigma) < 1e-8
      % Stab in the dark, could be improved
      sigma = xk(end)/2;
    end
  end

  if exist('guess_A', 'var') && ~isempty(guess_A)
    if ~isnumeric(guess_A) || length(guess_A) ~= 1
      error('GUESS_A must be a numeric scalar.');
    end
    A = guess_A;	
  else
    A = traprule(yk,mean(diff(xk)));
  end


  % Form A or rather Ai
  Ai = [A mu sigma];
  Ai = Ai(:);

  % Initialised, now solve ....
  counter = 0;
  loop    = 1;
  lambda  = 1000;
  Fold = [0.01 0.01 0.01];
  Fold = Fold(:);
  while loop==1
    counter = counter+1;
    Ah    = Ai;       % Store old values
    A     = Ai(1);    % Area factor
    mu    = Ai(2);    % Mean
    sigma = Ai(3);    % Standard deviation
    fx = fgauss(xk,A,mu,sigma); % abbreviation...xk = independent variable
    [aa bb] = size(yk);
    if aa < bb, yk = yk'; end
    rk = yk-fx;

    % Calculate the Jacobian
    % First derivatives
    dfdA     = fx ./ A;
    dfdmu    = fx .* (xk-mu) ./ sigma^2;
    dfdsigma = fx .* ((xk-mu).^2 / sigma^3);
    % Second derivatives
    %% d2fdA2 = 0;
    %% d2fdAdsigma  = dfdsigma ./ A;
    %% d2fdAdmu     = dfdmu ./ A;
    %% d2fdsigma2   = fx .* ((xk-mu).^4/sigma^6-5*(xk-mu).^2/sigma^4+...
    %% 2/sigma^2);
    %% d2fdsigmadmu = dfdmu .* ((xk-mu).^2/sigma^3-3/sigma);
    %% d2fdmu2      = dfdmu .* (xk-mu)/sigma^2-fx./sigma^2;

    % Form Jacobian
    Jacob(1,1) = sum((1+lambda)*dfdA.^2);
    Jacob(1,2) = sum(dfdA.*dfdmu);
    Jacob(1,3) = sum(dfdA.*dfdsigma);
    Jacob(2,1) = Jacob(1,2);
    Jacob(2,2) = sum((1+lambda)*dfdmu.^2);
    Jacob(2,3) = sum(dfdmu.*dfdsigma);
    Jacob(3,1) = Jacob(1,3);
    Jacob(3,2) = Jacob(2,3);
    Jacob(3,3) = sum((1+lambda)*dfdsigma.^2);
    % Form F
    F(1) = sum(rk .* dfdA);
    F(2) = sum(rk .* dfdmu);
    F(3) = sum(rk .* dfdsigma);
    F = F(:);
    Ai = Ah + Jacob\F;

    rkold   = yk-fgauss(xk,Ah(1),Ah(2),Ah(3));  % for testing the end of
    % the loop
    chi2old = sumsq(rkold);
    rk      = yk-fgauss(xk,Ai(1),Ai(2),Ai(3));
    chi2    = sumsq(rk);

    % Calculate relative change in Chi^2 from previous iteration.
    pc_change = (sumsq(F)-sumsq(Fold)) / sumsq(Fold);
    Fold = F;

    if chi2 > chi2old
      % Residuals have INCREASED. Make refinements coarser.
      lambda = lambda*10;
    else
      if (chi2-chi2old < 1) && (abs(pc_change) < CHI2_REL_TOL), loop=0;end
      % Residuals have decreased, make finer refinements.
      lambda = lambda/10;
    end

    if counter > MAX_ITERATIONS, loop=0; end
    if isnan(chi2)
      A = NaN; mu = NaN; sigma = NaN; warning(swarn); return
    end
  end
  test_val = (A-abs(mean(yk)))-1.75*std(yk);
  if test_val < 0
    A = NaN; mu = NaN; sigma = NaN; warning(swarn); return
  end

  % Restore original warning settings
  warning(swarn);
  idx = findmaxima(yk);
  [dummy, idx2] = max(yk(idx));
  A = yk(idx(idx2));
%   if mu_save ~= 0, mu = mu_save; end
end
function output = fgauss(x,A,mu,sigma)
  % Abbreviation to evaluate Gaussian functions.
  output = A * exp(-0.5*((x-mu)./sigma).^2);
end

function a = sumsq(b,dim)
  % A = SUMSQ(B,DIM)
  %
  % Returns the sum of the square of each element in the matrix
  % If two arguments are specified, then it sums the squares in
  % a particular dimension.

  if nargin==1, a = sum(b.*b); return; end
  if ndims(b) < dim, error('DIM exceeds number of dimensions of B.'); end
  a = sum(b.*b,dim);
end
function idx = findmaxima(y)
  % Finds indices of Y where Y is locally maximum.

  if length(y) < 3, idx = []; return; end
  grad = diff(y);
  idx = find(grad(2:end) < 0 & grad(1:end-1) > 0) + 1;
end
function output = traprule(y,sep)
  % Approximate the area under a curve using the trapezium rule
  % OUTPUT = TRAPRULE(Y, SEP)
  %
  % Y   - Y values at each x sample
  % SEP - X separation between samples
  %
  % OUTPUT - Area

  if nargin==2
    output = sep*(sum(y(:)) - 0.5*(y(1)+y(end)));
  else
    output =      sum(y(:)) - 0.5*(y(1)+y(end));
  end
end
function [maxtab, mintab]=peakdet(v, delta, x)
  %PEAKDET Detect peaks in a vector
  %        [MAXTAB, MINTAB] = PEAKDET(V, DELTA) finds the local
  %        maxima and minima ("peaks") in the vector V.
  %        MAXTAB and MINTAB consists of two columns. Column 1
  %        contains indices in V, and column 2 the found values.
  %      
  %        With [MAXTAB, MINTAB] = PEAKDET(V, DELTA, X) the indices
  %        in MAXTAB and MINTAB are replaced with the corresponding
  %        X-values.
  %
  %        A point is considered a maximum peak if it has the maximal
  %        value, and was preceded (to the left) by a value lower by
  %        DELTA.

  % Eli Billauer, 3.4.05 (Explicitly not copyrighted).
  % This function is released to the public domain; Any use is allowed.

  maxtab = [];
  mintab = [];

  v = v(:); % Just in case this wasn't a proper vector

  if nargin < 3
    x = (1:length(v))';
  else 
    x = x(:);
    if length(v)~= length(x)
      error('Input vectors v and x must have same length');
    end
  end

  if (length(delta(:)))>1
    error('Input argument DELTA must be a scalar');
  end

  if delta <= 0
    error('Input argument DELTA must be positive');
  end

  mn = Inf; mx = -Inf;
  mnpos = NaN; mxpos = NaN;

  lookformax = 1;

  for i=1:length(v)
    this = v(i);
    if this > mx, mx = this; mxpos = x(i); end
    if this < mn, mn = this; mnpos = x(i); end

    if lookformax
      if this < mx-delta
        maxtab = [maxtab ; mxpos mx]; %#ok<AGROW>
        mn = this; mnpos = x(i);
        lookformax = 0;
      end  
    else
      if this > mn+delta
        mintab = [mintab ; mnpos mn]; %#ok<AGROW>
        mx = this; mxpos = x(i);
        lookformax = 1;
      end
    end
  end
end