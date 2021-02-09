function [sigma_values] = plotoptions(filename,numbun,bunchvec,plotvec,...
                                      xpos,ypos,time,ymin,ymax,tmin,...
                                      tmax,turnnum,saveplotvec,powmax,...
                                      fftx,ffty,gauss,tune_expect,...
                                      tune_shift,parabfit,printnum,...
                                      olddir,bpmname,current_bpm)

filetype='xy';

fftx(:,1)=[];
ffty(:,1)=[];

powerfullx=abs(fftx).^2;
powerfully=abs(ffty).^2;

nyquist=1/2;
invperiod=390.12;
[numbunch,numturns]=size(fftx);

weightedtune=zeros(numbun,2);

if mod(numturns,2) == 1

    mirror=(numturns+1)/2;

else

    mirror=numturns/2;

end

powerx=powerfullx(:,1:mirror);
powery=powerfully(:,1:mirror);

frequencyfull=(1:numturns)/numturns*invperiod;
frequency=nyquist*linspace(0,1,mirror);
frequency=(1-frequency)*invperiod;

freqmin=min(frequency);
freqmax=max(frequency);

newdir=strcat(olddir,'/',bpmname);

if exist(newdir,'dir') ~= 7

    disp(['Creating directory ',newdir])
    mkdir(newdir);

end

textfile1=strcat(newdir,'/',filename,'_FFT-',bpmname,'.dat');
textid1=fopen(textfile1,'wt');

fprintf(textid1,'FFT of Horizontal and Vertical Orbits for %s BPM %s\n',filename,bpmname);
fprintf(textid1,'  Freq(kHz)  ');

for n=1:numbun                                                              %repeats for each bunch
    
       fprintf(textid1,'   FFTxb%1.0f          FFTyb%1.0f     ',n,n);       %prints the header in the analyzed data file
       
end

fprintf(textid1,'\n');

for i=1:turnnum-1
    
  fprintf(textid1,'%10.6f',frequencyfull(i));
  
  for n=1:numbun 
      
    fprintf(textid1,'%14.8f  %14.8f',powerfullx(n,i),powerfully(n,i));
    
  end
  
  fprintf(textid1,'\n');
  
end

if plotvec(1)  || saveplotvec(1)                                            %plot raw dat
    
    for F=1:2
        
        if F==1
            
            pos=xpos;
            
        else
            
            pos=ypos;
            
        end
        
        for i=1:numbun    
          
            m=mean(pos(i,1:turnnum));
            s=std(pos(i,1:turnnum));
            
            fig1=figure(F);
            
            if ~plotvec(1) && saveplotvec(1)
                set(fig1,'Visible','off')
            end
            plot(time(1:turnnum)/2.5633,pos(i,1:turnnum),...
                'LineStyle','-',...
                'Marker','o',...
                'MarkerEdgeColor',[1 0 0],...
                'MarkerFaceColor','r',...
                'MarkerSize',3);
            xlabel('Turns');
            ylabel(strcat(filetype(F),{' (m)'}));
            title({strcat(filename,' ',filetype(F),' Bunch ',num2str(bunchvec(i)),'-',bpmname);strcat('Mean=',num2str(m,'%10.4e\n'),'m');strcat('Std Dev=',num2str(s,'%10.4e\n'),'m')})
           %pause(0.5)
           
            if tmin > -1

                xlim([tmin tmax])

            end

            if strcmp(ymin,'none') ~= 1

                ylim([ymin(F) ymax(F)]);

            end

            
            if saveplotvec(1) 

                saveas(fig1, strcat(newdir,'/',filename,'_raw_',filetype(F),'_b_',num2str(bunchvec(i)),'_',bpmname), 'epsc');

            end

        end
    
    end
    
end

if plotvec(2) || saveplotvec(2)                                             %plot full power spectrum
    
    for F=1:2
        
        if F==1
           
            powerfull=powerfullx;
            
        else
            
            powerfull=powerfully;
            
        end
    
        for i=1:numbun
      
            if powmax(F) == 0  
                
                fig1=figure(2+F);
                
                if ~plotvec(2) && saveplotvec(2)
                    set(fig1,'Visible','off')
                else
                    set(fig1,'Visible','on')
                end
              
                powermax=max(powerfull(i,:));
                plot(frequencyfull,powerfull(i,:))
                xlim([min(frequencyfull) max(frequencyfull)])
                ylim([0 1.1*powermax])
                grid on

            else                                                                
                fig1=figure(2+F);
                
                if ~plotvec(2) && saveplotvec(2)
                    set(fig1,'Visible','off')
                else
                    set(fig1,'Visible','on')
                end
                
                plot(frequencyfull,powerfull(i,:))
                xlim([min(frequencyfull) max(frequencyfull)])
                ylim([0 powmax(F)])
                grid on

            end

            xlabel('Frequency (kHz)');                                          
            ylabel('Power');                                                         

            if gauss

                title(strcat(filename,{' '},filetype(F),{' Bunch '},num2str(bunchvec(i)),{' with gaussian window'},{'-'},bpmname));                                         

            else

                title(strcat(filename,{' '},filetype(F),{' Bunch '},num2str(bunchvec(i)),{'-'},bpmname));              
            
            end 

            if saveplotvec(2)

                saveas(fig1,strcat(newdir,'/',filename,'_freqfull_',filetype(F),'_b',num2str(bunchvec(i)),'_',bpmname),'epsc');

            end
            
            textfile1=strcat(newdir,'/',filename,'_FFT','.dat');

        end

    end
    
end

if plotvec(3) || saveplotvec(3)                                             %plot power over nyquist
 
    for F=1:2
        
        if F==1
            
            power=powerx;
            
        else
            
            power=powery;
            
        end
        
        for i=1:numbun

            fig1=figure(4+F);
            
            if ~plotvec(3) && saveplotvec(3)
                set(fig1,'Visible','off')
            else
                set(fig1,'Visible','on')
            end
            
            plot(frequency,power(i,:))
            xlim([freqmin freqmax])

            if powmax(F) > 0

                ylim([0 powmax(F)])

            end


            if gauss

                title(strcat(filename,{' '},filetype(F),{' Bunch '},num2str(bunchvec(i)),' with gaussian window',{'-'},bpmname))

            else

                title(strcat(filename,{' '},filetype(F),{' Bunch '},num2str(bunchvec(i)),{'-'},bpmname))

            end

            xlabel({' Frequency (kHz) '})
            ylabel({' Power '})
            
            if saveplotvec(3)
                
                saveas(fig1,strcat(newdir,'/',filename,'_freqhalf_',filetype(F),'_b',num2str(bunchvec(i)),'_',bpmname),'epsc');

            end
                
        end
    
    end
        
end

if plotvec(4) || saveplotvec(4)                                             %plot 3D
    
    MinCPT=0;
    MaxCPT=0.5;
    powerx3d=0;
    powery3d=0;
    MaxkHz=(1-MinCPT)*invperiod;
    MinkHz=(1-MaxCPT)*invperiod;
     
    for i=1:mirror
        
        for j=1:numbun
                        
            powerx3d(i,j)=abs(fftx(j,i))^2;
            powery3d(i,j)=abs(ffty(j,i))^2;
    
        end
        
    end
            
        Pmatminx=min(min(powerx));
        Pmatmaxx=max(max(powerx));

        Pmatminy=min(min(powery));
        Pmatmaxy=max(max(powery));

        frequency3d=0.5*linspace(0,1,mirror);
        FkHz=(1-frequency3d)*invperiod;

        Bmat=zeros(mirror,numbunch);
        Fmat=zeros(mirror,numbunch);
    
    for F=1:2
        
        if F==1
            
            Pmat=powerx3d;
            Pmatmin=Pmatminx;
            Pmatmax=Pmatmaxx;
            
        else
            
            Pmat=powery3d;
            Pmatmin=Pmatminy;
            Pmatmax=Pmatmaxy;
                   
        end
    
        for i=1:numbun

            Bmat(:,i)=bunchvec(i);

        end
        

        for i=1:numbun

            Fmat(:,i) = FkHz;

        end
        
        fig1=figure(6+F);
        
        if ~plotvec(4) && saveplotvec(4)
            set(fig1,'Visible','off')
        else
            set(fig1,'Visible','on')
        end
        
        surf(Bmat,Fmat,Pmat)
        grid
        xlim([bunchvec(1) bunchvec(numbun)]);
        ylim([MinkHz MaxkHz]);
        zlim([Pmatmin Pmatmax]);

        xlabel('Bunch');
        ylabel('Frequency (kHz)');
        zlabel('Power');
        
        title(strcat(filename,{' '},filetype(F),{' '},bpmname));
        
        fig2=figure(8+F);
        
        if ~plotvec(4) && saveplotvec(4)
            set(fig2,'Visible','off')
        else
            set(fig2,'Visible','on')
        end
        
        
        plot3(Bmat,Fmat,Pmat)
        xlim([bunchvec(1) bunchvec(end)]);
        ylim([MinkHz MaxkHz]);
        zlim([Pmatmin Pmatmax]);

        xlabel('Bunch');
        ylabel('Frequency (kHz)');
        zlabel('Power');
        
        title(strcat(filename,{' '},filetype(F),{' '},bpmname));
        
        if saveplotvec(4)

            saveas(fig2,strcat(newdir,'/',filename,'_plot3D_',filetype(F),'_',bpmname),'epsc');
            saveas(fig1,strcat(newdir,'/',filename,'_3D_',filetype(F),'_',bpmname),'epsc');

        end
    
     end
 
end

if plotvec(5) || saveplotvec(5)                                             %plot near tune
 
    for F=1:2
        
        if F==1
            
            
            power=powerx;
            
        else
            
            power=powery;
            
        end
        
        for i=1:numbun
            
            spectrumpts=turnnum/2;                                          %Calculate the weighted tune that exists in the window chosen
            num=0;                                                          %if ((>= 0.90) && (grade_average >= 60))
            den=0;

            for j=1:spectrumpts                                             %Calculate the weighted tune that exists in the window chosen

                if ((frequency(j)>= tune_shift(F,1)) && (frequency(j) <= tune_shift(F,2)))

                    num=num+frequency(j)*power(i,j);
                    den=den+power(i,j);

                end

            end

            weightedtune(i,F)=num/den;
            
            if tune_expect(F) == 0
           
                [maxpower,index]=max(power(i,2:end-1));
                
            
            else
              
                A=find(frequency(:) > tune_shift(F,1));
                A=find(frequency(A) < tune_shift(F,2));
          
                [maxpower,index]=max(power(i,A(1):A(end)));
                
                index=index+A(1)-2;
                                
                          
            end
     
            powervec=[power(i,index) power(i,index+1) power(i,index+2)];
            freqvec=[frequency(index) frequency(index+1) frequency(index+2)];
            logpowervec=log(powervec);
            [parab,error]=polyfit_alter(freqvec,logpowervec,2);
            xparab=linspace(0.999999*freqmin, 1.000001*freqmax,5000);
            yparab=polyval(parab,xparab);
            ygauss=exp(yparab);
            
            fig1=figure(10+F);
            
            if ~plotvec(5) && saveplotvec(5)
                set(fig1,'Visible','off')
            else
                set(fig1,'Visible','on')
            end
            
            if parabfit
            
                plot(frequency,power(i,:),'bo-',xparab,ygauss,'r--')
            
            else
                plot(frequency,power(i,:),'bo-')
                
            end                                                                                                                                                             
            
            if tune_expect(F) == 0
            
                xlim([freqmin freqmax]);
            
            else
                
                xlim([tune_shift(F,1) tune_shift(F,2)]);
                
            end
            
            if powmax(F) > 0

                ylim([0 powmax(F)])

            end
            

            if gauss

                title(strcat(filename,{' '},filetype(F),{' Bunch '},num2str(bunchvec(i)),' with gaussian window',{'-'},bpmname))

            elseif sum(sum(tune_expect)) > 0

                title({strcat(filename,' ',filetype(F),' Bunch ',num2str(bunchvec(i)),'-',bpmname);strcat('Weighted tune=',num2str(weightedtune(i,F)),'kHz')})
                
            else
                
                title(strcat(filename,{' '},filetype(F),{' Bunch '},num2str(bunchvec(i)),{'-'},bpmname))

            end

            xlabel({' Frequency (kHz) '})
            ylabel({' Power '})
            
            if saveplotvec(5)
                
                saveas(fig1,strcat(newdir,'/',filename,'_neartune_',filetype(F),'_b',num2str(bunchvec(i)),'_',bpmname),'epsc');

            end
                
        end
        
    end
        
end

if plotvec(6) || saveplotvec(6)                                                               %plot tune vs bunch
     
    tune=zeros(numbun,3);
    tune(:,1)=bunchvec;
    
    for F=1:2
        
        if F==1
            
            
            power=powerx;
            
        else
            
            power=powery;
            
        end
    
        
        for i=1:numbun
            
            if plotvec(5) == 0
            
                spectrumpts=turnnum/2;                                      %Calculate the weighted tune that exists in the window chosen
                num=0;                                                      %if ((>= 0.90) && (grade_average >= 60))
                den=0;

                for j=1:spectrumpts                                         %Calculate the weighted tune that exists in the window chosen

                    if ((frequency(j) >= tune_shift(F,1)) && (frequency(j) <= tune_shift(F,2)))

                        num=num+frequency(j)*power(i,j);
                        den=den+power(i,j);

                    end

                end

                weightedtune(i,F)=num/den;
                
            end
            
            [maxpower,value]=max(power(i,2:end-1));

            powervec=[log(power(i,value)) log(power(i,value+1)) log(power(i,value+2))];
            freqvec=[frequency(value) frequency(value+1) frequency(value+2)];
            
            [parab error]=polyfit_alter(freqvec,powervec,2);
            
            tune(i,F+1)=-parab(2)/(2*parab(1));
            
        end
        
        tune_minpeak=min(tune(:,F+1))*0.99999;
        tune_maxpeak=max(tune(:,F+1))*1.000001;

        if sum(sum(tune_expect)) >0 
            
            tune_minweight=min(weightedtune(:,F))*0.99999;
            tune_maxweight=max(weightedtune(:,F))*1.000001;
            
            if (tune_minpeak >= tune_minweight)

                tune_min=tune_minweight;

            else

                tune_min=tune_minpeak;

            end

            if (tune_maxpeak >= tune_maxweight)

                tune_max=tune_maxpeak;

            else

                tune_max=tune_maxweight;

            end
        
            fig1=figure(12+F);
            
            if ~plotvec(6) && saveplotvec(6)
                set(fig1,'Visible','off')
            else
                set(fig1,'Visible','on')
            end
            
            plot(tune(:,1),tune(:,F+1),'b.',tune(:,1),weightedtune(:,F),'r.','Markersize',20) 
            xlabel('Bunch')
            ylabel(strcat({' \nu '},filetype(F), {' (kHz)' }))
            title(strcat(filename,{' '},{'Tune vs. Bunch: '},filetype(F)))
            h=legend('Tune Peak','Weighted Tune',2);
            set(h);
            xlim([bunchvec(1)-1 bunchvec(end)+1])
            ylim([tune_min tune_max])
            set(gca,'XGrid','on')
            set(gca,'YGrid','on')
            
        else
            
            fig1=figure(12+F);
          
            if ~plotvec(6) && saveplotvec(6)
                set(fig1,'Visible','off')
            else
                set(fig1,'Visible','on')
            end
            
            plot(tune(:,1),tune(:,F+1),'b.','Markersize',20)
            xlabel('Bunch')
            ylabel(strcat({' \nu '},filetype(F), {' (kHz)' }))
            title(strcat(filename,{' '},{'Tune vs. Bunch: '},filetype(F)))
            h=legend('Tune Peak',2);
            set(h);
            xlim([bunchvec(1)-1 bunchvec(end)+1])
            ylim([tune_minpeak tune_maxpeak])
            set(gca,'XGrid','on')
            set(gca,'YGrid','on')
            
        end
            
        
        if saveplotvec(6)

            saveas(fig1,strcat(newdir,'/',filename,'_tune_',filetype(F),'_',bpmname),'epsc');

        end
                    
    end
 
end

if plotvec(5) == 0 && plotvec(6) == 0   
    
    for F=1:2
        
        if F==1           
            
            power=powerx;
            
        else
            
            power=powery;
            
        end
    
        for i=1:numbun

            spectrumpts=turnnum/2;                                          %Calculate the weighted tune that exists in the window chosen
            num=0;                                                          %if ((>= 0.90) && (grade_average >= 60))
            den=0;

            for j=1:spectrumpts                                             %Calculate the weighted tune that exists in the window chosen

                if ((frequency(j)>= tune_shift(F,1)) && (frequency(j) <= tune_shift(F,2)))

                    num=num+frequency(j)*power(i,j);
                    den=den+power(i,j);

                end

            end
            
            weightedtune(i,F)=num/den;
            
        end
        
    end
    
end

if plotvec(7) || saveplotvec(7) 
    
    vx=zeros(numbun,turnnum-2);
    phix=zeros(numbun,turnnum-2);
    vy=zeros(numbun,turnnum-2);
    phiy=zeros(numbun,turnnum-2); 
    
    for j=1:numbun

        for i=1:turnnum-2

            vx(j,i)=1/(2*pi)*acos((xpos(j,i)+xpos(j,i+2))/2*xpos(j,i+1));
            phix(j,i)=atan(cot(2*pi*vx(j,i))-(xpos(j,i+1)/xpos(j,i))*csc(2*pi*vx(j,i)));

            vy(j,i)=1/(2*pi)*acos((ypos(j,i)+ypos(j,i+2))/2*ypos(j,i+1));
            phiy(j,i)=atan(cot(2*pi*vy(j,i))-(ypos(j,i+1)/ypos(j,i))*csc(2*pi*vy(j,i)));

        end

    end

    for F=1:2

        for i=1:numbun

            if F==1

                nu=vx;
                phi=phix;

            else

                phi=phiy;
                nu=vy;

            end

            fig1=figure(14+F);
            
            plot(time(2:turnnum-1),nu(i,:))
            
            if ~plotvec(7) && saveplotvec(7)
                set(fig1,'Visible','off')
            else
                set(fig1,'Visible','on')
            end
            
            title({strcat(filename,' ',filetype(F));strcat(' Bunch ','{ }',num2str(bunchvec(i)),'-',bpmname)});
            xlabel('Time (\mus)','FontSize',16);
            ylabel(strcat('\nu_',filetype(F)),'FontSize',16);                

            if tmin > -1

                xlim([tmin tmax])

            end
            
            if saveplotvec(7) == 1
                
                saveas(fig1,strcat(newdir,'/',filename,'_phase_',filetype(F),'_',bpmname),'epsc');
                
            end
            
        end
        
        for i=1:numbun

            fig1=figure(16+F); 
            
            if ~plotvec(7) && saveplotvec(7)
                set(fig1,'Visible','off')
            else
                set(fig1,'Visible','on')
            end
            
            plot(time(2:turnnum-1),phi(i,:))
            title({strcat(filename,' ',filetype(F));strcat(' Bunch ','{ }',num2str(bunchvec(i)),'-',bpmname)});
            xlabel('Time (\mus)','FontSize',16);
            ylabel(strcat('\phi_',filetype(F)),'FontSize',16);

            if tmin > -1

                xlim([tmin tmax])

            end
            
            if saveplotvec(7) == 1
                
                saveas(fig1,strcat(newdir,'/',filename,'_phase_shift_',filetype(F),'_',bpmname),'epsc');
                
            end
            
        end
            
        for i=1:numbun
            
            if F==2
                format long
                
                xticks=min(vx(i,:)):(max(vx(i,:))-min(vx(i,:)))/3:max(vx(i,:));
                yticks=min(vy(i,:)):(max(vy(i,:))-min(vy(i,:)))/3:max(vy(i,:));
                
                fig1=figure(15);
                
                if ~plotvec(7) && saveplotvec(7)
                    set(fig1,'Visible','off')
                else
                    set(fig1,'Visible','on')
                end
                
                plot(vx(i,:),vy(i,:),'b.','Markersize',20)
                title({strcat(filename);strcat(' Bunch ','{ }',num2str(bunchvec(i)),'-',bpmname)});           
                xlabel(strcat('\nu_',filetype(F-1)),'FontSize',16);
                ylabel(strcat('\nu_',filetype(F)),'FontSize',16);
                set(gca,'XLim',[min(vx(i,:)) max(vx(i,:))])
                set(gca,'XTick',xticks)
                set(gca,'YLim',[min(vy(i,:)) max(vy(i,:))])
                set(gca,'YTick',yticks)
                pause(0.5)
                
            end
            
        end

    end

end
    
if printnum                                                                %output file
  [sigma_values] = output(printnum,powerx,powery,frequency,filename,...
                   numbun,bunchvec,freqmin,freqmax,weightedtune,...
                   bpmname,olddir,tune_shift,current_bpm,tune_shift,tune_expect);
else
    sigma_values=0;
end
