% procedure for 'aim-mat'
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:45 $
% $Revision: 1.3 $

function handles=aim_updatecheckboxes(hObject, eventdata, handles,nr);


% curval=get(hObject,'Value');
% handles.info.calculate(nr)=curval;

loadstatus(1)=handles.info.pcp_loaded;
loadstatus(2)=handles.info.bmm_loaded;
loadstatus(3)=handles.info.nap_loaded;
loadstatus(4)=handles.info.strobes_loaded;
loadstatus(5)=handles.info.sai_loaded;
loadstatus(6)=handles.info.usermodule_loaded;
loadstatus(7)=handles.info.movie_loaded;

 
curval=get(hObject,'Value');
if curval==1
    for num=nr:-1:1
        if loadstatus(num)==0
%             handles.info.calculate(num)=curval;
            hand=getcheckboxhandle(handles,num);
            set(hand,'Value',curval);
        else
            break
        end
    end
else
    for num=nr:7
%         handles.info.calculate(num)=curval;
        hand=getcheckboxhandle(handles,num);
        set(hand,'Value',curval);
    end
end



function hand=getcheckboxhandle(handles,nr)
switch nr
    case 1
        hand=handles.checkbox0;
    case 2
        hand=handles.checkbox1;
    case 3
        hand=handles.checkbox2;
    case 4
        hand=handles.checkbox3;
    case 5
        hand=handles.checkbox4;
    case 6
        hand=handles.checkbox8;
    case 7
        hand=handles.checkbox5;
end
