% Init RS485 Device
s = initSerialDevice('COM13');
h=1;
ID = 1;
x=zeros(1500,2);
% Ping
%disp(char(pingBoard(s, ID)));

% Activate Device
%activateDevice(s, ID, true);

% Read encoders
%encoders = getEncoders(s, ID);
tic
for l=1:1498
x(l,1:2)=getEMG(s, ID);
end
toc
% Set Motor inputs (ticks)
%setMotorInputs(s, ID, 10000, -2000);
a=x;
% Deactivate Device
%activateDevice(s, ID, false);

% Close RS485 Device
clear s;
clear x;
