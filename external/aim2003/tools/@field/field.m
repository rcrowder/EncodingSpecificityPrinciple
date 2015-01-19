function res=field(nrt,nrf,sr)
%Konstruktor der field klasse
% Fields sind 2D-Array einer Frequenz über Zeit
% also ein umgedrehtes Array
% Bei mir ist die erste Koordinate X und die zweite Y!!
% das ist verständlicher! So, wie auf dem Schirm zu sehen: 
% disp(irgend ein Array) macht nach rechts die Zeit 
% und nach oben die Frequenz

if nargin<3
    sr=1;
end


if nargin==1
    if isnumeric(nrt);  % Aufruf mit einem fertigen Feld
%         x=size(nrt,2);
%         y=size(nrt,1);
        res.werte=nrt;
    end
else    %wenn nicht mit einem aufgerufen, dann müssen die beiden Werte Begrenzer sein
    x=nrt;
    y=nrf;
    % blödes Matlab: Matrizen sind kodiert oben,rechts
    % Der X-Wert geht nach oben, der Y-Wert geht nach rechts.
    % Ich will Zeit nach rechts gehen lassen, also muss Zeit der zweite Parameter sein
    res.werte=zeros(y,x);
end

% res.numbert=x;
% res.numberf=y;
res.samplerate=sr;
% res.minfre=0;
res.maxfre=0;
res.offset=0;


res=class(res,'field');



