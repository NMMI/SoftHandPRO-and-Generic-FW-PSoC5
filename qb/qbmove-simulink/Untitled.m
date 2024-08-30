x =(simout.Data(1,2,:))
x =  x(:)
% x= x-(max(x)+min(x))/2

y =(simout.Data(2,2,:))
y =  y(:)
%y= y-(max(y)+min(y))/2


z =(simout.Data(3,2,:))
z =  z(:)
%z= z-(max(z)+min(z))/2
 u=[x,y,z];

plot(x,y)
hold on
plot(x,z)
hold on
plot(y,z)
axis equal
xlim([-20000 20000])
ylim([-20000 20000])
[A,b,expMFS]  = magcal(u);
uCorrected = (u-b)*A;

figure ;
plot(u(:,1),u(:,2))
hold on

plot(u(:,1),u(:,3))
hold on

plot(u(:,2),u(:,3))
axis equal
xlim([-20000 20000])
ylim([-20000 20000])