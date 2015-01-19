function filename=get_new_filename(prefix,extension);
% gives back the a name of a file that does not exist yet that has a number
% one higher then the highest number that exist with that name
% example: newfile('new','mat') gives back new1.mat
% again:   newfile('new','mat') gives back new2.mat new3.mat etc

if nargin<2 || strcmp(extension,'');
    extension='*';
else
    if isempty(strfind(extension,'.'))
        extension=['.' extension];
    end
end
searchstr=sprintf('allfiles=dir(''%s*%s'');',prefix,extension);
eval(searchstr);

if length(allfiles)==0
    filename=[prefix '1' extension];
    return
end

lastfileinfo=allfiles(end);
lastfile=lastfileinfo.name;
nr1=strfind(lastfile,prefix)+length(prefix);
nr2=strfind(lastfile,extension);
nr=str2num(lastfile(nr1:nr2));

newnumber=nr+1;
filename=sprintf('%s%d%s',prefix,newnumber,extension);
