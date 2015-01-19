function disp(f)
nrt=getnrpoints(f);
nrf=getnrfrequencies(f);

disp(sprintf('field: (time:)%d - by - (frequencies:)%d',nrt,nrf));
getdata(f)
return
for j=1:nrt
    row=getcolumn(f,j);
    a=getdata(row)
end