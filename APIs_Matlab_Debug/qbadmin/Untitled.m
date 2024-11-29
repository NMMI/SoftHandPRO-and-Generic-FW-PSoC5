T = readtable('emg_values.csv');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% IMU 0

figure('Name','Imu 0');
hold on
plot(T.Var1);
plot(T.Var2);
plot(T.Var3);

figure('Name','Imu 0');
plot(T.Var1,T.Var2);
hold on
plot(T.Var1,T.Var3);
plot(T.Var2,T.Var3);
axis equal

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% IMU 1

figure('Name','Imu 1');
plot(T.Var4);
hold on
plot(T.Var5);
plot(T.Var6);

figure('Name','Imu 1');
plot(T.Var4,T.Var5);
hold on
plot(T.Var4,T.Var6);
plot(T.Var5,T.Var6);
axis equal

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% IMU 2

figure('Name','Imu 2');
plot(T.Var7);
hold on
plot(T.Var8);
plot(T.Var9);

figure('Name','Imu 2');
plot(T.Var7,T.Var8);
hold on
plot(T.Var7,T.Var9);
plot(T.Var8,T.Var9);
axis equal

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% IMU 3

figure('Name','Imu 3');
plot(T.Var10);
hold on
plot(T.Var11);
plot(T.Var12);

figure('Name','Imu 3');
plot(T.Var10,T.Var11);
hold on
plot(T.Var10,T.Var12);
plot(T.Var11,T.Var12);
axis equal

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% IMU 4

figure('Name','Imu 4');
plot(T.Var13);
hold on
plot(T.Var14);
plot(T.Var15);
figure('Name','Imu 4');

plot(T.Var13,T.Var14);
hold on
plot(T.Var13,T.Var15);
plot(T.Var14,T.Var15);
axis equal
