function DrawSMap()       --�泡����ͼ
--DrawSSSSMap()
	lib.DrawSMap(JY.SubSceneX, JY.SubSceneY, JY.Base["��X1"], JY.Base["��Y1"], JY.SubScene, JY.MyPic)

end

--��������������
function Game_SMap()         --��������������

    DrawSMap();
    DrawString(5,CC.ScreenH-15,string.format("%s %d %d",JY.Scene[JY.SubScene]["����"],JY.Base["��X1"],JY.Base["��Y1"]) ,C_GOLD,16);
    ShowScreen();

    local d_pass=GetS(JY.SubScene,JY.Base["��X1"],JY.Base["��Y1"],3);   --��ǰ·���¼�
    if d_pass>=0 then
        if d_pass ~=JY.OldDPass then     --�����ظ�����
            EventExecute(d_pass,3);       --·�������¼�
            JY.OldDPass=d_pass;
        end
    else
        JY.OldDPass=-1;
    end

    local isout=0;                --�Ƿ���������
    if (JY.Scene[JY.SubScene]["����X1"] ==JY.Base["��X1"] and JY.Scene[JY.SubScene]["����Y1"] ==JY.Base["��Y1"]) or
       (JY.Scene[JY.SubScene]["����X2"] ==JY.Base["��X1"] and JY.Scene[JY.SubScene]["����Y2"] ==JY.Base["��Y1"]) or
       (JY.Scene[JY.SubScene]["����X3"] ==JY.Base["��X1"] and JY.Scene[JY.SubScene]["����Y3"] ==JY.Base["��Y1"]) then
       isout=1;
    end

    if isout==1 then    --��ȥ����������ͼ
        JY.Status=GAME_MMAP;
        lib.ShowSlow(50,1)
        lib.PicInit();
 	    lib.PicLoadFile(CC.MMAPPicFile,0);
 	    lib.PicLoadFile(CC.HeadPicFile,1);
	    lib.PicLoadFile(CC.ThingPicFile,2);

        if JY.MMAPMusic>=0 then
            PlayMIDI(JY.MMAPMusic);
        else
            PlayMIDI(JY.Scene[JY.SubScene]["��������"]);
        end

        JY.SubScene=-1;

        lib.DrawMMap(JY.Base["��X"],JY.Base["��Y"],GetMyPic());
        lib.ShowSlow(50,0)
        lib.WaitKey();
        return;
    end

    --�Ƿ���ת����������
    if JY.Scene[JY.SubScene]["��ת����"] >=0 then
        if JY.Base["��X1"]==JY.Scene[JY.SubScene]["��ת��X1"] and JY.Base["��Y1"]==JY.Scene[JY.SubScene]["��ת��Y1"] then
            JY.SubScene=JY.Scene[JY.SubScene]["��ת����"];
            lib.ShowSlow(50,1);
            PlayMIDI(JY.Scene[JY.SubScene]["��������"]);
            if JY.Scene[JY.SubScene]["�⾰���X1"]==0 and JY.Scene[JY.SubScene]["�⾰���Y1"]==0 then
                JY.Base["��X1"]=JY.Scene[JY.SubScene]["���X"];            --�³������⾰���Ϊ0����ʾ����һ���ڲ�����
                JY.Base["��Y1"]=JY.Scene[JY.SubScene]["���Y"];
            else
                JY.Base["��X1"]=JY.Scene[JY.SubScene]["��ת��X2"];         --�ⲿ���������������ڲ��������ء�
                JY.Base["��Y1"]=JY.Scene[JY.SubScene]["��ת��Y2"];
            end
            JY.SubSceneX=0;
            JY.SubSceneY=0;

            DrawSMap();
            lib.ShowSlow(50,0);
            lib.WaitKey();
            return;
        end
    end

    local x,y;
    local keypress = lib.GetKey();
    local direct=-1;
    if keypress ~= -1 then
		if keypress==VK_ESCAPE then
			MMenu();
		elseif keypress==VK_UP then
			direct=0;
		elseif keypress==VK_DOWN then
			direct=3;
		elseif keypress==VK_LEFT then
			direct=2;
		elseif keypress==VK_RIGHT then
			direct=1;
		end
		keypress = lib.WaitKey()
		if keypress==VK_SPACE or keypress==VK_RETURN  then
            if JY.Base["�˷���"]>=0 then        --��ǰ������һ��λ��
                local d_num=GetS(JY.SubScene,JY.Base["��X1"]+CC.DirectX[JY.Base["�˷���"]+1],JY.Base["��Y1"]+CC.DirectY[JY.Base["�˷���"]+1],3);
                if d_num>=0 then
                    EventExecute(d_num,1);       --�ո񴥷��¼�
                end
            end
		end
    end

    if JY.Status~=GAME_SMAP then
        return ;
    end

    if direct ~= -1 then
        AddMyCurrentPic();
        x=JY.Base["��X1"]+CC.DirectX[direct+1];
        y=JY.Base["��Y1"]+CC.DirectY[direct+1];
        JY.Base["�˷���"]=direct;
    else
        x=JY.Base["��X1"];
        y=JY.Base["��Y1"];
    end

    JY.MyPic=GetMyPic();
    DtoSMap();
    if SceneCanPass(x,y)==true then          --�µ���������߹�ȥ
        JY.Base["��X1"]=x;
        JY.Base["��Y1"]=y;
    end

    JY.Base["��X1"]=limitX(JY.Base["��X1"],1,CC.SWidth-2);
    JY.Base["��Y1"]=limitX(JY.Base["��Y1"],1,CC.SHeight-2);




end

--��������(x,y)�Ƿ����ͨ��
--����true,���ԣ�false����
function SceneCanPass(x,y)  --��������(x,y)�Ƿ����ͨ��
    local ispass=true;        --�Ƿ����ͨ��

    if GetS(JY.SubScene,x,y,1)>0 then     --������1����Ʒ������ͨ��
        ispass=false;
    end

    local d_data=GetS(JY.SubScene,x,y,3);     --�¼���4
    if d_data>=0 then
        if GetD(JY.SubScene,d_data,0)==1 then  --d*����Ϊ����ͨ��
            ispass=false;
        end
    end

    if CC.SceneWater[GetS(JY.SubScene,x,y,0)] ~= nil then   --ˮ�棬���ɽ���
        ispass=false;
    end
    return ispass;
end

function DtoSMap()
	lib.DtoSMap(JY.SubScene, JY.Mytick)
end 

GetS = lib.GetS
SetS = lib.SetS
GetD = lib.GetD
SetD = lib.SetD