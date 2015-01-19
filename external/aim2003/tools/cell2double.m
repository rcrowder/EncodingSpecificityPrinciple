function doubles=cell2double(cell,name)
% usage: doubles=cell2double(cell,name)
% returns a array of doubles, if the cell is a string of numbers
% the cell must have the filds name

nr=length(cell);
doubles=zeros(nr,1);

for i=1:nr
    eval(sprintf('doubles(i)=cell{i}.%s;',name));
end