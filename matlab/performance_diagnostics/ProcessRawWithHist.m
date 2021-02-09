% TestScript

ca

% try
% 
% I = []; S = []; H = []; H(300000) = 0; N = 100; 
% namess = {};
% 
% [num_insts1,names1] = ReadInRawDataFile(12677,1);
% for i = 1:num_insts1; 
%     id = cbpm_file_raw_data_m(names1{i});
%     S(i,:) = sum(id'); 
%     [~,I(i,:)] = sort(sum(id')); 
%     H(I(1:N)) = H(I(1:N))+ones(1,N);
%     namess{i} = names1{i}; end
% 
% [num_insts2,names2] = ReadInRawDataFile(12676,1);
% ind = 1;
% for i = num_insts1:num_insts1+num_insts2-1;
%     id = cbpm_file_raw_data_m(names2{ind});
%     S(i,:) = sum(id'); 
%     [~,I(i,:)] = sort(sum(id'));
%     H(I(1:N)) = H(I(1:N))+ones(1,N);
%     namess{i} = names2{ind};  
%     ind = ind+1;
% end
% 
% catch me
%     rethrow(me)
% end

% GetPlot;plot(H,'.');

try

inds = find(H >= 96);

M = mean(S');

for j = 1:length(inds)
%     ind = 263056; 
    ind = inds(j);
    wid = 20; V = []; 
    
    for i = 1:num_insts1+num_insts2-1;
%         id = cbpm_file_raw_data_m(namess{i}); 
        V(i,:) = sum(S(:,ind-wid:ind+wid)) ./ M(i); 
    end
    
%     ind = 1;
%     for i = num_insts1:num_insts1+num_insts2-1;
%         id = cbpm_file_raw_data_m(names2{i}); V(i,:) = sum(id(ind-wid:ind+wid,:)') ./ M(i); 
%          ind = ind+1;
%     end    

    X = (ones(num_insts1+num_insts2-1,1) * (-wid:wid));
    Y = (ones(2*wid+1,1) * (1:num_insts1+num_insts2-1))';

    GetPlot;surf(X,Y,V); ylabel('BPMindex'); xlabel('turn offset'); view([-90 90])

end

catch me 
    rethrow(me)
end
