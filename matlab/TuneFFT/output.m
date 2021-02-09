function [sigma_values] = output(printnum,powerx,powery,frequency,filename,...
                  numbun,bunchvec,freqmin,freqmax,weightedtune,bpm,olddir,...
                  tuneshift,current_bpm,tune_shift,tune_expect)
sigma_values = 0;
interval_size = 20;
tune=zeros(numbun,printnum*2+1);
tune(:,1)=bunchvec;
index=zeros(numbun,printnum);
powervec=zeros(printnum,3);
freqvec=zeros(printnum,3);
parab=zeros(printnum,3);
for F=1:2 
  if F==1
    power=powerx;
  else
    power=powery;
  end
  for j=1:numbun
    
    if tune_expect(F) == 0
    powsort=sort(power(j,2:end-1));
    powsort=powsort(end-printnum+1:end);
    else
    A=find(frequency(:) > tune_shift(F,1));
    A=find(frequency(A) < tune_shift(F,2));
          
    powsort=sort(power(j,A(1):A(end)));
    powsort=powsort(end-printnum+1:end);
    
    end
    
    for i=1:printnum
      if ~isnan(powsort(i))
        index(j,i)=find(powsort(i)==power(j,2:end-1));
        powervec(i,:)=[log(power(j,index(j,i))) log(power(j,index(j,i)+1)) log(power(j,index(j,i)+2))];
        freqvec(i,:)=[frequency(index(j,i)) frequency(index(j,i)+1) frequency(index(j,i)+2)];
        [parab(i,:), val]=polyfit_alter(freqvec(i,:),powervec(i,:),2); %#ok<NASGU>
        tune(j,2*i+F-1)=-parab(i,2)/(2*parab(i,1));
      end
    end
  end
end

x = regexp(bpm,'[A-Z.a-z,0-9]');
str = '';
for i=x
  str = strcat(str,bpm(i));
end
bpm = str;

if printnum == 1 
  if sum(sum(weightedtune)) > 0
    textfile=strcat(olddir,'/',filename,'_',bpm,'_tune1.dat'); 
    textid=fopen(textfile,'wt');
    fprintf(textid,'Most likely frequency/period found for each bunch in %s.\n',filename);
    fprintf(textid,'Given limits: %5.4f to %5.4f kHz \n\n',freqmin,freqmax);
    fprintf(textid,' Bunch   Qx1(kHz)-BPM%s      Qy1(kHz)-BPM%s       Weighted_Qx(kHz)     Weighted_Qy(kHz)\n',bpm,bpm);
    for i=1:numbun
      fprintf(textid,'%5d %16.3f %18.3f %20.3f %20.3f \n',tune(i,1),tune(i,2),tune(i,3),weightedtune(i,1),weightedtune(i,2));
    end
    fclose(textid);
  else        
    textfile=strcat(olddir,'/',filename,'_',bpm,'_tune1.dat'); 
    textid=fopen(textfile,'wt');
    fprintf(textid,'Most likely frequency/period found for each bunch in %s.\n',filename);
    fprintf(textid,'Given limits: %5.4f to %5.4f kHz \n\n',freqmin, freqmax);
    fprintf(textid,' Bunch   Qx1(kHz)-BPM%s      Qy1(kHz)-BPM%s \n',bpm,bpm);
    for i=1:numbun
      fprintf(textid,'%3d %16.3f %19.3f \n',tune(i,1),tune(i,2),tune(i,3));
    end
    fclose(textid);
  end
elseif printnum == 2
  if sum(sum(weightedtune)) > 0
    textfile=strcat(olddir,'/',filename,'_',bpm,'_tune2.dat'); 
    textid=fopen(textfile,'wt');
    fprintf(textid,'%d most likely frequencies/periods found for each bunch in %s.\n',printnum,filename);
    fprintf(textid,'Given limits: %5.4f to %5.4f kHz \n\n',freqmin, freqmax);
    fprintf(textid,' Bunch   Qx1(kHz)-BPM%s     Qy1(kHz)-BPM%s        Qx2(kHz)-BPM%s     Qy2(kHz)-BPM%s     Weighted_Qx(kHz)   Weighted_Qy(kHz)\n',bpm,bpm,bpm,bpm);
    for i=1:numbun
      fprintf(textid,'%3d %16.3f %19.3f %22.3f %19.3f %18.3f %18.3f\n',tune(i,1),tune(i,4),tune(i,5),tune(i,2),tune(i,3),weightedtune(i,1),weightedtune(i,2));
    end
    fclose(textid);
  else
    textfile=strcat(olddir,'/',filename,'_',bpm,'_tune2.dat'); 
    textid=fopen(textfile,'wt');
    fprintf(textid,'%d most likely frequencies/periods found for each bunch in %s.\n',printnum,filename);
    fprintf(textid,'Given limits: %5.4f to %5.4f kHz \n\n',freqmin, freqmax);
    fprintf(textid,' Bunch   Qx1(kHz)-BPM%s     Qy1(kHz)-BPM%s        Qx2(kHz)-BPM%s     Qy2(kHz)-BPM%s \n',bpm,bpm,bpm,bpm);
    for i=1:numbun
      fprintf(textid,'%3d %16.3f %19.3f %22.3f %19.3f \n',tune(i,1),tune(i,4),tune(i,5),tune(i,2),tune(i,3));
    end
    fclose(textid);   
  end
elseif printnum == 3
  if sum(sum(weightedtune)) > 0
    textfile=strcat(olddir,'/',filename,'_',bpm,'_tune3.dat'); 
    textid=fopen(textfile,'wt');
    fprintf(textid,'%d most likely frequencies/periods found for each bunch in %s.\n',printnum,filename);
    fprintf(textid,'Given limits: %5.4f to %5.4f kHz \n\n',freqmin, freqmax);
    fprintf(textid,' Bunch   Qx1(kHz)-BPM%s     Qy1(kHz)-BPM%s      Qx2(kHz)-BPM%s    Qy2(kHz)-BPM%s     Qx3(kHz)-BPM%s    Qy3(kHz)-BPM%s    Weighted_Qx(kHz)    Weighted_Qy(kHz)\n',bpm,bpm,bpm,bpm,bpm,bpm);
    for i=1:numbun
      fprintf(textid,'%3d %16.3f %19.3f %19.3f %18.3f %18.3f %18.3f %18.3f %18.3f\n',tune(i,1),tune(i,6),tune(i,7),tune(i,4),tune(i,5),tune(i,2),tune(i,3),weightedtune(i,1),weightedtune(i,2));
    end
    fclose(textid);    
  else
    textfile=strcat(olddir,'/',filename,'_',bpm,'_tune3.dat'); 
    textid=fopen(textfile,'wt');
    fprintf(textid,'%d most likely frequencies/periods found for each bunch in %s.\n',printnum,filename);
    fprintf(textid,'Given limits: %5.4f to %5.4f kHz \n\n',freqmin, freqmax);
    fprintf(textid,' Bunch   Qx1(kHz)-BPM%s     Qy1(kHz)-BPM%s      Qx2(kHz)-BPM%s    Qy2(kHz)-BPM%s     Qx3(kHz)-BPM%s    Qy3(kHz)-BPM%s \n',bpm,bpm,bpm,bpm,bpm,bpm);
    for i=1:numbun
      fprintf(textid,'%3d %16.3f %19.3f %19.3f %18.3f %18.3f %18.3f \n',tune(i,1),tune(i,6),tune(i,7),tune(i,4),tune(i,5),tune(i,2),tune(i,3));
    end
    fclose(textid);    
  end
elseif printnum == 4 || printnum == 5
  sigma_values = nonparametric(powerx,powery,frequency,tuneshift,interval_size,current_bpm,printnum);
  textfile=strcat(olddir,'/',filename,'_',bpm,'_tune4.dat');
  [textid,msg]=fopen(textfile,'wt');
  if ~isempty(msg)
    disp(msg)
  end
  fprintf(textid,'Most likely frequency/period found for each bunch in %s.\n',filename);
  fprintf(textid,'Given limits: %5.4f to %5.4f kHz \n\n',freqmin, freqmax);
  fprintf(textid,' Bunch   Qx(kHz)-BPM%s    Qx(sigma)-BPM%s   Qx(kHz)-BPM%s    Qy(sigma)-BPM%s\n',bpm,bpm,bpm,bpm);
  for i=1:numbun
    fprintf(textid,'%3d %18.3f %18.3f %18.3f %18.3f\n',i,sigma_values(i,1),sigma_values(i,2),sigma_values(i,3),sigma_values(i,4));
  end
  fclose(textid);
end