clear all;
stringa = fileread('capture.txt');
k= strfind(stringa,',');
for(i=2:(length(k)-1))
    m(i)=k(i)-k(i-1);
    end

k= strfind(stringa,'4141');
stringa=split(stringa,'62413838');

for(i=2:(length(stringa)-1))
    stringa{i}=append (stringa{i}(3:4),stringa{i}(1:2));
 out(i) = typecast(uint16(base2dec(stringa(i), 16)), 'int16');
end
figure
plot(out)
grid on
stringa{1}(1:4)

emgvalues = readtable('emg_values.csv');
figure
plot (emgvalues.Var1)
hold on
plot (emgvalues.Var2)
plot (emgvalues.Var3)




figure
plot (emgvalues.Var4)
hold on
plot (emgvalues.Var5)
plot (emgvalues.Var6)


figure
plot (emgvalues.Var7)
hold on
plot (emgvalues.Var8)
plot (emgvalues.Var9)

