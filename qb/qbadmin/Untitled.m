T = readtable('emg_values.csv');
hold on
plot(T.Var1);
plot(T.Var2);
plot(T.Var3);
figure;
 plot(T.Var4);
 hold on
 plot(T.Var5);
 plot(T.Var6);
 figure;
 plot(T.Var7);
 hold on
 plot(T.Var8);
 plot(T.Var9);
 figure;
 plot(T.Var10);
 hold on
 plot(T.Var11);
 plot(T.Var12);


figure
plot(T.Var1,T.Var2);
hold on
plot(T.Var1,T.Var3);
plot(T.Var2,T.Var3);
axis ([-50 50 -50 50])
figure
plot(T.Var4,T.Var5);
hold on
plot(T.Var4,T.Var6);
plot(T.Var5,T.Var6);
axis ([-50 50 -50 50])

figure
plot(T.Var7,T.Var8);
hold on
plot(T.Var7,T.Var9);
plot(T.Var8,T.Var9);
axis ([-50 50 -50 50])

figure
plot(T.Var10,T.Var11);
hold on
plot(T.Var10,T.Var12);
plot(T.Var11,T.Var12);
axis ([-50 50 -50 50])
