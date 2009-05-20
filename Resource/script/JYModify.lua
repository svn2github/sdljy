


---本模块存放对JYMain.lua 的修改和扩充。

--尽量把新增加模块放在这里，少修改原始JYMain.Lua文件。
--这里一般包括以下几个部分
--1. SetModify函数。   该函数在游戏开始时调用，可以在此修改原有的数据，以及重定义原有的函数，以实现对原有函数的修改、
--                    这样就可以基本不动原始的函数
--2. 原有函数的重载函数。 SetModify中重载的函数放在此处。尽量不修改JYMain.lua文件，对它的修改采用重定义函数的形式。
--3. 新的物品使用函数。
--4. 新的场景事件函数。





--对jymain的修改，以及增加新的物品函数和场景事件函数。
--注意这里可以定义全程变量。
function SetModify()

   --这是一个定义函数的例子。这里重新修改主菜单中的系统菜单，增加在游戏运行中控制音效的功能。
   --原来只能在jyconst.lua中通过参数在运行前控制，不能做到实时控制。
   Menu_System_old=Menu_System;         --备份原始函数，如果新的函数需要，还可以调用原始函数。
   Menu_System=Menu_System_new;

   --在此定义特殊物品。没有定义的均调用缺省物品函数
    JY.ThingUseFunction[182]=Show_Position;     --罗盘函数
	JY.ThingUseFunction[0]=newThing_0;   --改变原来康贝特的功能为醉生梦死酒忘记武功。
	JY.ThingUseFunction[2]=newThing_2;

  --在此可以定义使用新事件函数的场景
    JY.SceneNewEventFunction[1]=newSceneEvent_1;          --新的河洛客栈事件处理函数

end


--新的系统子菜单，增加控制音乐和音效
function Menu_System_new()
	local menu={
	             {"读取进度",Menu_ReadRecord,1},
                 {"保存进度",Menu_SaveRecord,1},
				 {"关闭音乐",Menu_SetMusic,1},
				 {"关闭音效",Menu_SetSound,1},
                 {"离开游戏",Menu_Exit,1},   };

    if JY.EnableMusic==0 then
	    menu[3][1]="打开音乐";
	end

	if JY.EnableSound==0 then
	    menu[4][1]="打开音效";
    end


    local r=ShowMenu(menu,5,0,80,10,0,0,1,1,24,C_ORANGE, C_WHITE);
    if r == 0 then
        Cls(80,10,CC.ScreenW,CC.ScreenH);
        return 0;
    elseif r<0 then   --要退出全部菜单，
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
---------------------------物品使用函数--------------------------


--罗盘函数，显示主地图主角位置
function Show_Position()
    if JY.Status ~=GAME_MMAP then
        return 0;
    end
    DrawStrBoxWaitKey(string.format("当前位置(%d,%d)",JY.Base["人X"],JY.Base["人Y"]),C_ORANGE,24);
	return 1;
end


--醉生梦死酒。喝后可以忘掉一种武功
function newThing_0(id)
    if JY.Status ==GAME_WMAP then
	    return 0;
	end

    Cls();
    if DrawStrBoxYesNo(-1,-1,"喝后会忘记武功，但损害生命，是否继续(Y/N)?",C_WHITE,24,1) == false then
        return 0;
    end
    Cls();
    DrawStrBox(100,10,string.format("谁要服用%s?",JY.Thing[id]["名称"]),C_WHITE,24,1);
    local r=SelectTeamMenu(100,45);
    if r<=0 then
	    return 0;
	end

	local pid=JY.Base["队伍" .. r];

	if JY.Person[pid]["生命最大值"]<=50 then
	    return 0;
	end

	Cls();
    local numwugong=0;
    local menu={};
    for i=1,10 do
        local tmp=JY.Person[pid]["武功" .. i];
        if tmp>0 then
            menu[i]={JY.Wugong[tmp]["名称"],nil,1};
            numwugong=numwugong+1;
        end
    end

    if numwugong==0 then
        return 0;
    end

    DrawStrBox(100,10,string.format("请选择要忘记的武功"),C_WHITE,24,1);
    r=ShowMenu(menu,numwugong,0,100,45,0,0,1,1,24,C_ORANGE,C_WHITE);

    if r<=0 then
	    return 0;
    else
        local s=string.format("%s 忘记武功 %s",JY.Person[pid]["姓名"],JY.Wugong[JY.Person[pid]["武功" .. r]]["名称"]);
		DrawStrBoxWaitKey(s,C_WHITE,24);

		for i=r+1,10 do
		    JY.Person[pid]["武功" .. i-1]=JY.Person[pid]["武功" .. i];
		    JY.Person[pid]["武功等级" .. i-1]=JY.Person[pid]["武功等级" .. i];
		end

		local v,str=AddPersonAttrib(pid,"生命最大值",-50);

	    DrawStrBoxWaitKey(str,C_WHITE,24);
        AddPersonAttrib(pid,"生命",0);

        instruct_32(id,-1);
	end
    Cls();
	return 1;
end


--还魂液，战斗时可以使一个死亡的队友复活，各项机能恢复50%
function newThing_2(thingid)
    if JY.Status ~=GAME_WMAP then
	    return 0;
	end

	local menu={};
    local menunum=0;
    for i=0,WAR.PersonNum-1 do
	    menu[i+1]={JY.Person[WAR.Person[i]["人物编号"]]["姓名"],nil,0}
        if WAR.Person[i]["我方"]==true and WAR.Person[i]["死亡"]==true then
            menu[i+1][3]=1;
			menunum=menunum+1;
        end
    end

	if menunum==0 then
	    return 0;
	end

	Cls();
    DrawStrBox(100,10,string.format("请选择要复活的队友"),C_WHITE,24);
    local r=ShowMenu(menu,WAR.PersonNum,0,100,45,0,0,1,1,24,C_ORANGE,C_WHITE);
    Cls();
    if r>0 then
	    r=r-1;           --菜单返回值是从1开始编号的。
		WAR.Person[r]["死亡"]=false;
        local pid=WAR.Person[r]["人物编号"];
        JY.Person[pid]["生命"]=JY.Person[pid]["生命最大值"];
        SetRevivePosition(r);
        instruct_32(thingid,-1);
        WarSetPerson();        --重新设定战斗位置
	    return 1;
	else
	    return 0;
	end
end

--设置复活队友的位置为距离当前使用物品的战斗人物最近的空位
function  SetRevivePosition(id)
	local minDest=math.huge;
	local x,y;
	War_CalMoveStep(WAR.CurID,100,0);   --计算移动步数 假设最大100步
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
        WAR.Person[id]["坐标X"]=x;
        WAR.Person[id]["坐标Y"]=y;
	end

end


------------------------------------------------------------------------------------------
-------新的场景事件函数，也可以每个场景单独一个文件，然后再主模块中导入。或者几个相关联的场景使用一个文件。
------------------------------------------------------------------------------------------

-------新的河洛客栈场景事件处理函数


--flag 1 空格触发，2，物品触发，3，路过触发
function newSceneEvent_1(flag)
    if JY.CurrentD<=18 then     --对以前编号的D*，仍然调用旧的处理函数
         oldEventExecute(flag);
		 return ;
    end

    JY.CurrentEventType=flag;

    --按照给定格式生成要调用的D*处理函数名，然后调用相应的函数处理。
	--也可以用if来判断JY.CurrentD来调用不同的函数，这样函数起名就更加灵活了。

	local funstr=string.format("newSceneEvent_1_%d();",JY.CurrentD);    --根据D*编号，生成相应函数名
	local f=loadstring(funstr);    --加载函数。实时编译加载
	lib.Debug(funstr);
	f();        --调用函数

    JY.CurrentEventType=-1;
end

--编号为19的D*触发
function newSceneEvent_1_19()
    if JY.CurrentEventType~=1 then
	    return ;
	end

    local v=GetD(JY.SubScene,JY.CurrentD,2);       --得到空格触发事件的值

	if v<0 then
	    return ;
	elseif v==0 then
        Talk("咦？你刚才还在外面，怎么突然就到这里来了？莫不是使出了传说中的移行幻影之术？",0);
		TalkEx("哈哈，你看错了。我是韦小宝的哥哥韦大宝，我们两个是孪生，所以很多人都会认错的。",111,0);
        Talk("你既然是他哥哥，那么本领是不是比他大那么一点点呢？",0);
		TalkEx("那当然，他的那些货色也只能哄哄新人，没他的货同样可以在江湖上混。我这里提供的都是特殊服务，你要不要试试看？",111,0);
        Talk("你都提供哪些特殊服务啊？介绍一下先",0);
		TalkEx("提供以下几种服务：典当服务，地下市场，佣兵行会，瞬息千里",111,0);
        Talk("那典当服务就是当铺了？",0);
		TalkEx("对，你身上用不着的东西，都可以进行在这里当掉。不过是死当，不能赊回的。当然，有些东西我们是不收的。我们只收药品。也回收地下市场出售的物品",111,0);
        Talk("地下市场是什么？莫非是黑市？",0);
		TalkEx("嘘！小声点，当心官差来了。这里提供市面上见不到的特殊物品。",111,0);
        Talk("那佣兵行会是干什么的，难道也能接任务做？",0);
		TalkEx("对啊！你是不是经常感到没钱花？我们可以给你提供挣钱的机会，给你任务做。可以挣钱的同时顺便练练级。",111,0);
        Talk("有道理，那瞬息千里是干什么的？",0);
		TalkEx("看你大老远跑到我这里来。我可以给你免费提供去其他客栈的服务，您一眨眼的功夫，就到了其他客栈了，岂不美哉？",111,0);

        SetD(JY.SubScene,JY.CurrentD,2,1)     --设置空格触发事件的值
    elseif v==1 then
		TalkEx("要不要试试我提供的特殊服务？",111,0);
        Talk("不错，不错。我来见识一下你的特殊服务。",0);
        Cls();

		local menu={ {"典当服务",newSceneEvent_1_Sale,1},
		             {"地下市场",newSceneEvent_1_Shop,1},
		             {"佣兵行会",newSceneEvent_1_Task,1},
					 {"瞬息千里",newSceneEvent_1_Go,1},}

        local r=ShowMenu(menu,4,0,100,45,0,0,1,1,24,C_ORANGE,C_WHITE);
		Cls();



	end
end

--当铺
--对于价格表中有价格的，则可收。没有的，如果是药品则一律以10兩的价格收。
function newSceneEvent_1_Sale()
    local Price={};
	Price[0]=1600;           --醉生梦死酒
	Price[2]=1000;           --还魂液
    Price[28]=600;           --茯苓首乌丸
    Price[29]=700;           --千年灵芝
    Price[34]=500;           --千年人参
    Price[35]=800;           --天山雪莲

	Cls();
    DrawStrBox(100,10,"请选择您要当掉的物品",C_WHITE,24);
    lib.ShowSurface();
	WaitKey();
	local thing={};
    local thingnum={};

    for i = 0,CC.MyThingNum-1 do
        thing[i]=JY.Base["物品" .. i+1];
        thingnum[i]=JY.Base["物品数量" ..i+1];
    end

    local r=SelectThing(thing,thingnum);
	Cls();
    if r<0 then
	    return 0;
	end

    local value;
    if Price[r]==nil then
	    if JY.Thing[r]["类型"]==3 then
	        value=10;
		else
            DrawStrBoxWaitKey("抱歉，你这物品我们不收。",C_WHITE,24);
			Cls();
			return 0;
		end
	else
	    value=Price[r];
	end

    if DrawStrBoxYesNo(-1,-1,string.format("您的%s我看价值%s兩银子，是否当掉(Y/N)?",JY.Thing[r]["名称"],value),C_WHITE,24,1) == true then
        instruct_32(r,-1);                 --物品减少1
        instruct_32(CC.MoneyID,value);     --银子增加
    end
	Cls();
	return 0;
end




function newSceneEvent_1_Shop()
    TalkEx("我们这里出售市面上没有的产品，欢迎选购",111,0);

	local ShopThing={ {0,2000},         --醉生梦死酒
	                  {2,1200},          --还魂液
					  {28,800},}         --茯苓首乌丸

    local menu={};
	for i=1,3 do
	    menu[i]={};
	    menu[i][1]=string.format("%-12s %5d",JY.Thing[ShopThing[i][1]]["名称"],ShopThing[i][2]);
        menu[i][2]=nil;
		menu[i][3]=1;
	end

    local r=ShowMenu(menu,3,0,200,200,0,0,1,1,24,C_ORANGE, C_WHITE);

    if r>0 then
        if instruct_31(ShopThing[r][2])==false then
            TalkEx("非常抱歉，你身上的钱似乎不够．",111,0);
        else
            instruct_32(CC.MoneyID,-ShopThing[r][2]);     --银子减少
            instruct_32(ShopThing[r][1],1);           --物品增加
            TalkEx("欢迎下次光临！",111,0);
        end
    end

end


function newSceneEvent_1_Task()
    TalkEx("欢迎光临佣兵行会，您可以选择任务",111,0);

    local menu={ {"去沙漠消灭蜘蛛怪",nil,1,61,50},    --最后两个数据为战斗编号和赢后得到的钱数
	             {"去南海杀大鳄鱼",nil,1,89,100},
				 {"去长白山打雪怪",nil,1,6,200},
				 };

    local r=ShowMenu(menu,3,0,200,200,0,0,1,1,24,C_ORANGE, C_WHITE);

    if r>0 then
        TalkEx("祝你好运！",111,0);
		if WarMain(menu[r][4],1)==true then
		    instruct_13();
            DrawStrBoxWaitKey(string.format("您的战斗胜利，获得%d兩银子！",menu[r][5]),C_WHITE,24);
            instruct_32(CC.MoneyID,menu[r][5]);     --银子减少
		else
            DrawStrBoxWaitKey("战斗失败，抱歉没有银子可挣！",C_WHITE,24);
		end
	end

    Cls();

end



--去其他客栈
function newSceneEvent_1_Go()
    TalkEx("您可以选择要去的客栈。",111,0);
    local Address={ {3,21,50},           --依次为场景id，出现在场景的XY坐标。注意这个坐标必须是空地。
	                {40,26,43},
					{60,26,42},
                    {61,23,49}, };

    local menu={};
	for i=1,4 do
	    menu[i]={ JY.Scene[Address[i][1]]["名称"],nil,1};
	end

    local r=ShowMenu(menu,4,0,200,200,0,0,1,1,24,C_ORANGE, C_WHITE);

    if r>0 then

		lib.ShowSlow(50,1);
		ChangeSMap(Address[r][1],Address[r][2],Address[r][3],0);       --设置新场景
		--设置新的主地图坐标，以便从新场景正确出来。
		ChangeMMap(JY.Scene[JY.SubScene]["外景入口X1"],JY.Scene[JY.SubScene]["外景入口Y1"]+1,0);

        lib.ShowSlow(50,0);
    end;

    Cls();
    return 1;
end

