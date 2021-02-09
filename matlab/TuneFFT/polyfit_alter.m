function [p,normerror] = polyfit_alter(x,y,n)

if ~isequal(size(x),size(y))
    
    error('MATLAB:polyfit:XYSizeMismatch',...
          'X and Y vectors must be the same size.')
end

x = x(:);
y = y(:);

if nargout > 2
    
   mu = [mean(x); std(x)];
   x = (x - mu(1))/mu(2);

end

V(:,n+1) = ones(length(x),1,class(x));                                      % Construct Vandermonde matrix.

for j = n:-1:1
    
   V(:,j) = x.*V(:,j+1);

end

[Q,R] = qr(V,0);                                                            % Solve least squares problem.
ws = warning('off','all'); 
p = R\(Q'*y);                                                               % Same as p = V\y;
warning(ws);

% if size(R,2) > size(R,1)
%    
%     warning('MATLAB:polyfit:PolyNotUnique', ...
%        'Polynomial is not unique; degree >= number of data points.')
% 
% elseif warnIfLargeConditionNumber(R)
%     
%     if nargout > 2
%         
%         warning('MATLAB:polyfit:RepeatedPoints', ...
%                 ['Polynomial is badly conditioned. Add points with distinct X\n' ...
%                  '         values or reduce the degree of the polynomial.']);
%     else
%         warning('MATLAB:polyfit:RepeatedPointsOrRescale', ...
%                 ['Polynomial is badly conditioned. Add points with distinct X\n' ...
%                  '         values, reduce the degree of the polynomial, or try centering\n' ...
%                  '         and scaling as described in HELP POLYFIT.']);
%     end
%     
% end

r = y - V*p;
p = p.';                                                                    % Polynomial coefficients are row vectors by convention.

                                                                            %Find maximum error in coefficients.
maxerror=cond(V)*norm(r)/norm(y);
normerror=norm(x)*maxerror;

S.R = R;                                                                    % S is a structure containing three elements: the triangular factor from a
S.df = max(0,length(y) - (n+1));                                            % QR decomposition of the Vandermonde matrix, the degrees of freedom and
S.normr = norm(r);                                                          % the norm of the residuals.

function flag = warnIfLargeConditionNumber(R)
if isa(R, 'double')
    flag = (condest(R) > 1e+10);
else
    flag = (condest(R) > 1e+05);
end
