clear variables
close all
clc
sim('matlab_simulation.slx');
vysl = zeros(length(simout.time),3);
vysl(:,1)=simout.time;
vysl(:,2:3)=simout.signals.values;
vysl1=[];
j=1;
for i=1:length(simout.time)
    if((vysl(i,1)>0.5)&(vysl(i,1)<0.65))
        vysl1(j,:)=vysl(i,:);
        j=j+1;
    end
end
subplot(3,1,1);
plot(vysl(:,1),vysl(:,3));
grid on;
xlabel('t[s]');
ylabel('U[V]');
xlim([0.5 0.65]);
x=linspace(0.5,0.65,16);
set(gca,'xtick',x);
title('Impulz generovaný arduinem pro sepnutí solenoidu');

subplot(3,1,2);
plot(vysl(:,1),vysl(:,2));
grid on;
xlabel('t[s]');
ylabel('I[A]');
xlim([0.5 0.65]);
x=linspace(0.5,0.65,16);
set(gca,'xtick',x);
title('Zmìna proudu kolektorem/emitorem (Ic~Ie) v èase v závislosti na napìovém impulzu generovaném arduinem');

B=88;     %koeficient beta tranzistoru BD139 odeètený z grafických závislostí
Ib = vysl1(:,2)./B;

subplot(3,1,3);
plot(vysl1(:,1),Ib(:));
grid on;
xlabel('t[s]');
ylabel('I[A]');
xlim([0.5 0.65]);
x=linspace(0.5,0.65,16);
set(gca,'xtick',x);
title('Odhadovaný odebíraný proud z arduina (proud do báze tranzistoru) po pøivedení napìového impulzu z arduina');
