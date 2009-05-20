


---��ģ���Ŷ�JYMain.lua ���޸ĺ����䡣

--������������ģ�����������޸�ԭʼJYMain.Lua�ļ���
--����һ��������¼�������
--1. SetModify������   �ú�������Ϸ��ʼʱ���ã������ڴ��޸�ԭ�е����ݣ��Լ��ض���ԭ�еĺ�������ʵ�ֶ�ԭ�к������޸ġ�
--                    �����Ϳ��Ի�������ԭʼ�ĺ���
--2. ԭ�к��������غ����� SetModify�����صĺ������ڴ˴����������޸�JYMain.lua�ļ����������޸Ĳ����ض��庯������ʽ��
--3. �µ���Ʒʹ�ú�����
--4. �µĳ����¼�������





--��jymain���޸ģ��Լ������µ���Ʒ�����ͳ����¼�������
--ע��������Զ���ȫ�̱�����
function SetModify()

   --����һ�����庯�������ӡ����������޸����˵��е�ϵͳ�˵�����������Ϸ�����п�����Ч�Ĺ��ܡ�
   --ԭ��ֻ����jyconst.lua��ͨ������������ǰ���ƣ���������ʵʱ���ơ�
   Menu_System_old=Menu_System;         --����ԭʼ����������µĺ�����Ҫ�������Ե���ԭʼ������
   Menu_System=Menu_System_new;

   --�ڴ˶���������Ʒ��û�ж���ľ�����ȱʡ��Ʒ����
    JY.ThingUseFunction[182]=Show_Position;     --���̺���
	JY.ThingUseFunction[0]=newThing_0;   --�ı�ԭ�������صĹ���Ϊ���������������书��
	JY.ThingUseFunction[2]=newThing_2;

  --�ڴ˿��Զ���ʹ�����¼������ĳ���
    JY.SceneNewEventFunction[1]=newSceneEvent_1;          --�µĺ����ջ�¼�������

end


--�µ�ϵͳ�Ӳ˵������ӿ������ֺ���Ч
function Menu_System_new()
	local menu={
	             {"��ȡ����",Menu_ReadRecord,1},
                 {"�������",Menu_SaveRecord,1},
				 {"�ر�����",Menu_SetMusic,1},
				 {"�ر���Ч",Menu_SetSound,1},
                 {"�뿪��Ϸ",Menu_Exit,1},   };

    if JY.EnableMusic==0 then
	    menu[3][1]="������";
	end

	if JY.EnableSound==0 then
	    menu[4][1]="����Ч";
    end


    local r=ShowMenu(menu,5,0,80,10,0,0,1,1,24,C_ORANGE, C_WHITE);
    if r == 0 then
        Cls(80,10,CC.ScreenW,CC.ScreenH);
        return 0;
    elseif r<0 then   --Ҫ�˳�ȫ���˵���
        Cls();
        return 1;
 	end
end

function Menu_SetMusic()
    if JY.EnableMusic==0 then
	    JY.EnableMusic=1;
		PlayMIDI(JY.CurrentMIDI);
	else
	    JY.EnableMusic=0;
		lib.PlayMIDI("");
	end
	return 1;
end

function Menu_SetSound()
    if JY.EnableSound==0 then
	    JY.EnableSound=1;
	else
	    JY.EnableSound=0;
	end
	return 1;
end


----------------------------------------------------------------
---------------------------��Ʒʹ�ú���--------------------------


--���̺�������ʾ����ͼ����λ��
function Show_Position()
    if JY.Status ~=GAME_MMAP then
        return 0;
    end
    DrawStrBoxWaitKey(string.format("��ǰλ��(%d,%d)",JY.Base["��X"],JY.Base["��Y"]),C_ORANGE,24);
	return 1;
end


--���������ơ��Ⱥ��������һ���书
function newThing_0(id)
    if JY.Status ==GAME_WMAP then
	    return 0;
	end

    Cls();
    if DrawStrBoxYesNo(-1,-1,"�Ⱥ�������书�������������Ƿ����(Y/N)?",C_WHITE,24,1) == false then
        return 0;
    end
    Cls();
    DrawStrBox(100,10,string.format("˭Ҫ����%s?",JY.Thing[id]["����"]),C_WHITE,24,1);
    local r=SelectTeamMenu(100,45);
    if r<=0 then
	    return 0;
	end

	local pid=JY.Base["����" .. r];

	if JY.Person[pid]["�������ֵ"]<=50 then
	    return 0;
	end

	Cls();
    local numwugong=0;
    local menu={};
    for i=1,10 do
        local tmp=JY.Person[pid]["�书" .. i];
        if tmp>0 then
            menu[i]={JY.Wugong[tmp]["����"],nil,1};
            numwugong=numwugong+1;
        end
    end

    if numwugong==0 then
        return 0;
    end

    DrawStrBox(100,10,string.format("��ѡ��Ҫ���ǵ��书"),C_WHITE,24,1);
    r=ShowMenu(menu,numwugong,0,100,45,0,0,1,1,24,C_ORANGE,C_WHITE);

    if r<=0 then
	    return 0;
    else
        local s=string.format("%s �����书 %s",JY.Person[pid]["����"],JY.Wugong[JY.Person[pid]["�书" .. r]]["����"]);
		DrawStrBoxWaitKey(s,C_WHITE,24);

		for i=r+1,10 do
		    JY.Person[pid]["�书" .. i-1]=JY.Person[pid]["�书" .. i];
		    JY.Person[pid]["�书�ȼ�" .. i-1]=JY.Person[pid]["�书�ȼ�" .. i];
		end

		local v,str=AddPersonAttrib(pid,"�������ֵ",-50);

	    DrawStrBoxWaitKey(str,C_WHITE,24);
        AddPersonAttrib(pid,"����",0);

        instruct_32(id,-1);
	end
    Cls();
	return 1;
end


--����Һ��ս��ʱ����ʹһ�������Ķ��Ѹ��������ָܻ�50%
function newThing_2(thingid)
    if JY.Status ~=GAME_WMAP then
	    return 0;
	end

	local menu={};
    local menunum=0;
    for i=0,WAR.PersonNum-1 do
	    menu[i+1]={JY.Person[WAR.Person[i]["������"]]["����"],nil,0}
        if WAR.Person[i]["�ҷ�"]==true and WAR.Person[i]["����"]==true then
            menu[i+1][3]=1;
			menunum=menunum+1;
        end
    end

	if menunum==0 then
	    return 0;
	end

	Cls();
    DrawStrBox(100,10,string.format("��ѡ��Ҫ����Ķ���"),C_WHITE,24);
    local r=ShowMenu(menu,WAR.PersonNum,0,100,45,0,0,1,1,24,C_ORANGE,C_WHITE);
    Cls();
    if r>0 then
	    r=r-1;           --�˵�����ֵ�Ǵ�1��ʼ��ŵġ�
		WAR.Person[r]["����"]=false;
        local pid=WAR.Person[r]["������"];
        JY.Person[pid]["����"]=JY.Person[pid]["�������ֵ"];
        SetRevivePosition(r);
        instruct_32(thingid,-1);
        WarSetPerson();        --�����趨ս��λ��
	    return 1;
	else
	    return 0;
	end
end

--���ø�����ѵ�λ��Ϊ���뵱ǰʹ����Ʒ��ս����������Ŀ�λ
function  SetRevivePosition(id)
	local minDest=math.huge;
	local x,y;
	War_CalMoveStep(WAR.CurID,100,0);   --�����ƶ����� �������100��
	for i=0,CC.WarWidth-1 do
		for j=0,CC.WarHeight-1 do
			local dest=Byte.get16(WAR.Map3,(j*CC.WarWidth+i)*2);
			if dest>0 and dest <128 then
				if minDest>dest then
					minDest=dest;
					x=i;
					y=j;
				 elseif minDest==dest  then
					 if Rnd(2)==0 then
						x=i;
						y=j;
					end
				end
			end
		end
	end

	if minDest<math.huge then
        WAR.Person[id]["����X"]=x;
        WAR.Person[id]["����Y"]=y;
	end

end


------------------------------------------------------------------------------------------
-------�µĳ����¼�������Ҳ����ÿ����������һ���ļ���Ȼ������ģ���е��롣���߼���������ĳ���ʹ��һ���ļ���
------------------------------------------------------------------------------------------

-------�µĺ����ջ�����¼�������


--flag 1 �ո񴥷���2����Ʒ������3��·������
function newSceneEvent_1(flag)
    if JY.CurrentD<=18 then     --����ǰ��ŵ�D*����Ȼ���þɵĴ�����
         oldEventExecute(flag);
		 return ;
    end

    JY.CurrentEventType=flag;

    --���ո�����ʽ����Ҫ���õ�D*����������Ȼ�������Ӧ�ĺ�������
	--Ҳ������if���ж�JY.CurrentD�����ò�ͬ�ĺ������������������͸�������ˡ�

	local funstr=string.format("newSceneEvent_1_%d();",JY.CurrentD);    --����D*��ţ�������Ӧ������
	local f=loadstring(funstr);    --���غ�����ʵʱ�������
	lib.Debug(funstr);
	f();        --���ú���

    JY.CurrentEventType=-1;
end

--���Ϊ19��D*����
function newSceneEvent_1_19()
    if JY.CurrentEventType~=1 then
	    return ;
	end

    local v=GetD(JY.SubScene,JY.CurrentD,2);       --�õ��ո񴥷��¼���ֵ

	if v<0 then
	    return ;
	elseif v==0 then
        Talk("�ף���ղŻ������棬��ôͻȻ�͵��������ˣ�Ī����ʹ���˴�˵�е����л�Ӱ֮����",0);
		TalkEx("�������㿴���ˡ�����ΤС���ĸ��Τ�󱦣��������������������Ժܶ��˶����ϴ�ġ�",111,0);
        Talk("���Ȼ������磬��ô�����ǲ��Ǳ�������ôһ����أ�",0);
		TalkEx("�ǵ�Ȼ��������Щ��ɫҲֻ�ܺ�����ˣ�û���Ļ�ͬ�������ڽ����ϻ졣�������ṩ�Ķ������������Ҫ��Ҫ���Կ���",111,0);
        Talk("�㶼�ṩ��Щ������񰡣�����һ����",0);
		TalkEx("�ṩ���¼��ַ��񣺵䵱���񣬵����г���Ӷ���лᣬ˲Ϣǧ��",111,0);
        Talk("�ǵ䵱������ǵ����ˣ�",0);
		TalkEx("�ԣ��������ò��ŵĶ����������Խ��������ﵱ���������������������޻صġ���Ȼ����Щ���������ǲ��յġ�����ֻ��ҩƷ��Ҳ���յ����г����۵���Ʒ",111,0);
        Talk("�����г���ʲô��Ī���Ǻ��У�",0);
		TalkEx("�꣡С���㣬���Ĺٲ����ˡ������ṩ�����ϼ�������������Ʒ��",111,0);
        Talk("��Ӷ���л��Ǹ�ʲô�ģ��ѵ�Ҳ�ܽ���������",0);
		TalkEx("�԰������ǲ��Ǿ����е�ûǮ�������ǿ��Ը����ṩ��Ǯ�Ļ��ᣬ������������������Ǯ��ͬʱ˳����������",111,0);
        Talk("�е�����˲Ϣǧ���Ǹ�ʲô�ģ�",0);
		TalkEx("�������Զ�ܵ������������ҿ��Ը�������ṩȥ������ջ�ķ�����һգ�۵Ĺ��򣬾͵���������ջ�ˣ������գ�",111,0);

        SetD(JY.SubScene,JY.CurrentD,2,1)     --���ÿո񴥷��¼���ֵ
    elseif v==1 then
		TalkEx("Ҫ��Ҫ�������ṩ���������",111,0);
        Talk("��������������ʶһ������������",0);
        Cls();

		local menu={ {"�䵱����",newSceneEvent_1_Sale,1},
		             {"�����г�",newSceneEvent_1_Shop,1},
		             {"Ӷ���л�",newSceneEvent_1_Task,1},
					 {"˲Ϣǧ��",newSceneEvent_1_Go,1},}

        local r=ShowMenu(menu,4,0,100,45,0,0,1,1,24,C_ORANGE,C_WHITE);
		Cls();



	end
end

--����
--���ڼ۸�����м۸�ģ�����ա�û�еģ������ҩƷ��һ����10�ɵļ۸��ա�
function newSceneEvent_1_Sale()
    local Price={};
	Price[0]=1600;           --����������
	Price[2]=1000;           --����Һ
    Price[28]=600;           --����������
    Price[29]=700;           --ǧ����֥
    Price[34]=500;           --ǧ���˲�
    Price[35]=800;           --��ɽѩ��

	Cls();
    DrawStrBox(100,10,"��ѡ����Ҫ��������Ʒ",C_WHITE,24);
    lib.ShowSurface();
	WaitKey();
	local thing={};
    local thingnum={};

    for i = 0,CC.MyThingNum-1 do
        thing[i]=JY.Base["��Ʒ" .. i+1];
        thingnum[i]=JY.Base["��Ʒ����" ..i+1];
    end

    local r=SelectThing(thing,thingnum);
	Cls();
    if r<0 then
	    return 0;
	end

    local value;
    if Price[r]==nil then
	    if JY.Thing[r]["����"]==3 then
	        value=10;
		else
            DrawStrBoxWaitKey("��Ǹ��������Ʒ���ǲ��ա�",C_WHITE,24);
			Cls();
			return 0;
		end
	else
	    value=Price[r];
	end

    if DrawStrBoxYesNo(-1,-1,string.format("����%s�ҿ���ֵ%s�����ӣ��Ƿ񵱵�(Y/N)?",JY.Thing[r]["����"],value),C_WHITE,24,1) == true then
        instruct_32(r,-1);                 --��Ʒ����1
        instruct_32(CC.MoneyID,value);     --��������
    end
	Cls();
	return 0;
end




function newSceneEvent_1_Shop()
    TalkEx("�����������������û�еĲ�Ʒ����ӭѡ��",111,0);

	local ShopThing={ {0,2000},         --����������
	                  {2,1200},          --����Һ
					  {28,800},}         --����������

    local menu={};
	for i=1,3 do
	    menu[i]={};
	    menu[i][1]=string.format("%-12s %5d",JY.Thing[ShopThing[i][1]]["����"],ShopThing[i][2]);
        menu[i][2]=nil;
		menu[i][3]=1;
	end

    local r=ShowMenu(menu,3,0,200,200,0,0,1,1,24,C_ORANGE, C_WHITE);

    if r>0 then
        if instruct_31(ShopThing[r][2])==false then
            TalkEx("�ǳ���Ǹ�������ϵ�Ǯ�ƺ�������",111,0);
        else
            instruct_32(CC.MoneyID,-ShopThing[r][2]);     --���Ӽ���
            instruct_32(ShopThing[r][1],1);           --��Ʒ����
            TalkEx("��ӭ�´ι��٣�",111,0);
        end
    end

end


function newSceneEvent_1_Task()
    TalkEx("��ӭ����Ӷ���лᣬ������ѡ������",111,0);

    local menu={ {"ȥɳĮ����֩���",nil,1,61,50},    --�����������Ϊս����ź�Ӯ��õ���Ǯ��
	             {"ȥ�Ϻ�ɱ������",nil,1,89,100},
				 {"ȥ����ɽ��ѩ��",nil,1,6,200},
				 };

    local r=ShowMenu(menu,3,0,200,200,0,0,1,1,24,C_ORANGE, C_WHITE);

    if r>0 then
        TalkEx("ף����ˣ�",111,0);
		if WarMain(menu[r][4],1)==true then
		    instruct_13();
            DrawStrBoxWaitKey(string.format("����ս��ʤ�������%d�����ӣ�",menu[r][5]),C_WHITE,24);
            instruct_32(CC.MoneyID,menu[r][5]);     --���Ӽ���
		else
            DrawStrBoxWaitKey("ս��ʧ�ܣ���Ǹû�����ӿ�����",C_WHITE,24);
		end
	end

    Cls();

end



--ȥ������ջ
function newSceneEvent_1_Go()
    TalkEx("������ѡ��Ҫȥ�Ŀ�ջ��",111,0);
    local Address={ {3,21,50},           --����Ϊ����id�������ڳ�����XY���ꡣע�������������ǿյء�
	                {40,26,43},
					{60,26,42},
                    {61,23,49}, };

    local menu={};
	for i=1,4 do
	    menu[i]={ JY.Scene[Address[i][1]]["����"],nil,1};
	end

    local r=ShowMenu(menu,4,0,200,200,0,0,1,1,24,C_ORANGE, C_WHITE);

    if r>0 then

		lib.ShowSlow(50,1);
		ChangeSMap(Address[r][1],Address[r][2],Address[r][3],0);       --�����³���
		--�����µ�����ͼ���꣬�Ա���³�����ȷ������
		ChangeMMap(JY.Scene[JY.SubScene]["�⾰���X1"],JY.Scene[JY.SubScene]["�⾰���Y1"]+1,0);

        lib.ShowSlow(50,0);
    end;

    Cls();
    return 1;
end

