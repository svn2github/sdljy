
----------------------------------------------------------
-----------金庸群侠传复刻之Lua版----------------------------

--版权所无，敬请复制
--您可以随意使用代码

---本代码由游泳的鱼编写

--本模块是lua主模块，由C主程序JYLua.exe调用。C程序主要提供游戏需要的视频、音乐、键盘等API函数，供lua调用。
--游戏的所有逻辑都在lua代码中，以方便大家对代码的修改。


--导入其他模块。之所以做成函数是为了避免编译查错时编译器会寻找这些模块。
function IncludeFile()              --导入其他模块
   -- package.path="script\\?.lua";        ---设置加载路径
   -- require("old_talk");
   -- require("old_kdef");
	--require("JYConst");
    --require("JYModify");
end


function SetGlobal()   --设置游戏内部使用的全程变量
   JY={};

   JY.Status=GAME_INIT;  --游戏当前状态

   --保存R×数据
   JY.Base={};           --基本数据
   JY.PersonNum=0;      --人物个数
   JY.Person={};        --人物数据
   JY.ThingNum=0        --物品数量
   JY.Thing={};         --物品数据
   JY.SceneNum=0        --物品数量
   JY.Scene={};         --物品数据
   JY.WugongNum=0        --物品数量
   JY.Wugong={};         --物品数据
   JY.ShopNum=0        --商店数量
   JY.Shop={};         --商店数据

   JY.S={};             --S*数据
   JY.D={};             --D*数据

   JY.MyCurrentPic=0;       --主角当前走路贴图在贴图文件中偏移
   JY.MyPic=0;              --主角当前贴图
   JY.Mytick=0;             --主角没有走路的持续帧数

   JY.SubScene=-1;          --当前子场景编号
   JY.SubSceneX=0;          --子场景显示位置偏移，场景移动指令使用
   JY.SubSceneY=0;

   JY.Darkness=0;             --=0 屏幕正常显示，=1 不显示，屏幕全黑

   JY.CurrentD=-1;          --当前调用D*的编号
   JY.OldDPass=-1;          --上次触发路过事件的D*编号, 避免多次触发
   JY.CurrentEventType=-1   --当前触发事件的方式 1 空格 2 物品 3 路过

   JY.CurrentThing=-1;      --当前选择物品，触发事件使用

   JY.MmapMusic=-1;         --切换大地图音乐，返回主地图时，如果设置，则播放此音乐

   JY.CurrentMIDI=-1;       --当前播放的音乐id，用来在关闭音乐时保存音乐id。
   JY.EnableMusic=1;        --是否播放音乐 1 播放，0 不播放
   JY.EnableSound=1;        --是否播放音效 1 播放，0 不播放

   JY.ThingUseFunction={};          --物品使用时调用函数，SetModify函数使用，增加新类型的物品
   JY.SceneNewEventFunction={};     --调用场景事件函数，SetModify函数使用，定义使用新场景事件触发的函数

	 JY.DestX = -1;
	 JY.DestY = -1;
   WAR={};     --战斗使用的全程变量。。这里占个位置，因为程序后面不允许定义全局变量了。具体内容在WarSetGlobal函数中

end


function JY_Main()        --主程序入口
	--os.remove("debug.txt");        --清除以前的debug输出
	--lib.Debug("tst");
    xpcall(JY_Main_sub,myErrFun);     --捕获调用错误
end

function myErrFun(err)      --错误处理，打印错误信息
    lib.Debug(err);                 --输出错误信息
    lib.Debug(debug.traceback());   --输出调用堆栈信息
end

function JY_Main_sub()        --真正的游戏主程序入口
	
	IncludeFile();       --导入其他模块
	SetGlobalConst();    --设置全程变量CC, 程序使用的常量
	SetGlobal();         --设置全程变量JY
	SetModify();         --设置对函数的修改，定义新的物品，事件等等
	
	--禁止访问全程变量
	setmetatable(_G,
		{
		__newindex = 
				function (_,n)
					error("attempt read write to undeclared variable " .. n,2);
				end,
					
		__index =
				function (_,n)
					error("attempt read read to undeclared variable " .. n,2);
				end,
		}  
	);
	
	lib.Debug("JY_Main start.");
	
	math.randomseed(lib.GetTime())--os.time());          --初始化随机数发生器
	
	JY.Status=GAME_START;
	
	lib.PicInit(CC.PaletteFile);       --加载原来的256色调色板
	
	Cls();
	lib.ShowSlow(50,0);
	PlayMIDI(16);
	
	
	local menu=
	{  
		{"重新开始", nil, 1},
		{"载入进度", nil, 1},
		{"离开游戏", nil, 1}  
	};
	
	local menuReturn=ShowMenu(menu, 3, 0, CC.ScreenW / 2 - 40, CC.ScreenH / 2 + 20, 0, 0, 0, 0, 32, C_STARTMENU, C_RED)
	
	if menuReturn == 1 then        	--重新开始游戏
		NewGame();          						--设置新游戏数据
		lib.LoadMMap(CC.MMapFile[1],CC.MMapFile[2],CC.MMapFile[3],
		CC.MMapFile[4],CC.MMapFile[5],CC.MWidth,CC.MHeight);
		
		JY.SubScene=CC.NewGameSceneID;         --新游戏直接进入场景
		JY.Scene[JY.SubScene]["名称"]=JY.Person[0]["姓名"] .. "居";
		JY.Base["人X1"]=CC.NewGameSceneX;
		JY.Base["人Y1"]=CC.NewGameSceneY;
		JY.MyPic=CC.NewPersonPic;
		
		lib.ShowSlow(50,1)
		JY.Status=GAME_SMAP;
		JY.MMAPMusic=-1;
		
		lib.PicInit();
		lib.PicLoadFile(CC.SMAPPicFile,0);      --加载场景贴图文件
		lib.PicLoadFile(CC.HeadPicFile,1);
		lib.PicLoadFile(CC.ThingPicFile,2);
		
		PlayMIDI(JY.Scene[JY.SubScene]["进门音乐"]);
		
		JY.SubSceneX=0;
		JY.SubSceneY=0;
		DrawSMap();
		lib.ShowSlow(50,0)
		
	elseif menuReturn == 2 then         --载入旧的进度
		local loadMenu=
		{ 
			{"进度一", nil, 1},
			{"进度二", nil, 1},
			{"进度三", nil, 1} 
		};
		
		local r = ShowMenu(loadMenu, 3, 0, CC.ScreenW / 2 - 40, CC.ScreenH / 2 + 20, 0, 0, 0, 0, 32, C_STARTMENU, C_RED)
		lib.LoadMMap(CC.MMapFile[1],CC.MMapFile[2],CC.MMapFile[3],
		CC.MMapFile[4],CC.MMapFile[5],CC.MWidth,CC.MHeight);
		
		LoadRecord(r);
		JY.Status=GAME_FIRSTMMAP;
	
	elseif menuReturn == 3 then
		return ;
	end
	lib.WaitKey();
	Game_Cycle();
end

function Game_Cycle()       --游戏主循环
    lib.Debug("Start game cycle");

    while JY.Status ~=GAME_END do
        local t1=lib.GetTime();

	    JY.Mytick=JY.Mytick+1;    --20个节拍无击键，则主角变为站立状态
		if JY.Mytick%20==0 then
            JY.MyCurrentPic=0;
		end

        if JY.Mytick%1000==0 then
            JY.MYtick=0;
        end

        if JY.Status==GAME_FIRSTMMAP then  --首次显示主场景，重新调用主场景贴图，渐变显示。然后转到正常显示
            lib.ShowSlow(50,1)
            lib.PicInit();
            lib.PicLoadFile(CC.MMAPPicFile,0);
            lib.PicLoadFile(CC.HeadPicFile,1);
            lib.PicLoadFile(CC.ThingPicFile,2);
            PlayMIDI(16);
            JY.Status=GAME_MMAP;
            lib.DrawMMap(JY.Base["人X"],JY.Base["人Y"],GetMyPic());
			lib.ShowSlow(50,0);
        elseif JY.Status==GAME_MMAP then
            Game_MMap();
		elseif JY.Status==GAME_SMAP then
            Game_SMap()
		end

		local t2=lib.GetTime();
    	if t2-t1<CC.Frame then
            lib.Delay(CC.Frame-(t2-t1));
	    end
	end
end


function GetMyPic()      --计算主角当前贴图
    return CC.MyStartPic+JY.Base["人方向"]*7+JY.MyCurrentPic;
end

--增加当前主角走路动画帧, 主地图和场景地图都使用
function AddMyCurrentPic()        ---增加当前主角走路动画帧,
    JY.MyCurrentPic=JY.MyCurrentPic+1;
	if JY.MyCurrentPic >6 then
        JY.MyCurrentPic=1
	end
end

--场景是否可进
--id 场景代号
--x,y 当前主地图坐标
--返回：true 可进，false 不可进
function CanEnterScene(id,x,y)         --场景是否可进
    if (x==JY.Scene[id]["外景入口X1"] and y==JY.Scene[id]["外景入口Y1"]) or
       (x==JY.Scene[id]["外景入口X2"] and y==JY.Scene[id]["外景入口Y2"]) then
        local e=JY.Scene[id]["进入条件"];
        if e==0 then        --可进
            return true;
        elseif e==1 then    --不可进
            return false
        elseif e==2 then    --有轻功高者进
            for i=1,CC.TeamNum do
                if JY.Base["队伍" .. i]>=0 then
                    if JY.Person[JY.Base["队伍" .. i]]["轻功"]>=70 then
                        return true;
                    end
                end
            end
        end
    end
    return false;
end

--主菜单
function MMenu()      --主菜单
    local menu={      {"医疗",Menu_Doctor,1},
	                  {"解毒",Menu_DecPoison,1},
	                  {"物品",Menu_Thing,1},
	                  {"状态",Menu_Status,1},
	                  {"离队",Menu_PersonExit,1},
	                  {"系统",Menu_System,1},      };
    if JY.Status==GAME_SMAP then  --子场景，后两个菜单不可见
        menu[5][3]=0;
        menu[6][3]=0;
    end

    ShowMenu(menu,6,0,10,10,0,0,1,1,24,C_ORANGE, C_WHITE)
end

--系统子菜单
function Menu_System()         --系统子菜单
	local menu={
	             {"读取进度",Menu_ReadRecord,1},
                 {"保存进度",Menu_SaveRecord,1},
                 {"离开游戏",Menu_Exit,1},   };


    local r=ShowMenu(menu,3,0,80,10,0,0,1,1,24,C_ORANGE, C_WHITE);
    if r == 0 then
        Cls(80,10,CC.ScreenW,CC.ScreenH);
        return 0;
    elseif r<0 then   --要退出全部菜单，
        Cls();
        return 1;
 	end
end



--离开菜单
function Menu_Exit()      --离开菜单
    if DrawStrBoxYesNo(200,10,"是否真的要离开游戏(Y/N)?",C_WHITE,24) == true then
        JY.Status =GAME_END;
    end
    return 1;
end


--保存进度
function Menu_SaveRecord()         --保存进度菜单
	local menu={ {"进度一",nil,1},
                 {"进度二",nil,1},
                 {"进度三",nil,1},  };
    local r=ShowMenu(menu,3,0,200,10,0,0,1,1,24,C_ORANGE, C_WHITE);

    if r>0 then
        DrawStrBox(200,10,"请稍候......",C_WHITE,24);
        ShowScreen();
        SaveRecord(r);
        Cls(200,10,CC.ScreenW,CC.ScreenH);
	end
    return 0;
end

--读取进度
function Menu_ReadRecord()        --读取进度菜单
	local menu={ {"进度一",nil,1},
                 {"进度二",nil,1},
                 {"进度三",nil,1},  };
    local r=ShowMenu(menu,3,0,200,10,0,0,1,1,24,C_ORANGE, C_WHITE);

    if r == 0 then
        return 0;
    elseif r>0 then
        LoadRecord(r);
		JY.Status=GAME_FIRSTMMAP;
        return 1;
	end
end

--状态子菜单
function Menu_Status()           --状态子菜单
    DrawStrBox(80,10,"要查阅谁的状态",C_WHITE,24);
    local r=SelectTeamMenu(80,45);
	Cls(80,10,CC.ScreenW,CC.ScreenH);
    if r >0 then
        ShowPersonStatus(JY.Base["队伍" .. r])
	end
    Cls(80,10,CC.ScreenW,CC.ScreenH);
    ShowScreen();
    return 0;
end

--离队Exit
function Menu_PersonExit()        --离队Exit
    DrawStrBox(80,10,"要求谁离队",C_WHITE,24);

    local r=SelectTeamMenu(80,45);
    if r==1 then
        DrawStrBoxWaitKey("抱歉！没有你游戏进行不下去",C_WHITE,24,1);
    elseif r>1 then
        local personid=JY.Base["队伍" .. r];
        for i,v in ipairs(CC.PersonExit) do         --在离队列表中调用离队函数
             if personid==v[1] then
                 v[2]();
             end
        end
    end
    Cls(80,10,CC.ScreenW,CC.ScreenH);
    ShowScreen();
    return 0;
end

--队伍选择人物菜单
function SelectTeamMenu(x,y)          --队伍选择人物菜单
	local menu={};
	for i=1,CC.TeamNum do
        menu[i]={"",nil,0};
		local id=JY.Base["队伍" .. i]
		if id>=0 then
            if JY.Person[id]["生命"]>0 then
                menu[i][1]=JY.Person[id]["姓名"];
                menu[i][3]=1;
            end
		end
	end
    return ShowMenu(menu,CC.TeamNum,0,x,y,0,0,1,1,24,C_ORANGE, C_WHITE);
end

--显示人状态
function ShowPersonStatus(id)        --显示人状态
--定义内部函数，用来显示属性值
    local p=JY.Person[id];

    local x1=80;
	local y1=10;
	local h=30;
	local i=1;
    DrawBox(x1,y1,600,440,C_WHITE);

	x1=x1+10;
	lib.PicLoadCache(1,p["头像代号"]*2,x1+20,y1+20,1,1);
	y1=y1+100;
    DrawString(x1,y1+h*i,p["姓名"],C_WHITE,24);

    local function DrawAttrib(str,color1,color2,v)    --定义内部函数
        v=v or 0;
        DrawString(x1,y1+h*i,str,color1,24);
        DrawString(x1+96,y1+h*i,string.format("%5d",p[str]+v),color2,24);
        i=i+1;
    end

    i=i+1;
    DrawAttrib("等级",C_ORANGE,C_GOLD);

    local color;              --显示生命和最大值，根据受伤和中毒显示不同颜色
    if p["受伤程度"]<33 then
        color =RGB(236,200,40);
    elseif p["受伤程度"]<66 then
        color=RGB(244,128,32);
    else
        color=RGB(232,32,44);
    end
    DrawString(x1,y1+h*i,"生命",C_ORANGE,24);
    DrawString(x1+48,y1+h*i,string.format("%4d",p["生命"]),color,24);
    DrawString(x1+48+48,y1+h*i,"/",C_GOLD,24);
    if p["中毒程度"]==0 then
        color =RGB(252,148,16);
    elseif p["中毒程度"]<50 then
        color=RGB(120,208,88);
    else
        color=RGB(56,136,36);
    end
    DrawString(x1+48+60,y1+h*i,string.format("%4d",p["生命最大值"]),color,24);

    i=i+1;              --显示内力和最大值，根据内力性质显示不同颜色
    if p["内力性质"]==0 then
        color=RGB(208,152,208);
    elseif p["内力性质"]==1 then
        color=RGB(236,200,40);
    else
        color=RGB(236,236,236);
    end
    DrawString(x1,y1+h*i,"内力",C_ORANGE,24);
    DrawString(x1+48,y1+h*i,string.format("%4d/%4d",p["内力"],p["内力最大值"]),color,24);

    i=i+1;
    DrawAttrib("体力",C_ORANGE,C_GOLD)
    DrawAttrib("经验",C_ORANGE,C_GOLD)

    local tmp;
	if p["等级"] >=CC.Level then
	    tmp="=";
	else
        tmp=string.format("%5d",CC.Exp[p["等级"]]);
	end

    DrawString(x1,y1+h*i,"升级",C_ORANGE,24);
    DrawString(x1+96,y1+h*i,tmp,C_GOLD,24);

    i=i+1
    DrawString(x1,y1+h*i,"装备物品",C_ORANGE,24);

	i=i+1;
	if p["武器"]>-1 then
        DrawString(x1+24,y1+h*i,JY.Thing[p["武器"]]["名称"],C_GOLD,24);
    end

	i=i+1;
	if p["防具"]>-1 then
        DrawString(x1+24,y1+h*i,JY.Thing[p["防具"]]["名称"],C_GOLD,24);
    end



    local tmp1,tmp2,tmp3=0,0,0;
    if p["武器"]>-1 then
        tmp1=tmp1+JY.Thing[p["武器"]]["加攻击力"];
        tmp2=tmp2+JY.Thing[p["武器"]]["加防御力"];
        tmp3=tmp3+JY.Thing[p["武器"]]["加轻功"];
	end
    if p["防具"]>-1 then
        tmp1=tmp1+JY.Thing[p["防具"]]["加攻击力"];
        tmp2=tmp2+JY.Thing[p["防具"]]["加防御力"];
        tmp3=tmp3+JY.Thing[p["防具"]]["加轻功"];
	end

    i=0;
	x1=260;
	y1=20;
    DrawAttrib("攻击力",C_WHITE,C_GOLD,tmp1);
    DrawAttrib("防御力",C_WHITE,C_GOLD,tmp2);
    DrawAttrib("轻功",C_WHITE,C_GOLD,tmp3);

    DrawAttrib("医疗能力",C_WHITE,C_GOLD);
    DrawAttrib("用毒能力",C_WHITE,C_GOLD);
    DrawAttrib("解毒能力",C_WHITE,C_GOLD);
    DrawAttrib("抗毒能力",C_WHITE,C_GOLD);

    DrawAttrib("拳掌功夫",C_WHITE,C_GOLD);
    DrawAttrib("御剑能力",C_WHITE,C_GOLD);
    DrawAttrib("耍刀技巧",C_WHITE,C_GOLD);
    DrawAttrib("特殊兵器",C_WHITE,C_GOLD);

    DrawAttrib("暗器技巧",C_WHITE,C_GOLD);
    DrawAttrib("武学常识",C_WHITE,C_GOLD);
    DrawAttrib("资质",C_WHITE,C_GOLD);

    i=0;
	x1=260+170;
	y1=20;

    DrawString(x1,y1+h*i,"修炼物品",C_ORANGE,24);
	local thingid=p["修炼物品"];
	if thingid>0 then
	    i=i+1;
        DrawString(x1+24,y1+h*i,JY.Thing[thingid]["名称"],C_GOLD,24);
		i=i+1;
        local n=TrainNeedExp(id);
		if n <math.huge then
            DrawString(x1+24,y1+h*i,string.format("%5d/%5d",p["修炼点数"],n),C_GOLD,24);
		else
            DrawString(x1+24,y1+h*i,string.format("%5d/===",p["修炼点数"]),C_GOLD,24);
		end
	else
	    i=i+2;
	end

	i=i+1
    DrawString(x1,y1+h*i,"所会功夫",C_ORANGE,24);
	for j=1,10 do
        i=i+1
		local wugong=p["武功" .. j];
		if wugong > 0 then
    		local level=math.modf(p["武功等级" .. j] / 100)+1;
            DrawString(x1+24,y1+h*i,string.format("%s",JY.Wugong[wugong]["名称"]),C_GOLD,24);
            DrawString(x1+24+120,y1+h*i,string.format("%2d",level),C_GOLD,24);
		end
	end
	ShowScreen();
	WaitKey();
end

--计算人物修炼成功需要的点数
--id 人物id
function TrainNeedExp(id)         --计算人物修炼物品成功需要的点数
    local thingid=JY.Person[id]["修炼物品"];
	local r =0;
	if thingid >= 0 then
        if JY.Thing[thingid]["练出武功"] >=0 then
            local level=0;          --此处的level是实际level-1。这样没有武功時和炼成一级是一样的。
			for i =1,10 do               -- 查找当前已经炼成武功等级
			    if JY.Person[id]["武功" .. i]==JY.Thing[thingid]["练出武功"] then
                    level=math.modf(JY.Person[id]["武功等级" .. i] /100);
					break;
                end
            end
			if level <9 then
                r=(7-math.modf(JY.Person[id]["资质"]/15))*JY.Thing[thingid]["需经验"]*(level+1);
			else
                r=math.huge;
			end
		else
            r=(7-math.modf(JY.Person[id]["资质"]/15))*JY.Thing[thingid]["需经验"]*2;
		end
	end
    return r;
end

--医疗菜单
function Menu_Doctor()       --医疗菜单
    DrawStrBox(80,10,"谁要使用医术",C_WHITE,24);
    DrawStrBox(80,50,"医疗能力",C_ORANGE,24);

	local menu1={};
	for i=1,CC.TeamNum do
        menu1[i]={"",nil,0};
		local id=JY.Base["队伍" .. i]
        if id >=0 then
            if JY.Person[id]["医疗能力"]>=20 then
                 menu1[i][1]=string.format("%-10s%4d",JY.Person[id]["姓名"],JY.Person[id]["医疗能力"]);
                 menu1[i][3]=1;
            end
        end
	end

    local id1,id2;
    local r=ShowMenu(menu1,CC.TeamNum,0,80,90,0,0,1,1,24,C_ORANGE, C_WHITE);

    if r >0 then
	    id1=JY.Base["队伍" .. r];
        Cls(80,10,CC.ScreenW,CC.ScreenH);
		DrawStrBox(80,10,"要医治谁",C_WHITE,24);

		local menu2={};
		for i=1,CC.TeamNum do
			menu2[i]={"",nil,0};
			local id=JY.Base["队伍" .. i]
			if id>=0 then
				 menu2[i][1]=string.format("%-10s%4d/%4d",JY.Person[id]["姓名"],JY.Person[id]["生命"],JY.Person[id]["生命最大值"]);
				 menu2[i][3]=1;
			end
		end

		local r2=ShowMenu(menu2,CC.TeamNum,0,80,50,0,0,1,1,24,C_ORANGE,C_WHITE);

		if r2 >0 then
	        id2=JY.Base["队伍" .. r2];
            local num=ExecDoctor(id1,id2);
			if num>0 then
                AddPersonAttrib(id1,"体力",-2);
			end
            DrawStrBoxWaitKey(string.format("%s 生命增加 %d",JY.Person[id2]["姓名"],num),C_ORANGE,24);
		end
	end

	Cls();

    return 0;
end

--执行医疗
--id1 医疗id2, 返回id2生命增加点数
function ExecDoctor(id1,id2)      --执行医疗
	if JY.Person[id1]["体力"]<50 then
        return 0;
	end

	local add=JY.Person[id1]["医疗能力"];
    local value=JY.Person[id2]["受伤程度"];
    if value > add+20 then
        return 0;
	end

    if value <25 then    --根据受伤程度计算实际医疗能力
        add=add*4/5;
	elseif value <50 then
        add=add*3/4;
	elseif value <75 then
        add=add*2/3;
	else
        add=add/2;
	end
 	add=math.modf(add)+Rnd(5);

    AddPersonAttrib(id2,"受伤程度",-add);
    return AddPersonAttrib(id2,"生命",add)
end

--解毒
function Menu_DecPoison()         --解毒
    DrawStrBox(80,10,"谁要帮人解毒",C_WHITE,24);
    DrawStrBox(80,50,"解毒能力",C_ORANGE,24);

	local menu1={};
	for i=1,CC.TeamNum do
        menu1[i]={"",nil,0};
		local id=JY.Base["队伍" .. i]
        if id>=0 then
            if JY.Person[id]["解毒能力"]>=20 then
                 menu1[i][1]=string.format("%-10s%4d",JY.Person[id]["姓名"],JY.Person[id]["解毒能力"]);
                 menu1[i][3]=1;
            end
        end
	end

    local id1,id2;
    local r=ShowMenu(menu1,CC.TeamNum,0,80,90,0,0,1,1,24,C_ORANGE, C_WHITE);

    if r >0 then
	    id1=JY.Base["队伍" .. r];
        Cls(80,10,CC.ScreenW,CC.ScreenH);
		DrawStrBox(80,10,"替谁解毒",C_WHITE,24);
        DrawStrBox(80,50,"中毒程度",C_ORANGE,24);

		local menu2={};
		for i=1,CC.TeamNum do
			menu2[i]={"",nil,0};
			local id=JY.Base["队伍" .. i]
			if id>=0 then
				 menu2[i][1]=string.format("%-10s%5d",JY.Person[id]["姓名"],JY.Person[id]["中毒程度"]);
				 menu2[i][3]=1;
			end
		end

		local r2=ShowMenu(menu2,CC.TeamNum,0,80,90,0,0,1,1,24,C_ORANGE, C_WHITE);
		if r2 >0 then
	        id2=JY.Base["队伍" .. r2];
            local num=ExecDecPoison(id1,id2);
            DrawStrBoxWaitKey(string.format("%s 中毒程度减少 %d",JY.Person[id2]["姓名"],num),C_ORANGE,24);
		end
	end
    Cls();
    ShowScreen();
    return 0;
end

--解毒
--id1 解毒id2, 返回id2中毒减少点数
function ExecDecPoison(id1,id2)     --执行解毒
    local add=JY.Person[id1]["解毒能力"];
    local value=JY.Person[id2]["中毒程度"];

    if value > add+20 then
        return 0;
	end

 	add=limitX(math.modf(add/3)+Rnd(10)-Rnd(10),0,value);
    return -AddPersonAttrib(id2,"中毒程度",-add);
end

--物品菜单
function Menu_Thing()       --物品菜单
    local thing={};
    local thingnum={};

    for i = 0,CC.MyThingNum-1 do
        thing[i]=JY.Base["物品" .. i+1];
        thingnum[i]=JY.Base["物品数量" ..i+1];
    end

    local r=SelectThing(thing,thingnum);
    if r>=0 then
        UseThing(r);           --使用物品
    end
    return 1;
end

--显示物品菜单
--返回选择的物品编号, -1表示没有选择
function SelectThing(thing,thingnum)        --显示物品供选择
    local xnum,ynum=5,5;
    local dx,dy=90,20;
    local w,h=110,80;
    local cur_line=0;
    local cur_x=0;
    local cur_y=0;
    local cur_thing=-1;


    while true do
        Cls(80,10,CC.ScreenW,CC.ScreenH);
        for y=0,ynum-1 do
            for x=0,xnum-1 do
                local id=y*xnum+x+xnum*cur_line         --当前待选择物品
                if x==cur_x and y==cur_y then
                    lib.Background(dx-10,dy,dx+(xnum)*w-10,dy+20,128);
                    lib.DrawRect(dx-10,dy,dx+(xnum)*w-10,dy+20,C_WHITE);
                    if thing[id]>=0 then
                        cur_thing=thing[id];
                        local str=JY.Thing[thing[id]]["名称"];
                        if JY.Thing[thing[id]]["类型"]==1 or JY.Thing[thing[id]]["类型"]==2 then
                            if JY.Thing[thing[id]]["使用人"] >=0 then
                                str=str .. "(" .. JY.Person[JY.Thing[thing[id]]["使用人"]]["姓名"] .. ")";
                            end
                        end
                        str=str .. ":" .. JY.Thing[thing[id]]["物品说明"];
       			        DrawString(dx-10+1,dy+2,str,C_GOLD,16);
                    else
                        cur_thing=-1;
                    end

                    lib.Background(dx+x*w-10,dy+y*h+20,dx+(x+1)*w-10,dy+(y+1)*h+20,64);
                    lib.DrawRect(dx+x*w-1,dy+y*h+30-1,dx+(x)*w+40,dy+(y)*h+70,C_WHITE);
                else
                    lib.Background(dx+x*w-10,dy+y*h+20,dx+(x+1)*w-10,dy+(y+1)*h+20,192);
                end

                lib.DrawRect(dx+x*w-10,dy+y*h+20,dx+(x+1)*w-10,dy+(y+1)*h+20,C_WHITE);
       			DrawString(dx+x*w-10+1,dy+y*h+20,tostring(id),C_GOLD,12);
                if thing[id]>=0 then
                	lib.PicLoadCache(2,thing[id]*2,dx+x*w,dy+y*h+30,1,1);
        			DrawString(dx+x*w,dy+y*h+80,JY.Thing[thing[id]]["名称"],C_ORANGE,16);
        			DrawString(dx+x*w+50,dy+y*h+55,tostring(thingnum[id]),C_WHITE,16);
                end
            end
        end

        ShowScreen();
	    local keypress=WaitKey();
        lib.Delay(100);
        if keypress==VK_ESCAPE then
            cur_thing=-1;
            break;
        elseif keypress==VK_RETURN or keypress==VK_SPACE then
            break;
        elseif keypress==VK_UP then
            if  cur_y == 0 then
                if  cur_line > 0 then
                    cur_line = cur_line - 1;
                end
            else
                cur_y = cur_y - 1;
            end
        elseif keypress==VK_DOWN then
            if  cur_y ==ynum-1 then
                if  cur_line < (math.modf(200/xnum)-ynum) then
                    cur_line = cur_line + 1;
                end
            else
                cur_y = cur_y + 1;
            end
        elseif keypress==VK_LEFT then
            if  cur_x > 0 then
                cur_x=cur_x-1;
            else
                cur_x=xnum-1;
            end
        elseif keypress==VK_RIGHT then
            if  cur_x ==xnum-1 then
                cur_x=0;
            else
                cur_x=cur_x+1;
            end
        end
    end


    Cls(80,10,CC.ScreenW,CC.ScreenH);
    ShowScreen();
    return cur_thing;
end


-- 读取游戏进度
-- id=0 新进度，=1/2/3 进度
function LoadRecord(id)       -- 读取游戏进度

    --读取R*.idx文件
    local data=Byte.create(6*4);
    Byte.loadfile(data,CC.R_IDXFilename[id],0,6*4);

	local idx={};
	idx[0]=0;
	for i =1,6 do
	    idx[i]=Byte.get32(data,4*(i-1));
	end

    --读取R*.grp文件
    data=Byte.create(idx[1]-idx[0]);              --基本数据
    Byte.loadfile(data,CC.R_GRPFilename[id],idx[0],idx[1]-idx[0]);
    LoadData(JY.Base,CC.Base_S,data);

    JY.PersonNum=math.floor((idx[2]-idx[1])/CC.PersonSize);   --人物
    data=Byte.create(CC.PersonSize);
	for i=0,JY.PersonNum-1 do
        Byte.loadfile(data,CC.R_GRPFilename[id],idx[1]+i*CC.PersonSize,CC.PersonSize);
		JY.Person[i]={};
        LoadData(JY.Person[i],CC.Person_S,data);
	end

    JY.ThingNum=math.floor((idx[3]-idx[2])/CC.ThingSize);     --物品
    data=Byte.create(CC.ThingSize);
	for i=0,JY.ThingNum-1 do
        Byte.loadfile(data,CC.R_GRPFilename[id],idx[2]+i*CC.ThingSize,CC.ThingSize);
		JY.Thing[i]={};
        LoadData(JY.Thing[i],CC.Thing_S,data);
	end

    JY.SceneNum=math.floor((idx[4]-idx[3])/CC.SceneSize);     --场景
    data=Byte.create(CC.SceneSize);
	for i=0,JY.SceneNum-1 do
        Byte.loadfile(data,CC.R_GRPFilename[id],idx[3]+i*CC.SceneSize,CC.SceneSize);
		JY.Scene[i]={};
        LoadData(JY.Scene[i],CC.Scene_S,data);
	end

    JY.WugongNum=math.floor((idx[5]-idx[4])/CC.WugongSize);     --武功
    data=Byte.create(CC.WugongSize);
	for i=0,JY.WugongNum-1 do
        Byte.loadfile(data,CC.R_GRPFilename[id],idx[4]+i*CC.WugongSize,CC.WugongSize);
		JY.Wugong[i]={};
        LoadData(JY.Wugong[i],CC.Wugong_S,data);
	end

    JY.ShopNum=math.floor((idx[6]-idx[5])/CC.ShopSize);     --小宝商店
    data=Byte.create(CC.ShopSize);
	for i=0,JY.ShopNum-1 do
        Byte.loadfile(data,CC.R_GRPFilename[id],idx[5]+i*CC.ShopSize,CC.ShopSize);
		JY.Shop[i]={};
        LoadData(JY.Shop[i],CC.Shop_S,data);
	end

    local ll=filelength(CC.S_Filename[id]);
    JY.S=Byte.create(ll);
    Byte.loadfile(JY.S,CC.S_Filename[id],0,ll);

    ll=filelength(CC.D_Filename[id]);
    JY.D=Byte.create(ll);
    Byte.loadfile(JY.D,CC.D_Filename[id],0,ll);
    
    lib.SetBaseData(JY.S, JY.D);

end

-- 写游戏进度
-- id=0 新进度，=1/2/3 进度
function SaveRecord(id)         -- 写游戏进度
    --读取R*.idx文件
    local data=Byte.create(6*4);
    Byte.loadfile(data,CC.R_IDXFilename[id],0,6*4);

	local idx={};
	idx[0]=0;
	for i =1,6 do
	    idx[i]=Byte.get32(data,4*(i-1));
	end

    --写R*.grp文件
    local tmpbyte=Byte.create(idx[1]-idx[0])              --基本数据
    SaveData(JY.Base,CC.Base_S,tmpbyte);
    Byte.savefile(tmpbyte,CC.R_GRPFilename[id],idx[0],idx[1]-idx[0]);

    tmpbyte=Byte.create(CC.PersonSize);               --人物
	for i=0,JY.PersonNum-1 do
        SaveData(JY.Person[i],CC.Person_S,tmpbyte);
        Byte.savefile(tmpbyte,CC.R_GRPFilename[id],idx[1]+i*CC.PersonSize,CC.PersonSize);
	end

    tmpbyte=Byte.create(CC.ThingSize);               --物品
	for i=0,JY.ThingNum-1 do
        SaveData(JY.Thing[i],CC.Thing_S,tmpbyte);
        Byte.savefile(tmpbyte,CC.R_GRPFilename[id],idx[2]+i*CC.ThingSize,CC.ThingSize);
	end

    tmpbyte=Byte.create(CC.SceneSize);               --场景
	for i=0,JY.SceneNum-1 do
        SaveData(JY.Scene[i],CC.Scene_S,tmpbyte);
        Byte.savefile(tmpbyte,CC.R_GRPFilename[id],idx[3]+i*CC.SceneSize,CC.SceneSize);
	end

    tmpbyte=Byte.create(CC.WugongSize);               --武功
	for i=0,JY.WugongNum-1 do
        SaveData(JY.Wugong[i],CC.Wugong_S,tmpbyte);
        Byte.savefile(tmpbyte,CC.R_GRPFilename[id],idx[4]+i*CC.WugongSize,CC.WugongSize);
	end

    tmpbyte=Byte.create(CC.ShopSize);               --小宝商店
	for i=0,JY.ShopNum-1 do
        SaveData(JY.Shop[i],CC.Shop_S,tmpbyte);
        Byte.savefile(tmpbyte,CC.R_GRPFilename[id],idx[5]+i*CC.ShopSize,CC.ShopSize);
	end

    local ll=filelength(CC.S_Filename[id]);
    Byte.savefile(JY.S,CC.S_Filename[id],0,ll);

    ll=filelength(CC.D_Filename[id]);
    Byte.savefile(JY.D,CC.D_Filename[id],0,ll);
end

-------------------------------------------------------------------------------------
-----------------------------------通用函数-------------------------------------------



function filelength(filename)         --得到文件长度
    --local inp=io.open(filename,"rb");
    --local l= inp:seek("end");
	--inp:close();
    return io.fsize(filename);
end


--读取R×数据
--按照t_struct 定义的结构把数据从data二进制串中读到表t中
function LoadData(t,t_struct,data)        --读取R×数据
    for k,v in pairs(t_struct) do
        if v[2]==0 then
            t[k]=Byte.get16(data,v[1]);
        elseif v[2]==1 then
            t[k]=Byte.getu16(data,v[1]);
		elseif v[2]==2 then
            if CC.SrcCharSet==0 then
                t[k]=lib.CharSet(Byte.getstr(data,v[1],v[3]),0);
		    else
		        t[k]=Byte.getstr(data,v[1],v[3]);
		    end
		end
	end
end

--写R×数据
--按照t_struct 定义的结构把数据写入data Byte数组中。
function SaveData(t,t_struct,data)      --写R×数据
    for k,v in pairs(t_struct) do
        if v[2]==0 then
            Byte.set16(data,v[1],t[k]);
		elseif v[2]==1 then
            Byte.setu16(data,v[1],t[k]);
		elseif v[2]==2 then
		    local s;
			if CC.SrcCharSet==0 then
			    s=lib.CharSet(t[k],1);
            else
			    s=t[k];
		    end
            Byte.setstr(data,v[1],v[3],s);
		end
	end
end


function limitX(x,minv,maxv)       --限制x的范围
    if x<minv then
	    x=minv;
	end
	if x>maxv then
	    x=maxv;
	end
	return x
end

RGB = lib.RGB
GetRGB = lib.GetRGB
subRGB = lib.subRGB

--等待键盘输入
function WaitKey()       --等待键盘输入
    local keyPress=-1;
    while true do
		keyPress=lib.WaitKey();
		if keyPress ~=-1 then
	         break;
	    end
        lib.Delay(20);
	end
	return keyPress;
end

--绘制一个带背景的白色方框，四角凹进
function DrawBox(x1,y1,x2,y2,color)         --绘制一个带背景的白色方框
    local s=4;
    lib.Background(x1,y1+s,x1+s,y2-s,128);    --阴影，四角空出
    lib.Background(x1+s,y1,x2-s,y2,128);
    lib.Background(x2-s,y1+s,x2,y2-s,128);
    local r,g,b=GetRGB(color);
    DrawBox_1(x1+1,y1+1,x2,y2,subRGB(color, 2));
    DrawBox_1(x1,y1,x2-1,y2-1,color);
end

--绘制四角凹进的方框
function DrawBox_1(x1,y1,x2,y2,color)       --绘制四角凹进的方框
    local s=4;
    lib.DrawRect(x1+s,y1,x2-s,y1,color);
    lib.DrawRect(x2-s,y1,x2-s,y1+s,color);
    lib.DrawRect(x2-s,y1+s,x2,y1+s,color);
    lib.DrawRect(x2,y1+s,x2,y2-s,color);
    lib.DrawRect(x2,y2-s,x2-s,y2-s,color);
    lib.DrawRect(x2-s,y2-s,x2-s,y2,color);
    lib.DrawRect(x2-s,y2,x1+s,y2,color);
    lib.DrawRect(x1+s,y2,x1+s,y2-s,color);
    lib.DrawRect(x1+s,y2-s,x1,y2-s,color);
    lib.DrawRect(x1,y2-s,x1,y1+s,color);
    lib.DrawRect(x1,y1+s,x1+s,y1+s,color);
    lib.DrawRect(x1+s,y1+s,x1+s,y1,color);
end

--显示阴影字符串
function DrawString(x,y,str,color,size)         --显示阴影字符串
   -- local r,g,b=GetRGB(color);
    lib.DrawStr(x+1,y+1,str,subRGB(color, 2),size,CC.FontName,CC.SrcCharSet,CC.OSCharSet);
    lib.DrawStr(x,y,str,color,size,CC.FontName,CC.SrcCharSet,CC.OSCharSet);
end

--显示带框的字符串
--(x,y) 坐标，如果都为-1,则在屏幕中间显示
function DrawStrBox(x,y,str,color,size)         --显示带框的字符串
    local ll=#str;
    local w=size*ll/2+8;
	local h=size+8;
	if x==-1 and y==-1 then
        x=(CC.ScreenW-size/2*ll-10)/2;
        y=(CC.ScreenH-size-10)/2;
	end

    DrawBox(x,y,x+w-1,y+h-1,C_WHITE);
    DrawString(x+4,y+4,str,color,size);
end

--显示并询问Y/N，如果点击Y，则返回true, N则返回false
--(x,y) 坐标，如果都为-1,则在屏幕中间显示
function DrawStrBoxYesNo(x,y,str,color,size)        --显示字符串并询问Y/N
    lib.WaitKey();
    DrawStrBox(x,y,str,color,size);
    ShowScreen();
    while true do
        local keypress=WaitKey();
        if keypress==VK_Y then
            return true;
        elseif keypress==VK_N then
            return false
        end
    end
end


--显示字符串并等待击键，字符串带框，显示在屏幕中间
function DrawStrBoxWaitKey(s,color,size)          --显示字符串并等待击键
    lib.WaitKey();
    Cls();
    DrawStrBox(-1,-1,s,color,size);
    ShowScreen();
    WaitKey();
end

--返回 [0 , i-1] 的整形随机数
function Rnd(i)           --随机数
    local r=math.random(i);
    return r-1;
end

--增加人物属性，如果有最大值限制，则应用最大值限制。最小值则限制为0
--id 人物id
--str属性字符串
--value 要增加的值，负数表示减少
--返回1,实际增加的值
--返回2，字符串：xxx 增加/减少 xxxx，用于显示药品效果
function AddPersonAttrib(id,str,value)            --增加人物属性
    local oldvalue=JY.Person[id][str];
    local attribmax=math.huge;
    if str=="生命" then
        attribmax=JY.Person[id]["生命最大值"] ;
    elseif str=="内力" then
        attribmax=JY.Person[id]["内力最大值"] ;
    else
        if CC.PersonAttribMax[str] ~= nil then
            attribmax=CC.PersonAttribMax[str];
        end
    end
    local newvalue=limitX(oldvalue+value,0,attribmax);
    JY.Person[id][str]=newvalue;
    local add=newvalue-oldvalue;

    local showstr="";
    if add>0 then
        showstr=string.format("%s 增加 %d",str,add);
    elseif add<0 then
        showstr=string.format("%s 减少 %d",str,-add);
    end
    return add,showstr;
end

--播放midi
function PlayMIDI(id)             --播放midi
    JY.CurrentMIDI=id;
    if JY.EnableMusic==0 then
        return ;
    end
    if id>=0 then
        lib.PlayMIDI(string.format(CC.MIDIFile,id+1));
    end
end

--播放音效atk***
function PlayWavAtk(id)             --播放音效atk***
    if JY.EnableSound==0 then
        return ;
    end
    if id>=0 then
        lib.PlayWAV(string.format(CC.ATKFile,id));
    end
end

--播放音效e**
function PlayWavE(id)              --播放音效e**
    if JY.EnableSound==0 then
        return ;
    end
    if id>=0 then
        lib.PlayWAV(string.format(CC.EFile,id));
    end
end


function ShowScreen()              --刷新屏幕显示
    if JY.Darkness==0 then
	    lib.ShowSurface();
    end
end

------------------------------------------------------------------------------------
--------------------------------------物品使用---------------------------------------
--物品使用模块
--当前物品id
--返回1 使用了物品， 0 没有使用物品。可能是某些原因不能使用
function UseThing(id)             --物品使用
    --调用函数
	if JY.ThingUseFunction[id]==nil then
	    return DefaultUseThing(id);
	else
        return JY.ThingUseFunction[id](id);
    end
end

--缺省物品使用函数，实现原始游戏效果
--id 物品id
function DefaultUseThing(id)                --缺省物品使用函数
    if JY.Thing[id]["类型"]==0 then
        return UseThing_Type0(id);
    elseif JY.Thing[id]["类型"]==1 then
        return UseThing_Type1(id);
    elseif JY.Thing[id]["类型"]==2 then
        return UseThing_Type2(id);
    elseif JY.Thing[id]["类型"]==3 then
        return UseThing_Type3(id);
    elseif JY.Thing[id]["类型"]==4 then
        return UseThing_Type4(id);
    end
end

--剧情物品，触发事件
function UseThing_Type0(id)              --剧情物品使用
    if JY.SubScene>=0 then
		local x=JY.Base["人X1"]+CC.DirectX[JY.Base["人方向"]+1];
		local y=JY.Base["人Y1"]+CC.DirectY[JY.Base["人方向"]+1];
        local d_num=GetS(JY.SubScene,x,y,3)
        if d_num>=0 then
            JY.CurrentThing=id;
            EventExecute(d_num,2);       --物品触发事件
            JY.CurrentThing=-1;
			return 1;
		else
		    return 0;
        end
    end
end

--装备物品
function UseThing_Type1(id)            --装备物品使用
    DrawStrBox(80,10,string.format("谁要配备%s?",JY.Thing[id]["名称"]),C_WHITE,24);
    local r=SelectTeamMenu(80,45);

    if r>0 then
        local personid=JY.Base["队伍" ..r]
        if CanUseThing(id,personid) then
            if JY.Thing[id]["装备类型"]==0 then
                if JY.Thing[id]["使用人"]>=0 then
                    JY.Person[JY.Thing[id]["使用人"]]["武器"]=-1;
                end
                if JY.Person[personid]["武器"]>=0 then
                    JY.Thing[JY.Person[personid]["武器"]]["使用人"]=-1
                end
                JY.Person[personid]["武器"]=id;
            elseif JY.Thing[id]["装备类型"]==1 then
                if JY.Thing[id]["使用人"]>=0 then
                    JY.Person[JY.Thing[id]["使用人"]]["防具"]=-1;
                end
                if JY.Person[personid]["防具"]>=0 then
                    JY.Thing[JY.Person[personid]["防具"]]["使用人"]=-1
                end
                JY.Person[personid]["防具"]=id;
            end
            JY.Thing[id]["使用人"]=personid
        else
            Cls(80,10,CC.ScreenW,CC.ScreenH,0,1);
            DrawStrBox(80,10,"此人不适合配备此物品",C_WHITE,24);
            ShowScreen();
            WaitKey();
			return 0;
        end
    end

    Cls(80,10,CC.ScreenW,CC.ScreenH,0,1);
    ShowScreen();
	return 1;
end

--判断一个人是否可以装备或修炼一个物品
--返回 true可以修炼，false不可
function CanUseThing(id,personid)           --判断一个人是否可以装备或修炼一个物品
    if JY.Thing[id]["仅修炼人物"] >=0 then
        if JY.Thing[id]["仅修炼人物"] ~= personid then
            return false;
        end
    end

    if JY.Thing[id]["需内力性质"] ~=2 and JY.Person[personid]["内力性质"] ~=2 then
        if JY.Thing[id]["需内力性质"] ~= JY.Person[personid]["内力性质"] then
            return false;
        end
    end

    if JY.Thing[id]["需内力"] > JY.Person[personid]["内力最大值"] then
        return false;
    end
    if JY.Thing[id]["需攻击力"] > JY.Person[personid]["攻击力"] then
        return false;
    end
    if JY.Thing[id]["需轻功"] > JY.Person[personid]["轻功"] then
        return false;
    end
    if JY.Thing[id]["需用毒能力"] > JY.Person[personid]["用毒能力"] then
        return false;
    end
    if JY.Thing[id]["需医疗能力"] > JY.Person[personid]["医疗能力"] then
        return false;
    end
    if JY.Thing[id]["需解毒能力"] > JY.Person[personid]["解毒能力"] then
        return false;
    end
    if JY.Thing[id]["需拳掌功夫"] > JY.Person[personid]["拳掌功夫"] then
        return false;
    end
    if JY.Thing[id]["需御剑能力"] > JY.Person[personid]["御剑能力"] then
        return false;
    end
    if JY.Thing[id]["需耍刀技巧"] > JY.Person[personid]["耍刀技巧"] then
        return false;
    end
    if JY.Thing[id]["需特殊兵器"] > JY.Person[personid]["特殊兵器"] then
        return false;
    end
    if JY.Thing[id]["需暗器技巧"] > JY.Person[personid]["暗器技巧"] then
        return false;
    end
    if JY.Thing[id]["需资质"] >= 0 then
        if JY.Thing[id]["需资质"] > JY.Person[personid]["资质"] then
            return false;
        end
    else
        if -JY.Thing[id]["需资质"] < JY.Person[personid]["资质"] then
            return false;
        end
    end

    return true
end


--秘籍物品
function UseThing_Type2(id)               --秘籍物品使用
    if JY.Thing[id]["使用人"]>=0 then
        if DrawStrBoxYesNo(80,10,"此物品已经有人修炼，是否换人修炼(Y/N)?",C_WHITE,24)==false then
            Cls(80,10,CC.ScreenW,CC.ScreenH);
            ShowScreen();
            return 0;
        end
    end

    Cls(80,10,CC.ScreenW,CC.ScreenH);
    DrawStrBox(80,10,string.format("谁要修炼%s?",JY.Thing[id]["名称"]),C_WHITE,24);
    local r=SelectTeamMenu(80,45);

    if r>0 then
        local personid=JY.Base["队伍" ..r]



        if JY.Thing[id]["练出武功"]>=0 then
            local yes=0;
            for i = 1,10 do
                if JY.Person[personid]["武功"..i]==JY.Thing[id]["练出武功"] then
                    yes=1;             --武功已经有了
                    break;
                end
            end
            if yes==0 and JY.Person[personid]["武功10"]>0 then
                Cls(80,10,CC.ScreenW,CC.ScreenH);
                DrawStrBox(80,10,"一个人只能修炼10种武功",C_WHITE,24);
                ShowScreen();
                WaitKey();
                Cls(80,10,CC.ScreenW,CC.ScreenH);
                ShowScreen();
                return 0;
            end
        end

       if id==78 or id==93 then                --辟邪和葵花
		    if JY.Person[personid]["性别"]==0 then
				Cls(80,10,CC.ScreenW,CC.ScreenH);
				if DrawStrBoxYesNo(80,10,"修炼此书必须先行挥刀自宫，你是否仍要修炼(Y/N)?",C_WHITE,24)==false then
					Cls(80,10,CC.ScreenW,CC.ScreenH);
					ShowScreen();
					return 0;
				else
					JY.Person[personid]["性别"]=2;
				end
			elseif JY.Person[personid]["性别"]==1 then
				Cls(80,10,CC.ScreenW,CC.ScreenH);
				DrawStrBox(80,10,"此人不适合修炼此物品",C_WHITE,24);
				ShowScreen();
				WaitKey();
				return 0;
			end
        end


        if CanUseThing(id,personid) then
            if JY.Thing[id]["使用人"]==personid then
                Cls(80,10,CC.ScreenW,CC.ScreenH);
                ShowScreen();
                return 0;
            end

            if JY.Person[personid]["修炼物品"]>=0 then
                JY.Thing[JY.Person[personid]["修炼物品"]]["使用人"]=-1;
            end

            if JY.Thing[id]["使用人"]>=0 then
                JY.Person[JY.Thing[id]["使用人"]]["修炼物品"]=-1;
                JY.Person[JY.Thing[id]["使用人"]]["修炼点数"]=0;
                JY.Person[JY.Thing[id]["使用人"]]["物品修炼点数"]=0;
            end

            JY.Thing[id]["使用人"]=personid
            JY.Person[personid]["修炼物品"]=id;
            JY.Person[personid]["修炼点数"]=0;
            JY.Person[personid]["物品修炼点数"]=0;
        else
            Cls(80,10,CC.ScreenW,CC.ScreenH);
            DrawStrBox(80,10,"此人不适合修炼此物品",C_WHITE,24);
            ShowScreen();
            WaitKey();
			return 0;
        end
    end

    Cls(80,10,CC.ScreenW,CC.ScreenH);
    ShowScreen();
	return 1;
end

--药品物品
function UseThing_Type3(id)        --药品物品使用
    local usepersonid=-1;
    if JY.Status==GAME_MMAP or JY.Status==GAME_SMAP then
        Cls(80,10,CC.ScreenW,CC.ScreenH);
        DrawStrBox(80,10,string.format("谁要使用%s?",JY.Thing[id]["名称"]),C_WHITE,24);
        local r=SelectTeamMenu(80,45);
        if r>0 then
            usepersonid=JY.Base["队伍" ..r]
        end
    elseif JY.Status==GAME_WMAP then           ---战斗场景使用药品
        usepersonid=WAR.Person[WAR.CurID]["人物编号"];
    end

    if usepersonid>=0 then
        if UseThingEffect(id,usepersonid)==1 then       --使用有效果
            instruct_32(id,-1);            --物品数量减少
            WaitKey();
        else
            return 0;
        end
    end

    Cls(80,10,CC.ScreenW,CC.ScreenH);
    ShowScreen();
	return 1;
end

--药品使用实际效果
--id 物品id，
--personid 使用人id
--返回值：0 使用没有效果，物品数量应该不变。1 使用有效果，则使用后物品数量应该-1
function UseThingEffect(id,personid)          --药品使用实际效果
    local str={};
    str[0]=string.format("使用 %s",JY.Thing[id]["名称"]);
    local strnum=1;
    local addvalue;

    if JY.Thing[id]["加生命"] >0 then
        local add=JY.Thing[id]["加生命"]-math.modf(JY.Person[personid]["受伤程度"]/2)+Rnd(10);
        if add <=0 then
            add=5+Rnd(5);
        end
        AddPersonAttrib(personid,"受伤程度",-JY.Thing[id]["加生命"]/4);
        addvalue,str[strnum]=AddPersonAttrib(personid,"生命",add);
        if addvalue ~=0 then
            strnum=strnum+1
        end
    end

    local function ThingAddAttrib(s)     ---定义局部函数，处理吃药后增加属性
        if JY.Thing[id]["加" .. s] ~=0 then
            addvalue,str[strnum]=AddPersonAttrib(personid,s,JY.Thing[id]["加" .. s]);
            if addvalue ~=0 then
                strnum=strnum+1
            end
        end
    end

    ThingAddAttrib("生命最大值");

    if JY.Thing[id]["加中毒解毒"] <0 then
        addvalue,str[strnum]=AddPersonAttrib(personid,"中毒程度",math.modf(JY.Thing[id]["加中毒解毒"]/2));
        if addvalue ~=0 then
            strnum=strnum+1
        end
    end

    ThingAddAttrib("体力");

    if JY.Thing[id]["改变内力性质"] ==2 then
        str[strnum]="内力门路改为阴阳合一"
        strnum=strnum+1
    end

    ThingAddAttrib("内力");
    ThingAddAttrib("内力最大值");
    ThingAddAttrib("攻击力");
    ThingAddAttrib("防御力");
    ThingAddAttrib("轻功");
    ThingAddAttrib("医疗能力");
    ThingAddAttrib("用毒能力");
    ThingAddAttrib("解毒能力");
    ThingAddAttrib("抗毒能力");
    ThingAddAttrib("拳掌功夫");
    ThingAddAttrib("御剑能力");
    ThingAddAttrib("耍刀技巧");
    ThingAddAttrib("特殊兵器");
    ThingAddAttrib("暗器技巧");
    ThingAddAttrib("武学常识");
    ThingAddAttrib("攻击带毒");

    if strnum>1 then
        local maxlength=0      --计算字符串最大长度
        for i = 0,strnum-1 do
            if #str[i] > maxlength then
                maxlength=#str[i];
            end
        end
        local x,y=100,100;
        Cls(80,10,CC.ScreenW,CC.ScreenH);

		DrawBox(x,y,x+maxlength*12+10,y+strnum*30+10,C_WHITE);
        DrawString(x+5,y+5,str[0],C_WHITE,24);
        for i =1,strnum-1 do
            DrawString(x+5,y+5+30*i,str[i],C_ORANGE,24);
        end
        ShowScreen();
        return 1;
    else
        return 0;
    end

end


--暗器物品
function UseThing_Type4(id)             --暗器物品使用
    if JY.Status==GAME_WMAP then
         return War_UseAnqi(id);
    end
	return 0;
end



--------------------------------------------------------------------------------
--------------------------------------事件调用-----------------------------------

--事件调用主入口
--id，d*中的编号
--flag 1 空格触发，2，物品触发，3，路过触发
function EventExecute(id,flag)               --事件调用主入口
    JY.CurrentD=id;
    if JY.SceneNewEventFunction[JY.SubScene]==nil then         --没有定义新的事件处理函数，调用旧的
        oldEventExecute(flag)
	else
        JY.SceneNewEventFunction[JY.SubScene](flag)         --调用新的事件处理函数
    end
    JY.CurrentD=-1;
end

--调用原有的指定位置的函数
--旧的函数名字格式为  oldevent_xxx();  xxx为事件编号
function oldEventExecute(flag)            --调用原有的指定位置的函数

	local eventnum;
	if flag==1 then
		eventnum=GetD(JY.SubScene,JY.CurrentD,2);
	elseif flag==2 then
		eventnum=GetD(JY.SubScene,JY.CurrentD,3);
	elseif flag==3 then
		eventnum=GetD(JY.SubScene,JY.CurrentD,4);
	end

	if eventnum>=0 then
		local funstr=string.format("oldevent_%d();",eventnum);    --根据事件编号，生成相应函数名
		local f=loadstring(funstr);    --加载函数
		lib.Debug(funstr);
		f();        --调用函数
	end

end


--改变大地图坐标，从场景出去后移动到相应坐标
function ChangeMMap(x,y,direct)          --改变大地图坐标
	JY.Base["人X"]=x;
	JY.Base["人Y"]=y;
	JY.Base["人方向"]=direct;
end

--改变当前场景
function ChangeSMap(sceneid,x,y,direct)       --改变当前场景
    JY.SubScene=sceneid;
	JY.Base["人X1"]=x;
	JY.Base["人Y1"]=y;
	JY.Base["人方向"]=direct;
end


--清除(x1,y1)-(x2,y2)矩形内的文字等。
--如果没有参数，则清除整个屏幕表面
--注意该函数并不直接刷新显示屏幕
function Cls(x1,y1,x2,y2)                    --清除屏幕
    if x1==nil then        --第一个参数为nil,表示没有参数，用缺省
	    x1=0;
		y1=0;
		x2=CC.ScreenW;
		y2=CC.ScreenH;
	end

	lib.SetClip(x1,y1,x2,y2);

	if JY.Status==GAME_START then
        lib.LoadPicture(CC.FirstFile, (CC.ScreenW-320)/2, (CC.ScreenH-200)/4);
	elseif JY.Status==GAME_MMAP then
        lib.DrawMMap(JY.Base["人X"],JY.Base["人Y"],GetMyPic());             --显示主地图
	elseif JY.Status==GAME_SMAP then
        DrawSMap();
	elseif JY.Status==GAME_WMAP then
        WarDrawMap(0);
	elseif JY.Status==GAME_DEAD then
        lib.LoadPicture(CC.DeadFile, (CC.ScreenW-320)/2, (CC.ScreenH-200)/4);
	end
	lib.SetClip(0,0,CC.ScreenW,CC.ScreenH);
end


--测试指令，占位置用
function instruct_test(s)
    DrawStrBoxWaitKey(s,C_ORANGE,24);
end


--清屏
function instruct_0()         --清屏
    Cls();
end

--对话
--talkid: 为数字，则为对话编号；为字符串，则为对话本身。
--headid: 头像id
--flag :对话框位置：0 屏幕上方显示, 左边头像，右边对话
--            1 屏幕下方显示, 左边对话，右边头像
--            2 屏幕上方显示, 左边空，右边对话
--            3 屏幕下方显示, 左边对话，右边空
--            4 屏幕上方显示, 左边对话，右边头像
--            5 屏幕下方显示, 左边头像，右边对话

function instruct_1(talkid,headid,flag)        --对话
    local s=oldtalk[talkid];
	if s==nil then        --对话id不存在
	    return ;
	end
    TalkEx(oldtalk[talkid],headid,flag);
end

--得到物品
function instruct_2(thingid,num)            --得到物品
    if JY.Thing[thingid]==nil then   --无此物品id
        return ;
	end

    instruct_32(thingid,num);    --增加物品
    DrawStrBoxWaitKey(string.format("得到物品:%s %d",JY.Thing[thingid]["名称"],num),C_ORANGE,24);
    instruct_2_sub();         --是否可得武林帖
end

--声望>200以及14天书后得到武林帖
function instruct_2_sub()               --声望>200以及14天书后得到武林帖

    if JY.Person[0]["声望"] < 200 then
        return ;
    end

    if instruct_18(189) ==true then            --有武林帖， 189 武林帖id
        return;
    end

    local booknum=0;
    for i=1,CC.BookNum do
        if instruct_18(CC.BookStart+i-1)==true then
            booknum=booknum+1;
        end
    end

    if booknum==CC.BookNum then        --设置主角居桌子上的武林帖事件
        instruct_3(70,11,-1,1,932,-1,-1,7968,7968,7968,-2,-2,-2);
    end
end

--修改D*
-- sceneid 场景id, -2表示当前场景
-- id  D*的id， -2表示当前id
-- v0 - v10 D*参数， -2表示此参数不变
function instruct_3(sceneid,id,v0,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)     --修改D*
    if sceneid==-2 then
        sceneid=JY.SubScene;
    end
    if id==-2 then
        id=JY.CurrentD;
    end

    if v0~=-2 then
        SetD(sceneid,id,0,v0)
    end
    if v1~=-2 then
        SetD(sceneid,id,1,v1)
    end
    if v2~=-2 then
        SetD(sceneid,id,2,v2)
    end
    if v3~=-2 then
        SetD(sceneid,id,3,v3)
    end
    if v4~=-2 then
        SetD(sceneid,id,4,v4)
    end
    if v5~=-2 then
        SetD(sceneid,id,5,v5)
    end
    if v6~=-2 then
        SetD(sceneid,id,6,v6)
    end
    if v7~=-2 then
        SetD(sceneid,id,7,v7)
    end
    if v8~=-2 then
        SetD(sceneid,id,8,v8)
    end

    SetS(sceneid,GetD(sceneid,id,9),GetD(sceneid,id,10),3,-1);   --如果xy坐标移动了，那么S中相应数据要修改。

    if v9~=-2 then
        SetD(sceneid,id,9,v9)
    end

    if v10~=-2 then
        SetD(sceneid,id,10,v10)
    end
    SetS(sceneid,GetD(sceneid,id,9),GetD(sceneid,id,10),3,id);
end

--是否使用物品触发
function instruct_4(thingid)         --是否使用物品触发
    if JY.CurrentThing==thingid then
        return true;
    else
        return false;
    end
end


function instruct_5()         --选择战斗
    return DrawStrBoxYesNo(-1,-1,"是否与之过招(Y/N)?",C_ORANGE,24);
end


function instruct_6(warid,tmp,tmp,flag)      --战斗
    return WarMain(warid,flag);
end


function instruct_7()                 --已经翻译为return了
    instruct_test("指令7测试")
end


function instruct_8(musicid)            --改变主地图音乐
    JY.MmapMusic=musicid;
end


function instruct_9()                --是否要求加入队伍
    Cls();
    return DrawStrBoxYesNo(-1,-1,"是否要求加入(Y/N)?",C_ORANGE,24);
end


function instruct_10(personid)            --加入队员
    if JY.Person[personid]==nil then
        lib.Debug("instruct_10 error: person id not exist");
		return ;
    end
    local add=0;
    for i =2, CC.TeamNum do             --第一个位置是主角，从第二个开始
        if JY.Base["队伍"..i]<0 then
            JY.Base["队伍"..i]=personid;
            add=1;
            break;
        end
    end
    if add==0 then
        lib.Debug("instruct_10 error: 加入队伍已满");
        return ;
    end

    for i =1,4 do                --个人物品归公
        local id =JY.Person[personid]["携带物品" .. i];
        local n=JY.Person[personid]["携带物品数量" .. i];
        if id>=0 and n>0 then
            instruct_2(id,n);
            JY.Person[personid]["携带物品" .. i]=-1;
            JY.Person[personid]["携带物品数量" .. i]=0;
        end
    end
end


function instruct_11()              --是否住宿
    Cls();
    return DrawStrBoxYesNo(-1,-1,"是否住宿(Y/N)?",C_ORANGE,24);
end


function instruct_12()             --住宿，回复体力
    for i=1,CC.TeamNum do
        local id=JY.Base["队伍" .. i];
        if id>=0 then
            if JY.Person[id]["受伤程度"]<33 and JY.Person[id]["中毒程度"]<=0 then
                JY.Person[id]["受伤程度"]=0;
                AddPersonAttrib(id,"体力",math.huge);     --给一个很大的值，自动限制为最大值
                AddPersonAttrib(id,"生命",math.huge);
                AddPersonAttrib(id,"内力",math.huge);
            end
        end
    end
end


function instruct_13()            --场景变亮
    Cls();
    JY.Darkness=0;
    lib.ShowSlow(50,0)
	lib.WaitKey();
end


function instruct_14()             --场景变黑
    lib.ShowSlow(50,1);
    JY.Darkness=1;
end

function instruct_15()          --game over
    JY.Status=GAME_DEAD;
    Cls();
    DrawString(255,165,JY.Person[0]["姓名"],RGB(0,0,0),24);
    DrawString(400,150,"1999-09-09",RGB(216, 20, 24) ,24);--os.date(),RGB(216, 20, 24) ,24);
    DrawString(400,180,"在地球的某处",RGB(216, 20, 24) ,24);
    DrawString(400,210,"当地人口的失踪数",RGB(216, 20, 24) ,24);
    DrawString(400,240,"又多了一笔。。。",RGB(216, 20, 24) ,24);

    local loadMenu={ {"载入进度一",nil,1},
                     {"载入进度二",nil,1},
                     {"载入进度三",nil,1},
                     {"回家睡觉去",nil,1} };
    local r=ShowMenu(loadMenu,4,0,400,300,0,0,0,0,24,C_ORANGE, C_WHITE)

    if r<4 then
        LoadRecord(r);
        JY.Status=GAME_FIRSTMMAP;
    else
        JY.Status=GAME_END;
    end

end


function instruct_16(personid)      --队伍中是否有某人
    local r=false;
    for i = 1, CC.TeamNum do
        if personid==JY.Base["队伍" .. i] then
            r=true;
            break;
        end
    end;
    return r;
end


function instruct_17(sceneid,level,x,y,v)     --修改场景图形
    if sceneid==-2 then
        sceneid=JY.SubScene;
    end
    SetS(sceneid,x,y,level,v);
end


function instruct_18(thingid)           --是否有某种物品
    for i = 1,CC.MyThingNum do
        if JY.Base["物品" .. i]==thingid then
            return true;
        end
    end
    return false;
end


function instruct_19(x,y)             --改变主角位置
    JY.Base["人X1"]=x;
    JY.Base["人Y1"]=y;
end


function instruct_20()                 --判断队伍是否满
    if JY.Base["队伍" .. CC.TeamNum ] >=0 then
        return true;
    end
    return false;
end


function instruct_21(personid)               --离队
    if JY.Person[personid]==nil then
        lib.Debug("instruct_21 error: personid not exist");
        return ;
    end
    local j=0;
    for i = 1, CC.TeamNum do
        if personid==JY.Base["队伍" .. i] then
            j=i;
            break;
        end
    end;
    if j==0 then
       return;
    end

    for  i=j+1,CC.TeamNum do
        JY.Base["队伍" .. i-1]=JY.Base["队伍" .. i];
    end
    JY.Base["队伍" .. CC.TeamNum]=-1;

    if JY.Person[personid]["武器"]>=0 then
        JY.Thing[JY.Person[personid]["武器"]]["使用人"]=-1;
        JY.Person[personid]["武器"]=-1
    end
    if JY.Person[personid]["防具"]>=0 then
        JY.Thing[JY.Person[personid]["防具"]]["使用人"]=-1;
        JY.Person[personid]["防具"]=-1;
    end

    if JY.Person[personid]["修炼物品"]>=0 then
        JY.Thing[JY.Person[personid]["修炼物品"]]["使用人"]=-1;
        JY.Person[personid]["修炼物品"]=-1;
    end

    JY.Person[personid]["修炼点数"]=0;
    JY.Person[personid]["物品修炼点数"]=0;
end


function instruct_22()            --内力降为0
    for i = 1, CC.TeamNum do
        if JY.Base["队伍" .. i] >=0 then
            JY.Person[JY.Base["队伍" .. i]]["内力"]=0;
        end
    end
end


function instruct_23(personid,value)           --设置用毒
    JY.Person[personid]["用毒能力"]=value;
    AddPersonAttrib(personid,"用毒能力",0)
end

--空指令
function instruct_24()
    instruct_test("指令24测试")
end

--场景移动
--为简化，实际上是场景移动(x2-x1)，(y2-y1)。先y后x。因此，x1,y1可设为0
function instruct_25(x1,y1,x2,y2)             --场景移动
    local sign;
    if y1 ~= y2 then
        if y2<y1 then
            sign=-1;
        else
            sign=1;
        end
        for i=y1,y2,sign do
            local t1=lib.GetTime();
            JY.SubSceneY=JY.SubSceneY+sign;
            DrawSMap();
            ShowScreen();
            local t2=lib.GetTime();
            if (t2-t1)<2*CC.Frame then
                lib.Delay(2*CC.Frame-(t2-t1));
            end
        end
    end

    if x1 ~= x2 then
        if x2<x1 then
            sign=-1;
        else
            sign=1;
        end
        for i=x1,x2,sign do
            local t1=lib.GetTime();
            JY.SubSceneX=JY.SubSceneX+sign;
            DrawSMap();
            ShowScreen();
            local t2=lib.GetTime();
            if (t2-t1)<2*CC.Frame then
                lib.Delay(2*CC.Frame-(t2-t1));
            end
        end
    end
end


function instruct_26(sceneid,id,v1,v2,v3)           --增加D*编号
    if sceneid==-2 then
        sceneid=JY.SubScene;
    end

    local v=GetD(sceneid,id,2);
    SetD(sceneid,id,2,v+v1);
    v=GetD(sceneid,id,3);
    SetD(sceneid,id,3,v+v2);
    v=GetD(sceneid,id,4);
    SetD(sceneid,id,4,v+v3);
end

--显示动画 id=-1 主角位置动画
function instruct_27(id,startpic,endpic)           --显示动画
    local old1,old2,old3;
	if id ~=-1 then
        old1=GetD(JY.SubScene,id,5);
        old2=GetD(JY.SubScene,id,6);
        old3=GetD(JY.SubScene,id,7);
    end

    Cls();
    for i =startpic,endpic,2 do
        local t1=lib.GetTime();
        if id==-1 then
            JY.MyPic=i/2;
        else
            SetD(JY.SubScene,id,5,i);
            SetD(JY.SubScene,id,6,i);
            SetD(JY.SubScene,id,7,i);
        end
        DtoSMap();
        DrawSMap();
        ShowScreen();
        local t2=lib.GetTime();
    	if t2-t1<3*CC.Frame then
            lib.Delay(3*CC.Frame-(t2-t1));
	    end
    end
	if id ~=-1 then
        SetD(JY.SubScene,id,5,old1);
        SetD(JY.SubScene,id,6,old2);
        SetD(JY.SubScene,id,7,old3);
    end
end

--判断品德
function instruct_28(personid,vmin,vmax)          --判断品德
    local v=JY.Person[personid]["品德"];
    if v >=vmin and v<=vmax then
        return true;
    else
        return false;
    end
end

--判断攻击力
function instruct_29(personid,vmin,vmax)           --判断攻击力
    local v=JY.Person[personid]["攻击力"];
    if v >=vmin and v<=vmax then
        return true;
    else
        return false
    end
end

--主角走动
--为简化，走动使用相对值(x2-x1)(y2-y1),因此x1,y1可以为0，不必一定要为当前坐标。
function instruct_30(x1,y1,x2,y2)                --主角走动
    if x1<x2 then
        for i=x1,x2 do
            local t1=lib.GetTime();
            instruct_30_sub(1);
            local t2=lib.GetTime();
            if (t2-t1)<2*CC.Frame then
                lib.Delay(2*CC.Frame-(t2-t1));
            end
        end
    elseif x1>x2 then
        for i=x2,x1 do
            local t1=lib.GetTime();
            instruct_30_sub(2);
            local t2=lib.GetTime();
            if (t2-t1)<2*CC.Frame then
                lib.Delay(2*CC.Frame-(t2-t1));
            end
        end
    end

    if y1<y2 then
        for i=y1,y2 do
            local t1=lib.GetTime();
            instruct_30_sub(3);
            local t2=lib.GetTime();
            if (t2-t1)<2*CC.Frame then
                lib.Delay(2*CC.Frame-(t2-t1));
            end
        end
    elseif y1>y2 then
        for i=y2,y1 do
            local t1=lib.GetTime();
            instruct_30_sub(0);
            local t2=lib.GetTime();
            if (t2-t1)<2*CC.Frame then
                lib.Delay(2*CC.Frame-(t2-t1));
            end
        end
    end
end

--主角走动sub
function instruct_30_sub(direct)            --主角走动sub
    local x,y;
    AddMyCurrentPic();
    x=JY.Base["人X1"]+CC.DirectX[direct+1];
    y=JY.Base["人Y1"]+CC.DirectY[direct+1];
    JY.Base["人方向"]=direct;
    JY.MyPic=GetMyPic();
    DtoSMap();

    if  SceneCanPass(x,y)==true then
        JY.Base["人X1"]=x;
        JY.Base["人Y1"]=y;
    end
    JY.Base["人X1"]=limitX(JY.Base["人X1"],1,CC.SWidth-2);
    JY.Base["人Y1"]=limitX(JY.Base["人Y1"],1,CC.SHeight-2);

    DrawSMap();
    Cls();
    ShowScreen();
end

--判断是否够钱
function instruct_31(num)             --判断是否够钱
    local r=false;
    for i =1,CC.MyThingNum do
        if JY.Base["物品" .. i]==CC.MoneyID then
            if JY.Base["物品数量" .. i]>=num then
                r=true;
            end
            break;
        end
    end
    return r;
end

--增加物品
--num 物品数量，负数则为减少物品
function instruct_32(thingid,num)           --增加物品
    local p=1;
    for i=1,CC.MyThingNum do
        if JY.Base["物品" .. i]==thingid then
            JY.Base["物品数量" .. i]=JY.Base["物品数量" .. i]+num
            p=i;
            break;
        elseif JY.Base["物品" .. i]==-1 then
            JY.Base["物品" .. i]=thingid;
            JY.Base["物品数量" .. i]=num;
            p=1;
            break;
        end
    end

    if JY.Base["物品数量" .. p] <=0 then
        for i=p+1,CC.MyThingNum do
            JY.Base["物品" .. i-1]=JY.Base["物品" .. i];
            JY.Base["物品数量" .. i-1]=JY.Base["物品数量" .. i];
        end
        JY.Base["物品" .. CC.MyThingNum]=-1;
        JY.Base["物品数量" .. CC.MyThingNum]=0;
    end
end

--学会武功
function instruct_33(personid,wugongid,flag)           --学会武功
    local add=0;
    for i=1,10 do
        if JY.Person[personid]["武功" .. i]==0 then
            JY.Person[personid]["武功" .. i]=wugongid;
            JY.Person[personid]["武功等级" .. i]=0;
            add=1
            break;
        end
    end

    if add==0 then      --，武功已满，覆盖最后一个武功
        JY.Person[personid]["武功10" ]=wugongid;
        JY.Person[personid]["武功等级10"]=0;
    end

    if flag==0 then
        DrawStrBoxWaitKey(string.format("%s 学会武功 %s",JY.Person[personid]["姓名"],JY.Wugong[wugongid]["名称"]),C_ORANGE,24);
    end
end

--资质增加
function instruct_34(id,value)              --资质增加
    local add,str=AddPersonAttrib(id,"资质",value);
    DrawStrBoxWaitKey(JY.Person[id]["姓名"] .. str,C_ORANGE,24);
end

--设置武功
function instruct_35(personid,id,wugongid,wugonglevel)         --设置武功
    if id>=0 then
        JY.Person[personid]["武功" .. id+1]=wugongid;
        JY.Person[personid]["武功等级" .. id+1]=wugonglevel;
    else
        local flag=0;
        for i =1,10 do
            if JY.Person[personid]["武功" .. i]==0 then
                flag=1;
                JY.Person[personid]["武功" .. i]=wugongid;
                JY.Person[personid]["武功等级" .. i]=wugonglevel;
                return;
            end
        end
        if flag==0 then
            JY.Person[personid]["武功" .. 1]=wugongid;
            JY.Person[personid]["武功等级" .. 1]=wugonglevel;
        end
    end
end

--判断主角性别
function instruct_36(sex)               --判断主角性别
    if JY.Person[0]["性别"]==sex then
        return true;
    else
        return false;
    end
end


function instruct_37(v)              --增加品德
    AddPersonAttrib(0,"品德",v);
end

--修改场景某层贴图
function instruct_38(sceneid,level,oldpic,newpic)         --修改场景某层贴图
    if sceneid==-2 then
        sceneid=JY.SubScene;
    end

    for i=0,CC.SWidth-1 do
        for j=1, CC.SHeight-1 do
            if GetS(sceneid,i,j,level)==oldpic then
                SetS(sceneid,i,j,level,newpic)
            end
        end
    end
end


function instruct_39(sceneid)             --打开场景
    JY.Scene[sceneid]["进入条件"]=0;
end


function instruct_40(v)                --改变主角方向
    JY.Base["人方向"]=v;
    JY.MyPic=GetMyPic();
end


function instruct_41(personid,thingid,num)        --其他人员增加物品
    local k=0;
    for i =1, 4 do        --已有物品
        if JY.Person[personid]["携带物品" .. i]==thingid then
            JY.Person[personid]["携带物品数量" .. i]=JY.Person[personid]["携带物品数量" .. i]+num;
            k=i;
            break
        end
    end

    --物品减少到0，则后面物品往前移动
    if k>0 and JY.Person[personid]["携带物品数量" .. k] <=0 then
        for i=k+1,4 do
            JY.Person[personid]["携带物品" .. i-1]=JY.Person[personid]["携带物品" .. i];
            JY.Person[personid]["携带物品数量" .. i-1]=JY.Person[personid]["携带物品数量" .. i];
        end
        JY.Person[personid]["携带物品" .. 4]=-1;
        JY.Person[personid]["携带物品数量" .. 4]=0;
    end


    if k==0 then    --没有物品，注意此处不考虑超过4个物品的情况，如果超过，则无法加入。
        for i =1, 4 do        --已有物品
            if JY.Person[personid]["携带物品" .. i]==-1 then
                JY.Person[personid]["携带物品" .. i]=thingid;
                JY.Person[personid]["携带物品数量" .. i]=num;
                break
            end
        end
    end
end


function instruct_42()          --队伍中是否有女性
    local r=false;
    for i =1,CC.TeamNum do
        if JY.Base["队伍" .. i] >=0 then
            if JY.Person[JY.Base["队伍" .. i]]["性别"]==1 then
                r=true;
            end
        end
    end
    return r;
end


function instruct_43(thingid)        --是否有某种物品
    return instruct_18(thingid);
end


function instruct_44(id1,startpic1,endpic1,id2,startpic2,endpic2)     --同时显示两个动画
    local old1=GetD(JY.SubScene,id1,5);
    local old2=GetD(JY.SubScene,id1,6);
    local old3=GetD(JY.SubScene,id1,7);
    local old4=GetD(JY.SubScene,id2,5);
    local old5=GetD(JY.SubScene,id2,6);
    local old6=GetD(JY.SubScene,id2,7);

    Cls();
    for i =startpic1,endpic1,2 do
        local t1=lib.GetTime();
        if id1==-1 then
            JY.MyPic=i/2;
        else
            SetD(JY.SubScene,id1,5,i);
            SetD(JY.SubScene,id1,6,i);
            SetD(JY.SubScene,id1,7,i);
        end
        if id2==-1 then
            JY.MyPic=i/2;
        else
            SetD(JY.SubScene,id2,5,i-startpic1+startpic2);
            SetD(JY.SubScene,id2,6,i-startpic1+startpic2);
            SetD(JY.SubScene,id2,7,i-startpic1+startpic2);
        end
        DtoSMap();
        DrawSMap();
        ShowScreen();
        local t2=lib.GetTime();
    	if t2-t1<3*CC.Frame then
            lib.Delay(3*CC.Frame-(t2-t1));
	    end
    end
    SetD(JY.SubScene,id1,5,old1);
    SetD(JY.SubScene,id1,6,old2);
    SetD(JY.SubScene,id1,7,old3);
    SetD(JY.SubScene,id2,5,old4);
    SetD(JY.SubScene,id2,6,old5);
    SetD(JY.SubScene,id2,7,old6);

end


function instruct_45(id,value)        --增加轻功
    local add,str=AddPersonAttrib(id,"轻功",value);
    DrawStrBoxWaitKey(JY.Person[id]["姓名"] .. str,C_ORANGE,24);
end


function instruct_46(id,value)            --增加内力
    local add,str=AddPersonAttrib(id,"内力最大值",value);
    AddPersonAttrib(id,"内力",0);
    DrawStrBoxWaitKey(JY.Person[id]["姓名"] .. str,C_ORANGE,24);
end


function instruct_47(id,value)
    local add,str=AddPersonAttrib(id,"攻击力",value);           --增加攻击力
    DrawStrBoxWaitKey(JY.Person[id]["姓名"] .. str,C_ORANGE,24);
end


function instruct_48(id,value)         --增加生命
    local add,str=AddPersonAttrib(id,"生命最大值",value);
    AddPersonAttrib(id,"生命",0);
    if instruct_16(id)==true then             --我方队员，显示增加
        DrawStrBoxWaitKey(JY.Person[id]["姓名"] .. str,C_ORANGE,24);
    end
end


function instruct_49(personid,value)       --设置内力属性
    JY.Person[personid]["内力属性"]=value;
end

--判断是否有5种物品
function instruct_50(id1,id2,id3,id4,id5)       --判断是否有5种物品
    local num=0;
    if instruct_18(id1)==true then
        num=num+1;
    end
    if instruct_18(id2)==true then
        num=num+1;
    end
    if instruct_18(id3)==true then
        num=num+1;
    end
    if instruct_18(id4)==true then
        num=num+1;
    end
    if instruct_18(id5)==true then
        num=num+1;
    end
    if num==5 then
        return true;
    else
        return false;
    end
end


function instruct_51()     --问软体娃娃
    instruct_1(2547+Rnd(18),114,0);
end


function instruct_52()       --看品德
    DrawStrBoxWaitKey(string.format("你现在的品德指数为: %d",JY.Person[0]["品德"]),C_ORANGE,24);
end


function instruct_53()        --看声望
    DrawStrBoxWaitKey(string.format("你现在的声望指数为: %d",JY.Person[0]["声望"]),C_ORANGE,24);
end


function instruct_54()        --开放其他场景
    for i = 0, JY.SceneNum-1 do
        JY.Scene[i]["进入条件"]=0;
    end
    JY.Scene[2]["进入条件"]=2;    --云鹤崖
    JY.Scene[38]["进入条件"]=2;   --摩天崖
    JY.Scene[75]["进入条件"]=1;   --桃花岛
    JY.Scene[80]["进入条件"]=1;   --绝情谷底
end


function instruct_55(id,num)      --判断D*编号的触发事件
    if GetD(JY.SubScene,id,2)==num then
        return true;
    else
        return false;
    end
end


function instruct_56(v)             --增加声望
    JY.Person[0]["声望"]=JY.Person[0]["声望"]+v;
    instruct_2_sub();     --是否可以增加武林帖
end

--高昌迷宫劈门
function instruct_57()       --高昌迷宫劈门
    instruct_27(-1,7664,7674);

    for i=0,56,2 do
        if JY.MyPic< 7688/2 then
            JY.MyPic=(7676+i)/2;
        end
        SetD(JY.SubScene,2,5,i+7690);
        SetD(JY.SubScene,2,6,i+7690);
        SetD(JY.SubScene,2,7,i+7690);
        SetD(JY.SubScene,3,5,i+7748);
        SetD(JY.SubScene,3,6,i+7748);
        SetD(JY.SubScene,3,7,i+7748);
        SetD(JY.SubScene,4,5,i+7806);
        SetD(JY.SubScene,4,6,i+7806);
        SetD(JY.SubScene,4,7,i+7806);

        DtoSMap();
        DrawSMap();
        ShowScreen();
        lib.Delay(3*CC.Frame);
    end
end

--武道大会比武
function instruct_58()           --武道大会比武
    local group=5           --比武的组数
    local num1 = 6          --每组有几个战斗
    local num2 = 3          --选择的战斗数
    local startwar=102      --起始战斗编号
    local flag={};

    for i = 0,group-1 do
        for j=0,num1-1 do
            flag[j]=0;
        end

        for j = 1,num2 do
            local r;
            while true do          --选择一场战斗
                r=Rnd(num1);
                if flag[r]==0 then
                    flag[r]=1;
                    break;
                end
            end
            local warnum =r+i*num1;      --武道大会战斗编号
            WarLoad(warnum + startwar);
            instruct_1(2854+warnum, JY.Person[WAR.Data["敌人1"]]["头像代号"], 0);
            instruct_0();
            if WarMain(warnum + startwar, 0) ==true  then     --赢
                instruct_0();
                instruct_13();
                TalkEx("还有那位前辈肯赐教？", 0, 1)
                instruct_0();
            else
                instruct_15();
                return;
            end
        end

        if i < group - 1 then
            TalkEx("少侠已连战三场，*可先休息再战．", 70, 0);
            instruct_0();
            instruct_14();
            lib.Delay(300);
            if JY.Person[0]["受伤程度"] < 50 and JY.Person[0]["中毒程度"] <= 0 then
               JY.Person[0]["受伤程度"] = 0
               AddPersonAttrib(0,"体力",math.huge);
               AddPersonAttrib(0,"内力",math.huge);
               AddPersonAttrib(0,"生命",math.huge);
            end
            instruct_13();
            TalkEx("我已经休息够了，*有谁要再上？",0,1);
            instruct_0();
        end
    end

    TalkEx("接下来换谁？**．．．．*．．．．***没有人了吗？",0,1);
    instruct_0();
    TalkEx("如果还没有人要出来向这位*少侠挑战，那麽这武功天下*第一之名，武林盟主之位，*就由这位少侠夺得．***．．．．．．*．．．．．．*．．．．．．*好，恭喜少侠，这武林盟主*之位就由少侠获得，而这把*”武林神杖”也由你保管．",70,0);
    instruct_0();
    TalkEx("恭喜少侠！",12,0);
    instruct_0();
    TalkEx("小兄弟，恭喜你！",64,4);
    instruct_0();
    TalkEx("好，今年的武林大会到此已*圆满结束，希望明年各位武*林同道能再到我华山一游．",19,0);
    instruct_0();
    instruct_14();
    for i = 24,72 do
        instruct_3(-2, i, 0, 0, -1, -1, -1, -1, -1, -1, -2, -2, -2)
    end
    instruct_0();
    instruct_13();
    TalkEx("历经千辛万苦，我终於打败*群雄，得到这武林盟主之位*及神杖．*但是”圣堂”在那呢？*为什麽没人告诉我，难道大*家都不知道．*这会儿又有的找了．", 0, 1)
    instruct_0();
    instruct_2(143, 1)           --得到神杖

end

--全体队员离队
function instruct_59()           --全体队员离队
    for i=CC.TeamNum,2,-1 do
	    if JY.Base["队伍" .. i]>=0 then
            instruct_21(JY.Base["队伍" .. i]);
	    end
    end

    for i,v in ipairs(CC.AllPersonExit) do
        instruct_3(v[1],v[2],0,0,-1,-1,-1,-1,-1,-1,0,-2,-2);
    end
end

--判断D*图片
function instruct_60(sceneid,id,num)          --判断D*图片
    if sceneid==-2 then
         sceneid=JY.SubScene;
    end

    if id==-2 then
         id=JY.CurrentD;
    end

    if GetD(sceneid,id,5)==num then
        return true;
    else
        return false;
    end
end

--判断是否放完14天书
function instruct_61()               --判断是否放完14天书
    for i=11,24 do
        if GetD(JY.SubScene,i,5) ~= 4664 then
            return false;
        end
    end
    return true;
end

--播放时空机动画，结束
function instruct_62(id1,startnum1,endnum1,id2,startnum2,endnum2)      --播放时空机动画，结束
      JY.MyPic=-1;
      instruct_44(id1,startnum1,endnum1,id2,startnum2,endnum2);

      --此处应该插入正规的片尾动画。这里暂时用图片代替

	  lib.LoadPicture("pic\\end.png",0,0);
	  lib.ShowSurface();
	  PlayMIDI(24);
	  lib.Delay(5000);
	  lib.WaitKey();
	  WaitKey();


	  JY.Status=GAME_END;
end

--设置性别
function instruct_63(personid,sex)          --设置性别
    JY.Person[personid]["性别"]=sex
end

--小宝卖东西
function instruct_64()                 --小宝卖东西
    local headid=111;           --小宝头像


    local id=-1;
    for i=0,JY.ShopNum-1 do                --找到当前商店id
        if CC.ShopScene[i].sceneid==JY.SubScene then
            id=i;
            break;
        end
    end
    if id<0 then
        return ;
    end

    TalkEx("这位小哥，看看有什麽需要*的，小宝我卖的东西价钱绝*对公道．",headid,0);

    local menu={};
    for i=1,5 do
        menu[i]={};
        local thingid=JY.Shop[id]["物品" ..i];
        menu[i][1]=string.format("%-12s %5d",JY.Thing[thingid]["名称"],JY.Shop[id]["物品价格" ..i]);
        menu[i][2]=nil;
        if JY.Shop[id]["物品数量" ..i] >0 then
            menu[i][3]=1;
        else
            menu[i][3]=0;
        end
    end

    local r=ShowMenu(menu,5,0,200,200,0,0,1,1,24,C_ORANGE, C_WHITE);

    if r>0 then
        if instruct_31(JY.Shop[id]["物品价格" ..r])==false then
            TalkEx("非常抱歉，*你身上的钱似乎不够．",headid,0);
        else
            JY.Shop[id]["物品数量" ..r]=JY.Shop[id]["物品数量" ..r]-1;
            instruct_32(CC.MoneyID,-JY.Shop[id]["物品价格" ..r]);
            instruct_32(JY.Shop[id]["物品" ..r],1);
            TalkEx("大爷买了我小宝的东西，*保证绝不後悔．",headid,0);
        end
    end

    for i,v in ipairs(CC.ShopScene[id].d_leave) do
        instruct_3(-2,v,0,-2,-1,-1,939,-1,-1,-1,-2,-2,-2);      --设置离开场景时触发小宝离开事件，
    end
end

--小宝去其他客栈
function instruct_65()           --小宝去其他客栈
    local id=-1;
    for i=0,JY.ShopNum-1 do                --找到当前商店id
        if CC.ShopScene[i].sceneid==JY.SubScene then
            id=i;
            break;
        end
    end
    if id<0 then
        return ;
    end

    ---清除当前商店所有小宝D×
    instruct_3(-2,CC.ShopScene[id].d_shop,0,-2,-1,-1,-1,-1,-1,-1,-2,-2,-2);
    for i,v in ipairs(CC.ShopScene[id].d_leave) do
        instruct_3(-2,v,0,-2,-1,-1,-1,-1,-1,-1,-2,-2,-2);
    end

    local newid=id+1;              --暂时用顺序取代随机，
    if newid>=5 then
        newid=0;
    end

    --设置新的小宝商店位置
    instruct_3(CC.ShopScene[newid].sceneid,CC.ShopScene[newid].d_shop,1,-2,938,-1,-1,8256,8256,8256,-2,-2,-2);
end

--播放音乐
function instruct_66(id)       --播放音乐
    PlayMIDI(id);
end

--播放音效
function instruct_67(id)      --播放音效
     PlayWavAtk(id);
end





---------------------------------------------------------------------------
---------------------------------战斗-----------------------------------
--入口函数为WarMain，由战斗指令调用

--设置战斗全程变量
function WarSetGlobal()            --设置战斗全程变量
    WAR={};

    WAR.Data={};              --战斗信息，war.sta文件

    WAR.SelectPerson={}            --设置选择参战人  0 未选，1 选，不可取消，2 选，可取消。选择参展人菜单调用使用

    WAR.Person={};                 --战斗人物信息
    for i=0,26-1 do
        WAR.Person[i]={};
        WAR.Person[i]["人物编号"]=-1;
        WAR.Person[i]["我方"]=true;            --true 我方，false敌人
        WAR.Person[i]["坐标X"]=-1;
        WAR.Person[i]["坐标Y"]=-1;
        WAR.Person[i]["死亡"]=true;
        WAR.Person[i]["人方向"]=-1;
        WAR.Person[i]["贴图"]=-1;
        WAR.Person[i]["贴图类型"]=0;        --0 wmap中贴图，1 fight***中贴图
        WAR.Person[i]["轻功"]=0;
        WAR.Person[i]["移动步数"]=0;
        WAR.Person[i]["点数"]=0;
        WAR.Person[i]["经验"]=0;
        WAR.Person[i]["自动选择对手"]=-1;     --自动战斗中每个人选择的战斗对手
   end

    WAR.PersonNum=0;               --战斗人物个数

    WAR.AutoFight=0;               --我方自动战斗参数 0 手动，1 自动。

    WAR.CurID=-1;                  --当前操作战斗人id

	WAR.ShowHead=1;                --是否缺省显示头像

    WAR.Map0=nil;                  --战斗地图数据,0层，地面
    WAR.Map1=nil;                  --1层，建筑
    WAR.Map2=nil;                  --2层，战斗人物
    WAR.Map3=nil;                  --3层，计算移动步数
    WAR.Map4=nil;                  --4层，效果 0 空，1 有战斗效果，无人中
                                   ---        2 杀生命 , 3 杀内力, 4 医疗 ， 5 用毒 ， 6 解毒

    WAR.EffectColor={};      ---定义人物头上数字的颜色
    WAR.EffectColor[2]=RGB(236, 200, 40);
    WAR.EffectColor[3]=RGB(112, 12, 112);
    WAR.EffectColor[4]=RGB(236, 200, 40);
    WAR.EffectColor[5]=RGB(96, 176, 64)
    WAR.EffectColor[6]=RGB(104, 192, 232);
end

--战斗主函数
--warid  战斗编号
--isexp  输后是否有经验 0 没经验，1 有经验
--返回  true 战斗胜利，false 失败
function WarLoad(warid)               --战斗主函数
    WarSetGlobal();         --初始化战斗变量
    local data=Byte.create(CC.WarDataSize);      --读取战斗数据
    Byte.loadfile(data,CC.WarFile,warid*CC.WarDataSize,CC.WarDataSize);
    LoadData(WAR.Data,CC.WarData_S,data);
end

function WarMain(warid,isexp)
    lib.Debug(string.format("war start. warid=%d",warid));
    WarLoad(warid);
    WarSelectTeam();          --选择我方
    WarSelectEnemy();         --选择敌人

    WAR.WarID=warid;

 	lib.ShowSlow(50,1) ;      --场景变暗

    WarLoadMap(WAR.Data["地图"]);

    JY.Status=GAME_WMAP;

    lib.PicInit();                   --加载贴图文件
    lib.PicLoadFile(CC.WMAPPicFile,0);
    lib.PicLoadFile(CC.HeadPicFile,1);
    lib.PicLoadFile(CC.ThingPicFile,2);
    lib.PicLoadFile(CC.EffectFile,3);

    PlayMIDI(WAR.Data["音乐"]);

    local first=0;            --第一次显示战斗标记
    local warStatus;          --战斗状态

    while true do             --战斗主循环

        WarPersonSort();    --战斗人按轻功排序=
        WarSetPerson();     --设置战斗人物位置

        for i=0,WAR.PersonNum-1 do
            WAR.Person[i]["贴图"]=WarCalPersonPic(i);
        end


        local p=0;
        while p<WAR.PersonNum do       --每回合战斗循环，每个人轮流战斗
            if WAR.AutoFight==1 then                 --我方自动战斗时读取键盘，看是否取消
                local keypress=lib.WaitKey();
                if keypress==VK_SPACE or keypress==VK_RETURN then
                    WAR.AutoFight=0;
                end
            end


            if WAR.Person[p]["死亡"]==false then

                WAR.CurID=p;
                --Cls();
                if first==0 then              --第一次渐亮显示
                    WarDrawMap(0);
                    --WarShowHead(p);
	                lib.ShowSlow(50,0)
                    first=1;
                else
                    WarDrawMap(0);
                    --WarShowHead(p);
                    ShowScreen();
                end

                local r;
                if WAR.Person[p]["我方"]==true then
                    if WAR.AutoFight==0 then
                        r=War_Manual();                  --手动战斗
                    else
                        r=War_Auto();                  --自动战斗
                    end
                else
                    r=War_Auto();                  --自动战斗
                end

                warStatus=War_isEnd();        --战斗是否结束？   0继续，1赢，2输

                if math.abs(r)==7 then         --等待
                    p=p-1;
                end

                if warStatus>0 then
                    break;
                end

            end
            p=p+1;
        end

        if warStatus>0 then
            break;
        end

        War_PersonLostLife();

    end

    local r;

	WAR.ShowHead=0;

	if warStatus==1 then
        DrawStrBoxWaitKey("战斗胜利",C_WHITE,24);
        r=true;
    elseif warStatus==2 then
        DrawStrBoxWaitKey("战斗失败",C_WHITE,24);
        r=false;
    end

    War_EndPersonData(isexp,warStatus);    --战斗后设置人物参数

    lib.ShowSlow(50,1);

    if JY.Scene[JY.SubScene]["进门音乐"]>=0 then
        PlayMIDI(JY.Scene[JY.SubScene]["进门音乐"]);
    else
        PlayMIDI(0);
    end

    lib.PicInit();
    lib.PicLoadFile(CC.SMAPPicFile,0);
    lib.PicLoadFile(CC.HeadPicFile,1);
    lib.PicLoadFile(CC.ThingPicFile,2);

    JY.Status=GAME_SMAP;
    return r;
end


function War_PersonLostLife()             --计算战斗后每回合由于中毒或受伤而掉血
    for i=0,WAR.PersonNum-1 do
        local pid=WAR.Person[i]["人物编号"]
        if WAR.Person[i]["死亡"]==false then
            if JY.Person[pid]["受伤程度"]>0 then
                local dec=math.modf(JY.Person[pid]["受伤程度"]/20);
                AddPersonAttrib(pid,"生命",-dec);
            end
            if JY.Person[pid]["中毒程度"]>0 then
                local dec=math.modf(JY.Person[pid]["中毒程度"]/10);
                AddPersonAttrib(pid,"生命",-dec);
            end
            if JY.Person[pid]["生命"]<=0 then
                JY.Person[pid]["生命"]=1;
            end
        end
    end
end



function War_EndPersonData(isexp,warStatus)            --战斗以后设置人物参数

--敌方人员参数恢复
    for i=0,WAR.PersonNum-1 do
        local pid=WAR.Person[i]["人物编号"]
        if WAR.Person[i]["我方"]==false then
            JY.Person[pid]["生命"]=JY.Person[pid]["生命最大值"];
            JY.Person[pid]["内力"]=JY.Person[pid]["内力最大值"];
            JY.Person[pid]["体力"]=CC.PersonAttribMax["体力"];
            JY.Person[pid]["受伤程度"]=0;
            JY.Person[pid]["中毒程度"]=0;
        end
    end

    if warStatus==2 and isexp==0 then
        return ;
    end

    local liveNum=0;          --计算我方活着的人数
    for i=0,WAR.PersonNum-1 do
        if WAR.Person[i]["我方"]==true and JY.Person[WAR.Person[i]["人物编号"]]["生命"]>0  then
            liveNum=liveNum+1;
        end
    end

    --分配战斗经验---基本经验，战斗数据中的
    if warStatus==1 then     --赢了才有
        for i=0,WAR.PersonNum-1 do
            local pid=WAR.Person[i]["人物编号"]
            if WAR.Person[i]["我方"]==true then
                if JY.Person[pid]["生命"]>0 then
                    WAR.Person[i]["经验"]=WAR.Person[i]["经验"] + math.modf(WAR.Data["经验"]/liveNum);
                end
            end
        end
    end

    --我方人员参数恢复，输赢都有
    for i=0,WAR.PersonNum-1 do
        local pid=WAR.Person[i]["人物编号"]
        if WAR.Person[i]["我方"]==true then
            if JY.Person[pid]["生命"]<JY.Person[pid]["生命最大值"]/5 then
                JY.Person[pid]["生命"]=math.modf(JY.Person[pid]["生命最大值"]/5);
            end
            if JY.Person[pid]["体力"]<10 then
                JY.Person[pid]["体力"]=10 ;
            end
        end
    end

    --每个人经验增加，以及升级
    for i=0,WAR.PersonNum-1 do
        local pid=WAR.Person[i]["人物编号"];
        AddPersonAttrib(pid,"物品修炼点数",math.modf(WAR.Person[i]["经验"]*0.8));
        AddPersonAttrib(pid,"修炼点数",math.modf(WAR.Person[i]["经验"]*0.8));
        AddPersonAttrib(pid,"经验",WAR.Person[i]["经验"]);

        if WAR.Person[i]["我方"]==true then

            DrawStrBoxWaitKey( string.format("%s 获得经验点数 %d",JY.Person[pid]["姓名"],WAR.Person[i]["经验"]),C_WHITE,24);

			local r=War_AddPersonLevel(pid);     --人物升级

			if r==true then

				DrawStrBoxWaitKey( string.format("%s 升级了",JY.Person[pid]["姓名"]),C_WHITE,24);

			end
        end


        War_PersonTrainBook(pid);            --修炼秘籍
        War_PersonTrainDrug(pid);            --修炼药品
    end
end

--人物是否升级
--pid 人id
--返回 true 升级，false不升级
function War_AddPersonLevel(pid)      --人物是否升级

    local tmplevel=JY.Person[pid]["等级"];
    if tmplevel>=CC.Level then     --级别到顶
        return false;
    end

    if JY.Person[pid]["经验"]<CC.Exp[tmplevel] then     --经验不够升级
        return false
    end

    while true do          --判断可以升几级
        if tmplevel >= CC.Level then
            break;
        end
        if JY.Person[pid]["经验"]>=CC.Exp[tmplevel] then
            tmplevel=tmplevel+1;
        else
            break;
        end
    end
    local leveladd=tmplevel-JY.Person[pid]["等级"];   --升级次数
    JY.Person[pid]["等级"]=JY.Person[pid]["等级"]+leveladd;
    AddPersonAttrib(pid,"生命最大值", (JY.Person[pid]["生命增长"]+Rnd(3))*leveladd*3);
    JY.Person[pid]["生命"]=JY.Person[pid]["生命最大值"];
    JY.Person[pid]["体力"]=CC.PersonAttribMax["体力"];
    JY.Person[pid]["受伤程度"]=0;
    JY.Person[pid]["中毒程度"]=0;

    local cleveradd;
    if JY.Person[pid]["资质"]<30 then
        cleveradd=2;
    elseif JY.Person[pid]["资质"]<50 then
        cleveradd=3;
    elseif JY.Person[pid]["资质"]<70 then
        cleveradd=4;
    elseif JY.Person[pid]["资质"]<90 then
        cleveradd=5;
    else
        cleveradd=6;
    end
    cleveradd=Rnd(cleveradd)+1;        --按照资质计算的增长点，越高则技能增加越多，而内力增加越少。
    AddPersonAttrib(pid,"内力最大值",  (9-cleveradd)*leveladd*4);   --聪明人不练内力
    JY.Person[pid]["内力"]=JY.Person[pid]["内力最大值"];

    AddPersonAttrib(pid,"攻击力",  cleveradd*leveladd);
    AddPersonAttrib(pid,"防御力",  cleveradd*leveladd);
    AddPersonAttrib(pid,"轻功",  cleveradd*leveladd);

    if JY.Person[pid]["医疗能力"]>=20 then
        AddPersonAttrib(pid,"医疗能力",  Rnd(3));
    end
    if JY.Person[pid]["用毒能力"]>=20 then
        AddPersonAttrib(pid,"用毒能力",  Rnd(3));
    end
    if JY.Person[pid]["解毒能力"]>=20 then
        AddPersonAttrib(pid,"解毒能力",  Rnd(3));
    end
    if JY.Person[pid]["拳掌功夫"]>=20 then
        AddPersonAttrib(pid,"拳掌功夫",  Rnd(3));
    end
    if JY.Person[pid]["御剑能力"]>=20 then
        AddPersonAttrib(pid,"御剑能力",  Rnd(3));
    end
    if JY.Person[pid]["耍刀技巧"]>=20 then
        AddPersonAttrib(pid,"耍刀技巧",  Rnd(3));
    end
    if JY.Person[pid]["暗器技巧"]>=20 then
        AddPersonAttrib(pid,"暗器技巧",  Rnd(3));
    end

    return true;

end

--修炼秘籍
function War_PersonTrainBook(pid)           --战斗后修炼秘籍是否成功
    local p=JY.Person[pid];

    local thingid=p["修炼物品"];
    if thingid<0 then
        return ;
    end

    local wugongid=JY.Thing[thingid]["练出武功"];

    local needpoint=TrainNeedExp(pid);      --计算修炼成功需要点数

    if p["修炼点数"]>=needpoint then   --修炼成功

        DrawStrBoxWaitKey( string.format("%s 修炼 %s 成功",p["姓名"],JY.Thing[thingid]["名称"]),C_WHITE,24);

        AddPersonAttrib(pid,"生命最大值",JY.Thing[thingid]["加生命最大值"]);
        if JY.Thing[thingid]["改变内力性质"]==2 then
            p["内力性质"]=2;
        end
        AddPersonAttrib(pid,"内力最大值",JY.Thing[thingid]["加内力最大值"]);
        AddPersonAttrib(pid,"攻击力",JY.Thing[thingid]["加攻击力"]);
        AddPersonAttrib(pid,"轻功",JY.Thing[thingid]["加轻功"]);
        AddPersonAttrib(pid,"防御力",JY.Thing[thingid]["加防御力"]);
        AddPersonAttrib(pid,"医疗能力",JY.Thing[thingid]["加医疗能力"]);
        AddPersonAttrib(pid,"用毒能力",JY.Thing[thingid]["加用毒能力"]);
        AddPersonAttrib(pid,"解毒能力",JY.Thing[thingid]["加解毒能力"]);
        AddPersonAttrib(pid,"抗毒能力",JY.Thing[thingid]["加抗毒能力"]);
        AddPersonAttrib(pid,"拳掌功夫",JY.Thing[thingid]["加拳掌功夫"]);
        AddPersonAttrib(pid,"御剑能力",JY.Thing[thingid]["加御剑能力"]);
        AddPersonAttrib(pid,"耍刀技巧",JY.Thing[thingid]["加耍刀技巧"]);
        AddPersonAttrib(pid,"特殊兵器",JY.Thing[thingid]["加特殊兵器"]);
        AddPersonAttrib(pid,"暗器技巧",JY.Thing[thingid]["加暗器技巧"]);
        AddPersonAttrib(pid,"武学常识",JY.Thing[thingid]["加武学常识"]);
        AddPersonAttrib(pid,"品德",JY.Thing[thingid]["加品德"]);
        AddPersonAttrib(pid,"攻击带毒",JY.Thing[thingid]["加攻击带毒"]);
        if JY.Thing[thingid]["加攻击次数"]==1 then
            p["左右互搏"]=1;
        end

        p["修炼点数"]=0;

        if wugongid>=0 then
            local oldwugong=0;
            for i =1,10 do
                if p["武功" .. i]==wugongid then
                    oldwugong=1;
                    p["武功等级" .. i]=p["武功等级" .. i]+100;

                    DrawStrBoxWaitKey(string.format("%s 升为第%s级",JY.Wugong[wugongid]["名称"],math.modf(p["武功等级" ..i]/100)+1),C_WHITE,24);

                    break;
                end
            end
            if oldwugong==0 then  --新的武功
                for i=1,10 do
                    if p["武功" .. i]==0 then
                        p["武功" .. i]=wugongid;
                        break;
                    end
                end
                ---这里不考虑10个武功已满的时候如何增加新的武功
            end
        end
    end
end

--练出药品
function War_PersonTrainDrug(pid)         --战斗后是否修炼出物品
    local p=JY.Person[pid];

    local thingid=p["修炼物品"];
    if thingid<0 then
        return ;
    end

    if JY.Thing[thingid]["练出物品需经验"] <=0 then          --不可以修炼出物品
        return ;
    end

    local needpoint=(7-math.modf(p["资质"]/15))*JY.Thing[thingid]["练出物品需经验"];
    if p["物品修炼点数"]< needpoint then
        return ;
    end

    local haveMaterial=0;       --是否有需要的材料
    local MaterialNum=-1;
    for i=1,CC.MyThingNum do
        if JY.Base["物品" .. i]==JY.Thing[thingid]["需材料"] then
            haveMaterial=1;
            MaterialNum=JY.Base["物品数量" .. i]
            break;
        end
    end

    if haveMaterial==1 then   --有材料
        local enough={};
        local canMake=0;
        for i=1,5 do       --根据需要材料的数量，标记可以练出哪些物品
            if MaterialNum >= JY.Thing[thingid]["需要物品数量" .. i] then
                canMake=1;
                enough[i]=1;
            else
                enough[i]=0;
            end
        end

        if canMake ==1 then    --可练出物品
            local makeID;
            while true do      --随机选择练出的物品，必须是前面enough数组中标记可以练出的
                makeID=Rnd(5)+1;
                if enough[makeID]==1 then
                    break;
                end
            end
            local newThingID=JY.Thing[thingid]["练出物品" .. makeID];

            DrawStrBoxWaitKey(string.format("%s 制造出 %s",p["姓名"],JY.Thing[newThingID]["名称"]),C_WHITE,24);

            if instruct_18(newThingID)==true then       --已经有物品
                instruct_32(newThingID,1);
            else
                instruct_32(newThingID,1+Rnd(3));
            end

            instruct_32(JY.Thing[thingid]["需材料"],-JY.Thing[thingid]["需要物品数量" .. makeID]);
            p["物品修炼点数"]=0;
        end
    end
end

--战斗是否结束
--返回：0 继续   1 赢    2 输
function War_isEnd()           --战斗是否结束

    for i=0,WAR.PersonNum-1 do
        if JY.Person[WAR.Person[i]["人物编号"]]["生命"]<=0 then
            WAR.Person[i]["死亡"]=true;
        end
    end
    WarSetPerson();     --设置战斗人物位置

    Cls();
    ShowScreen();

    local myNum=0;
    local EmenyNum=0;
    for i=0,WAR.PersonNum-1 do
        if WAR.Person[i]["死亡"]==false then
            if WAR.Person[i]["我方"]==true then
                myNum=1;
            else
                EmenyNum=1;
            end
        end
    end

    if EmenyNum==0 then
        return 1;
    end
    if myNum==0 then
        return 2;
    end
    return 0;
end

--选择我方参战人
function WarSelectTeam()            --选择我方参战人
    WAR.PersonNum=0;

    for i=1,6 do
	    local id=WAR.Data["自动选择参战人" .. i];
		if id>=0 then
            WAR.Person[WAR.PersonNum]["人物编号"]=id;
            WAR.Person[WAR.PersonNum]["我方"]=true;
            WAR.Person[WAR.PersonNum]["坐标X"]=WAR.Data["我方X"  .. i];
            WAR.Person[WAR.PersonNum]["坐标Y"]=WAR.Data["我方Y"  .. i];
            WAR.Person[WAR.PersonNum]["死亡"]=false;
            WAR.Person[WAR.PersonNum]["人方向"]=2;
            WAR.PersonNum=WAR.PersonNum+1;
		end
    end
	if WAR.PersonNum>0 then
	    return ;
    end

    for i=1,CC.TeamNum do                 --设置事先确定的参战人
        WAR.SelectPerson[i]=0;
        local id=JY.Base["队伍" .. i];
        if id>=0 then
            for j=1,6 do
                if WAR.Data["手动选择参战人" .. j]==id then
                    WAR.SelectPerson[i]=1;
                end
            end
        end
    end

    local menu={};
    for i=1, CC.TeamNum do
        menu[i]={"",WarSelectMenu,0};
        local id=JY.Base["队伍" .. i];
        if id>=0 then
            menu[i][3]=1;
            local s=JY.Person[id]["姓名"];
            if WAR.SelectPerson[i]==1 then
                menu[i][1]="*" .. s;
            else
                menu[i][1]=" " .. s;
            end
        end
    end

    menu[CC.TeamNum+1]={" 结束",nil,1}

    Cls();
    DrawStrBox(100,100,"请选择参战人物",C_WHITE,24);
    local r=ShowMenu(menu,CC.TeamNum+1,0,100,140,0,0,1,0,24,C_ORANGE,C_WHITE);
    Cls();
    for i=1,6 do
        if WAR.SelectPerson[i]>0 then
            WAR.Person[WAR.PersonNum]["人物编号"]=JY.Base["队伍" ..i];
            WAR.Person[WAR.PersonNum]["我方"]=true;
            WAR.Person[WAR.PersonNum]["坐标X"]=WAR.Data["我方X"  .. i];
            WAR.Person[WAR.PersonNum]["坐标Y"]=WAR.Data["我方Y"  .. i];
            WAR.Person[WAR.PersonNum]["死亡"]=false;
            WAR.Person[WAR.PersonNum]["人方向"]=2;
            WAR.PersonNum=WAR.PersonNum+1;
        end
    end

end


--选中战斗人菜单调用函数
function WarSelectMenu(newmenu,newid)            --选择战斗人菜单调用函数
    local id=newmenu[newid][4];

    if WAR.SelectPerson[id]==0 then
        WAR.SelectPerson[id]=2;
    elseif WAR.SelectPerson[id]==2 then
        WAR.SelectPerson[id]=0;
    end

    if WAR.SelectPerson[id]>0 then
        newmenu[newid][1]="*" .. string.sub(newmenu[newid][1],2);
    else
        newmenu[newid][1]=" " .. string.sub(newmenu[newid][1],2);
    end
    return 0;
end

--选择敌方参战人
function WarSelectEnemy()            --选择敌方参战人
    for i=1,20 do
        if WAR.Data["敌人"  .. i]>0 then
            WAR.Person[WAR.PersonNum]["人物编号"]=WAR.Data["敌人"  .. i];
            WAR.Person[WAR.PersonNum]["我方"]=false;
            WAR.Person[WAR.PersonNum]["坐标X"]=WAR.Data["敌方X"  .. i];
            WAR.Person[WAR.PersonNum]["坐标Y"]=WAR.Data["敌方Y"  .. i];
            WAR.Person[WAR.PersonNum]["死亡"]=false;
            WAR.Person[WAR.PersonNum]["人方向"]=1;
            WAR.PersonNum=WAR.PersonNum+1;
        end
    end
end


function WarLoadMap(mapid)          --读取战斗地图
    --读取war.idx文件
    local idxfile=CC.WarMapFile[1];
    local grpfile=CC.WarMapFile[2];

    local idxnum=filelength(idxfile)/4;
    local data=Byte.create(idxnum*4);
    Byte.loadfile(data,idxfile,0,idxnum*4);

	local idx={};
	idx[0]=0;
	for i =1,idxnum do
	    idx[i]=Byte.get32(data,4*(i-1));
	end

    local ll=CC.WarWidth*CC.WarHeight*2;
    WAR.Map0=Byte.create(ll);
    Byte.loadfile(WAR.Map0,grpfile,idx[mapid],ll);
    WAR.Map1=Byte.create(ll);
    Byte.loadfile(WAR.Map1,grpfile,idx[mapid]+ll,ll);
    WAR.Map2=Byte.create(ll);
    WAR.Map3=Byte.create(ll);
    WAR.Map4=Byte.create(ll);
end

--绘战斗地图
--flag==0 基本
--      1 显示移动路径   movex,movey 当前移动位置
--      2 命中人物（武功，医疗等）另一个颜色显示
--      3 在人的头顶显示点数，
function WarDrawMap(flag,movex,movey)            --绘战斗地图

    local rangeX=18+7;
	local rangeY=10+7;

	local i,j,i1,j1
	local x1,y1
    local picNum;
    local xx,yy;
	local j_2;

    lib.FillColor(0,0,0,0,0);    --清屏

    local dx=WAR.Person[WAR.CurID]["坐标X"];
    local dy=WAR.Person[WAR.CurID]["坐标Y"];

	for j=-rangeY, 2*rangeX+rangeY do
	   j_2=math.modf(j/2);
       for i=rangeX, 0, -1 do
            if 2*j_2==j then
                i1=-rangeX+i+ j_2;
				j1=-i+j_2;
			else
                i1=-rangeX+i+ j_2;
				j1=-i+j_2+1;
			end

			x1=CC.XScale*(i1-j1)+CC.ScreenW/2;
            y1=CC.YScale*(i1+j1)+CC.ScreenH/2;

			xx=dx+i1;
			yy=dy+j1;

            if yy >=0 and xx>=0 and xx <CC.WarWidth and yy<CC.WarHeight then
                local offset=(yy*CC.WarWidth+xx)*2
                local num=Byte.get16(WAR.Map0,offset);
                if num>0 then
                    lib.PicLoadCache(0,num,x1,y1);

                end

            end
		end
	end

if flag==1 then
	for j=-rangeY, 2*rangeX+rangeY do
	   j_2=math.modf(j/2);
       for i=rangeX, 0, -1 do
            if 2*j_2==j then
                i1=-rangeX+i+ j_2;
				j1=-i+j_2;
			else
                i1=-rangeX+i+ j_2;
				j1=-i+j_2+1;
			end

			x1=CC.XScale*(i1-j1)+CC.ScreenW/2;
            y1=CC.YScale*(i1+j1)+CC.ScreenH/2;

			xx=dx+i1;
			yy=dy+j1;

            if yy >=0 and xx>=0 and xx <CC.WarWidth and yy<CC.WarHeight then
                local offset=(yy*CC.WarWidth+xx)*2
				if Byte.get16(WAR.Map3,offset)<128 then
				    local flag;
					if WAR.Data["地图"]==0 then     --雪地地图用黑色菱形
					    flag=2+4;
					else                           --其他地图用白色菱形
					    flag=2+8;
					end
					if xx==movex and yy==movey then
						lib.PicLoadCache(0,0,x1,y1,flag,128);        --使用战斗贴图0的菱形。注意不要改动此贴图内容
					else
						lib.PicLoadCache(0,0,x1,y1,flag,64);
					end
				end
            end
		end
	end

end

	for j=-rangeY, 2*rangeX+rangeY do
	   j_2=math.modf(j/2);
       for i=rangeX, 0, -1 do
            if 2*j_2==j then
                i1=-rangeX+i+ j_2;
				j1=-i+j_2;
			else
                i1=-rangeX+i+ j_2;
				j1=-i+j_2+1;
			end

			x1=CC.XScale*(i1-j1)+CC.ScreenW/2;
            y1=CC.YScale*(i1+j1)+CC.ScreenH/2;

			xx=dx+i1;
			yy=dy+j1;

            if yy >=0 and xx>=0 and xx <CC.WarWidth and yy<CC.WarHeight then
                local offset=(yy*CC.WarWidth+xx)*2

                local num=Byte.get16(WAR.Map1,offset);
                if num>0 then
                    lib.PicLoadCache(0,num,x1,y1);
                end

                num=Byte.get16(WAR.Map2,offset);
                if num>=0 then
                    if flag==0 or flag==1 or flag==3 then
                        if WAR.Person[num]["贴图类型"]==0 then
                            lib.PicLoadCache(0,WAR.Person[num]["贴图"],x1,y1);
                        else
                            lib.PicLoadCache(4,WAR.Person[num]["贴图"],x1,y1);
                        end
                    elseif flag==2 then
                        if  Byte.get16(WAR.Map4,offset)>1 then
                            lib.PicLoadCache(0,WAR.Person[num]["贴图"],x1,y1,4+2,256);
                        else
                            lib.PicLoadCache(0,WAR.Person[num]["贴图"],x1,y1);
                        end
                    end

                    if flag==3 then
                        local effect=Byte.get16(WAR.Map4,offset);
                        if effect>1 then
                            local n=WAR.Person[num]["点数"];
                            DrawString(x1,y1-65-movex,string.format("%+d",n),WAR.EffectColor[effect],24);
                        end
                    end
                end
            end
		end
	end

	if WAR.ShowHead==1 then
        WarShowHead();
	end
end

--显示武功效果
--pic 效果贴图
function WarDrawEffect(pic)           --显示武功效果
    local rangeX=18+7;
	local rangeY=10+7;
	local i,j,i1,j1
	local x1,y1
    local picNum;
    local xx,yy;
	local j_2;

    local dx=WAR.Person[WAR.CurID]["坐标X"];
    local dy=WAR.Person[WAR.CurID]["坐标Y"];

	for j=-rangeY, 2*rangeX+rangeY do
	   j_2=math.modf(j/2);
       for i=rangeX, 0, -1 do
            if 2*j_2==j then
                i1=-rangeX+i+ j_2;
				j1=-i+j_2;
			else
                i1=-rangeX+i+ j_2;
				j1=-i+j_2+1;
			end

			x1=CC.XScale*(i1-j1)+CC.ScreenW/2;
            y1=CC.YScale*(i1+j1)+CC.ScreenH/2;

			xx=dx+i1;
			yy=dy+j1;

            if yy >=0 and xx>=0 and xx <CC.WarWidth and yy<CC.WarHeight then
                local offset=(yy*CC.WarWidth+xx)*2
                local num=Byte.get16(WAR.Map4,offset);
                if num>0 then
                     lib.PicLoadCache(3,pic,x1,y1);
                end
            end
		end
	end
end


function WarPersonSort()               --战斗人物按轻功排序

    for i=0,WAR.PersonNum-1 do                ---计算各人的轻功，包括装备加成
        local id=WAR.Person[i]["人物编号"];

        local add=0;
        if JY.Person[id]["武器"]>-1 then
            add=add+JY.Thing[JY.Person[id]["武器"]]["加轻功"];
        end
        if JY.Person[id]["防具"]>-1 then
            add=add+JY.Thing[JY.Person[id]["防具"]]["加轻功"];
        end
        WAR.Person[i]["轻功"]=JY.Person[id]["轻功"]+add;
        local move=math.modf(WAR.Person[i]["轻功"]/15)-math.modf(JY.Person[id]["受伤程度"]/40);
        if move<0 then
            move=0;
        end
        WAR.Person[i]["移动步数"]=move;
    end

    --按轻功排序，用比较笨的方法
    for i=0,WAR.PersonNum-2 do
        local maxid=i;
        for j=i,WAR.PersonNum-1 do
            if WAR.Person[j]["轻功"]>WAR.Person[maxid]["轻功"] then
                maxid=j;
            end
        end
        WAR.Person[maxid],WAR.Person[i]=WAR.Person[i],WAR.Person[maxid];
    end
end

--设置战斗人物位置
function WarSetPerson()            --设置战斗人物位置

    for i=0,CC.WarWidth*CC.WarHeight-1 do    --清零
        Byte.set16(WAR.Map2,i*2,-1);
    end

    for i=0,WAR.PersonNum-1 do
        if WAR.Person[i]["死亡"]==false then
            Byte.set16(WAR.Map2,(WAR.Person[i]["坐标Y"]*CC.WarWidth+WAR.Person[i]["坐标X"])*2,i);
        end
    end

end


function WarCalPersonPic(id)       --计算战斗人物贴图
    local n=5106;            --战斗人物贴图起始位置
    n=n+JY.Person[WAR.Person[id]["人物编号"]]["头像代号"]*8+WAR.Person[id]["人方向"]*2;
    return n;
end

-------------------------------------------------------------------------------------------
---------------------------------以下为手动战斗函数-------------------------------------------
-------------------------------------------------------------------------------------------

--手动战斗
--id 战斗人物编号
--返回，选中菜单编号，选中"等待"时有效，
function War_Manual()                --手动战斗
    local pid=WAR.Person[WAR.CurID]["人物编号"];
    local menu={ {"移动",War_MoveMenu,1},
                 {"攻击",War_FightMenu,1},
                 {"用毒",War_PoisonMenu,1},
                 {"解毒",War_DecPoisonMenu,1},
                 {"医疗",War_DoctorMenu,1},
                 {"物品",War_ThingMenu,1},
                 {"等待",War_WaitMenu,1},
                 {"状态",War_StatusMenu,1},
                 {"休息",War_RestMenu,1},
                 {"自动",War_AutoMenu,1},   };

    if JY.Person[pid]["体力"]<=5 or WAR.Person[WAR.CurID]["移动步数"]<=0 then  --不能移动
        menu[1][3]=0;
    end

    local minv=math.huge;              --计算所有武功中需要内力最少的
    for i=1,10 do
        local tmpid=JY.Person[pid]["武功" .. i];
        if tmpid >0 then
            if JY.Wugong[tmpid]["消耗内力点数"]< minv then
                minv=JY.Wugong[tmpid]["消耗内力点数"];
            end
        end
    end

    if JY.Person[pid]["内力"] < minv or JY.Person[pid]["体力"] <10 then  --不能战斗
        menu[2][3]=0;
    end

    if JY.Person[pid]["体力"]<10 or JY.Person[pid]["用毒能力"]<20 then  --不能用毒
        menu[3][3]=0;
    end

    if JY.Person[pid]["体力"]<10 or JY.Person[pid]["解毒能力"]<20 then  --不能解毒
        menu[4][3]=0;
    end

    if JY.Person[pid]["体力"]<50 or JY.Person[pid]["医疗能力"]<20 then  --不能医疗
        menu[5][3]=0;
    end

    lib.WaitKey();
    Cls();
    return ShowMenu(menu,10,0,10,10,0,0,1,0,24,C_ORANGE,C_WHITE);
end


function WarShowHead()               --显示战斗人头像
    local pid=WAR.Person[WAR.CurID]["人物编号"];
	local p=JY.Person[pid];

    local x1=500;
	local y1=10;
	local h=20;
	local i=1;
    if WAR.Person[WAR.CurID]["我方"]==true then
        y1=270;
    else
        y1=10;
    end

    DrawBox(x1,y1,630-1,200-1+y1,C_WHITE);
	x1=x1+10;
	lib.PicLoadCache(1,p["头像代号"]*2,x1+10,y1+10,1,1);
	y1=y1+100;
    DrawString(x1,y1+h*i,p["姓名"],C_WHITE,16);

    local color;              --显示生命和最大值，根据受伤和中毒显示不同颜色
    if p["受伤程度"]<33 then
        color =RGB(236,200,40);
    elseif p["受伤程度"]<66 then
        color=RGB(244,128,32);
    else
        color=RGB(232,32,44);
    end
	i=i+1;
    DrawString(x1,y1+h*i,"生命",C_ORANGE,16);
    DrawString(x1+40,y1+h*i,string.format("%4d",p["生命"]),color,16);
    DrawString(x1+40+32,y1+h*i,"/",C_GOLD,16);
    if p["中毒程度"]==0 then
        color =RGB(252,148,16);
    elseif p["中毒程度"]<50 then
        color=RGB(120,208,88);
    else
        color=RGB(56,136,36);
    end
    DrawString(x1+40+40,y1+h*i,string.format("%4d",p["生命最大值"]),color,16);

    i=i+1;              --显示内力和最大值，根据内力性质显示不同颜色
    if p["内力性质"]==0 then
        color=RGB(208,152,208);
    elseif p["内力性质"]==1 then
        color=RGB(236,200,40);
    else
        color=RGB(236,236,236);
    end
    DrawString(x1,y1+h*i,"内力",C_ORANGE,16);
    DrawString(x1+40,y1+h*i,string.format("%4d/%4d",p["内力"],p["内力最大值"]),color,16);

	i=i+1;
	DrawString(x1,y1+h*i,"体力",C_ORANGE,16);
	DrawString(x1+40,y1+h*i,string.format("%4d",p["体力"]),C_GOLD,16);

end



function War_MoveMenu()           --执行移动菜单
    if WAR.Person[WAR.CurID]["移动步数"]<=0 then
        return 0;
    end

    War_CalMoveStep(WAR.CurID,WAR.Person[WAR.CurID]["移动步数"],0);   --计算移动步数

    local x,y=War_SelectMove()             --选择移动位置
    if x ~=nil then            --空值表示没有选择，esc返回了，非空则表示选择了位置
        War_MovePerson(x,y);    --移动到相应的位置
    end
    lib.WaitKey();
	Cls();

    return 0;
end

--计算可移动步数
--id 战斗人id，
--stepmax 最大步数，
--flag=0  移动，物品不能绕过，1 武功，用毒医疗等，不考虑挡路。
function War_CalMoveStep(id,stepmax,flag)                   --计算可移动步数

    ---设置步数初值，和不能通过的地方
    for i=0,CC.WarWidth-1 do
        for j=0,CC.WarHeight-1 do
            local off=(j*CC.WarWidth+i)*2;
            local v=255;                    --设置一个比较大的值,大于最大可能步数64+64=128
            if Byte.get16(WAR.Map1,off)>0 then
                v=1000;                     --此值表示有物体
            end
            if flag==0 then
				if CC.WarWater[Byte.get16(WAR.Map0,off)]~=nil then          --水面，不可走
					v=1000;
				end
                if Byte.get16(WAR.Map2,off)>=0 then    --有人
                    v=1000;
                end
            end
            Byte.set16(WAR.Map3,off,v);
        end
    end

    local x=WAR.Person[id]["坐标X"];
    local y=WAR.Person[id]["坐标Y"];

    Byte.set16(WAR.Map3,2*(y*CC.WarWidth+x),0);          --当前位置为0步

    local c_step=0;             --当前处理的步数

    while c_step<stepmax do
        local add=0;            --是否有新的步数
        for i=0,CC.WarWidth-1 do
            for j=0,CC.WarHeight-1 do
                if Byte.get16(WAR.Map3,(j*CC.WarWidth+i)*2)==c_step then    --当前步数
                    if i+1<CC.WarWidth then                        --当前步数的相邻格
                        local v=Byte.get16(WAR.Map3,(j*CC.WarWidth+i+1)*2);
                        if v ~= 1000 and v > c_step then
                            Byte.set16(WAR.Map3,(j*CC.WarWidth+i+1)*2,c_step+1);
                            add=1;
                        end
                    end
                    if i-1>0 then                        --当前步数的相邻格
                        local v=Byte.get16(WAR.Map3,(j*CC.WarWidth+i-1)*2);
                        if v ~= 1000 and v > c_step then
                            Byte.set16(WAR.Map3,(j*CC.WarWidth+i-1)*2,c_step+1);
                            add=1;
                        end
                    end
                    if j+1<CC.WarHeight then                        --当前步数的相邻格
                        local v=Byte.get16(WAR.Map3,((j+1)*CC.WarWidth+i)*2);
                        if v ~= 1000 and v > c_step then
                            Byte.set16(WAR.Map3,((j+1)*CC.WarWidth+i)*2,c_step+1);
                            add=1;
                        end
                    end
                    if j-1>0 then                        --当前步数的相邻格
                        local v=Byte.get16(WAR.Map3,((j-1)*CC.WarWidth+i)*2);
                        if v ~= 1000 and v > c_step then
                            Byte.set16(WAR.Map3,((j-1)*CC.WarWidth+i)*2,c_step+1);
                            add=1;
                        end
                    end
                end
            end
        end

        if add==0 then
            break;
        end

        c_step=c_step+1;

    end
end


function War_SelectMove()              ---选择移动位置
    local x0=WAR.Person[WAR.CurID]["坐标X"];
    local y0=WAR.Person[WAR.CurID]["坐标Y"];
    local x=x0;
    local y=y0;

    while true do
        local x2=x;
        local y2=y;

        WarDrawMap(1,x,y);
        --WarShowHead(WAR.CurID);
        ShowScreen();
        local key=WaitKey();
        if key==VK_UP then
            y2=y-1;
        elseif key==VK_DOWN then
            y2=y+1;
        elseif key==VK_LEFT then
            x2=x-1;
        elseif key==VK_RIGHT then
            x2=x+1;
        elseif key==VK_SPACE or key==VK_RETURN then
            return x,y;
        elseif key==VK_ESCAPE then
            return nil;
        end

        if Byte.get16(WAR.Map3,(y2*CC.WarWidth+x2)*2)<128 then
            x=x2;
            y=y2;
        end
    end

end


function War_MovePerson(x,y)            --移动人物到位置x,y

    local movenum=Byte.get16(WAR.Map3,(y*CC.WarWidth+x)*2)        --移动位置的步数
    WAR.Person[WAR.CurID]["移动步数"]=WAR.Person[WAR.CurID]["移动步数"]-movenum;    --可移动步数减小

    local movetable={};  --   记录每步移动
    for i=movenum,1,-1 do    --从目的位置反着找到初始位置，作为移动的次序
        movetable[i]={};
        movetable[i].x=x;
        movetable[i].y=y;
        if Byte.get16(WAR.Map3,(y*CC.WarWidth+x-1)*2)==i-1 then
            x=x-1;
            movetable[i].direct=1;
        elseif Byte.get16(WAR.Map3,(y*CC.WarWidth+x+1)*2)==i-1 then
            x=x+1;
            movetable[i].direct=3;
        elseif Byte.get16(WAR.Map3,((y-1)*CC.WarWidth+x)*2)==i-1 then
            y=y-1;
            movetable[i].direct=2;
        elseif Byte.get16(WAR.Map3,((y+1)*CC.WarWidth+x)*2)==i-1 then
            y=y+1;
            movetable[i].direct=0;
        end
    end

    for i=1,movenum do
        local t1=lib.GetTime();
        WAR.Person[WAR.CurID]["坐标X"]=movetable[i].x;
        WAR.Person[WAR.CurID]["坐标Y"]=movetable[i].y;
        WAR.Person[WAR.CurID]["人方向"]=movetable[i].direct;
        WAR.Person[WAR.CurID]["贴图"]=WarCalPersonPic(WAR.CurID);
        WarSetPerson();

		Cls();
        ShowScreen();
        local t2=lib.GetTime();
        if (t2-t1)< 2*CC.Frame then
            lib.Delay(2*CC.Frame-(t2-t1));
        end
    end
end



function War_FightMenu()              --执行攻击菜单
    local pid=WAR.Person[WAR.CurID]["人物编号"];

    local numwugong=0;
    local menu={};
    for i=1,10 do
        local tmp=JY.Person[pid]["武功" .. i];
        if tmp>0 then
            menu[i]={JY.Wugong[tmp]["名称"],nil,1};
            if JY.Wugong[tmp]["消耗内力点数"] > JY.Person[pid]["内力"] then
                menu[i][3]=0;
            end
            numwugong=numwugong+1;
        end
    end

    if numwugong==0 then
        return 0;
    end
    local r;
    if numwugong==1 then
        r=1;
    else
        r=ShowMenu(menu,numwugong,0,80,10,0,0,1,1,24,C_ORANGE,C_WHITE);
    end
    if r==0 then
        return 0;
    end

    return War_Fight_Sub(WAR.CurID,r);
end

--执行战斗，自动和手动战斗都调用
function War_Fight_Sub(id,wugongnum,x,y)          --执行战斗
    local pid=WAR.Person[id]["人物编号"];
    local wugong=JY.Person[pid]["武功" .. wugongnum];
    local level=math.modf(JY.Person[pid]["武功等级" .. wugongnum]/100)+1;

    lib.PicLoadFile(string.format(CC.FightPicFile,JY.Person[pid]["头像代号"]),4);

    for i=0,CC.WarWidth*CC.WarHeight-1 do    --清零
        Byte.set16(WAR.Map4,i*2,0);
    end

    local fightscope=JY.Wugong[wugong]["攻击范围"];
    if fightscope==0 then
        if War_FightSelectType0(wugong,level,x,y)==false then
            return 0;
        end
    elseif fightscope==1 then
        War_FightSelectType1(wugong,level,x,y)

    elseif fightscope==2 then
        War_FightSelectType2(wugong,level,x,y)

    elseif fightscope==3 then
        if War_FightSelectType3(wugong,level,x,y)==false then
            return 0;
        end
    end

    local fightnum=1;
    if JY.Person[pid]["左右互搏"]==1 then
        fightnum=2;
    end
for k=1,fightnum  do         --如果左右互搏，则攻击两次
    for i=0,CC.WarWidth-1 do
        for j=0,CC.WarHeight-1 do
            local offset=(j*CC.WarWidth+i)*2;
            local effect=Byte.get16(WAR.Map4,offset);
            if effect>0 then              --攻击效果地方
                local emeny=Byte.get16(WAR.Map2,offset);
                if emeny>=0 then          --有人
                    if WAR.Person[WAR.CurID]["我方"] ~= WAR.Person[emeny]["我方"] then       --是敌人
                        if JY.Wugong[wugong]["伤害类型"]==0 then             --杀生命
                            WAR.Person[emeny]["点数"]=-War_WugongHurtLife(emeny,wugong,level)
                            Byte.set16(WAR.Map4,offset,2)
                        else
                            WAR.Person[emeny]["点数"]=-War_WugongHurtNeili(emeny,wugong,level)
                            Byte.set16(WAR.Map4,offset,3)
                        end
                     end
                 end
             end
         end
    end

    War_ShowFight(pid,wugong,JY.Wugong[wugong]["武功类型"],JY.Wugong[wugong]["武功动画&音效"]);

    for i=0,WAR.PersonNum-1 do
        WAR.Person[i]["点数"]=0;
    end

    WAR.Person[WAR.CurID]["经验"]=WAR.Person[WAR.CurID]["经验"]+2;

    if JY.Person[pid]["武功等级" .. wugongnum]<900 then
        JY.Person[pid]["武功等级" .. wugongnum]=JY.Person[pid]["武功等级" .. wugongnum]+Rnd(2)+1;
    end

    if math.modf(JY.Person[pid]["武功等级" .. wugongnum]/100)+1 ~= level then    --武功升级了
        level=math.modf(JY.Person[pid]["武功等级" .. wugongnum]/100)+1;
        DrawStrBox(-1,-1,string.format("%s 升为 %d 级",JY.Wugong[wugong]["名称"],level),C_ORANGE,24)
        ShowScreen();
        lib.Delay(500);
        Cls();
        ShowScreen();
    end

    AddPersonAttrib(pid,"内力",-math.modf(level/2)*JY.Wugong[wugong]["消耗内力点数"])

end

    AddPersonAttrib(pid,"体力",-3);

    return 1;

end

--选择点攻击
--x1,y1 攻击点，如果为空则手工选择
function War_FightSelectType0(wugong,level,x1,y1)          --选择点攻击
    local x0=WAR.Person[WAR.CurID]["坐标X"];
    local y0=WAR.Person[WAR.CurID]["坐标Y"];
    War_CalMoveStep(WAR.CurID,JY.Wugong[wugong]["移动范围" .. level],1);

    if x1==nil and y1==nil then
        x1,y1=War_SelectMove();              --选择攻击对象
    end
    if x1 ==nil then
        lib.WaitKey();
        --WarDrawMap(0);
        --ShowScreen()
		Cls();
        return false;
    end

    WAR.Person[WAR.CurID]["人方向"]=War_Direct(x0,y0,x1,y1);

    Byte.set16(WAR.Map4,(y1*CC.WarWidth+x1)*2,1);

end

--选择线攻击
--direct 攻击方向，为空则手工设置
function War_FightSelectType1(wugong,level,x,y)            --选择线攻击
    local x0=WAR.Person[WAR.CurID]["坐标X"];
    local y0=WAR.Person[WAR.CurID]["坐标Y"];
    local direct;

    if x==nil and y==nil  then
        direct =-1;
        DrawStrBox(80,10,"请选择攻击方向",C_ORANGE,24)
        ShowScreen();

        while true do           --选择方向
            local key=WaitKey();
            if key==VK_UP then
                direct=0;
            elseif key==VK_DOWN then
                direct=3;
            elseif key==VK_LEFT then
                direct=2;
            elseif key==VK_RIGHT then
                direct=1;
            end
            if direct>=0 then
                break;
            end
        end

        Cls(80,10,CC.ScreenW,CC.ScreenH);
        ShowScreen();
    else
        direct=War_Direct(x0,y0,x,y);
    end

    WAR.Person[WAR.CurID]["人方向"]=direct;
    local move=JY.Wugong[wugong]["移动范围" .. level]

    for i=1,move do
        if direct==0 then
            Byte.set16(WAR.Map4,((y0-i)*CC.WarWidth+x0)*2,1);
        elseif direct==3 then
            Byte.set16(WAR.Map4,((y0+i)*CC.WarWidth+x0)*2,1);
        elseif direct==2 then
            Byte.set16(WAR.Map4,((y0)*CC.WarWidth+(x0-i))*2,1);
        elseif direct==1 then
            Byte.set16(WAR.Map4,((y0)*CC.WarWidth+(x0+i))*2,1);
        end
    end



end

--选择十字攻击
function War_FightSelectType2(wugong,level)                 --选择十字攻击
    local x0=WAR.Person[WAR.CurID]["坐标X"];
    local y0=WAR.Person[WAR.CurID]["坐标Y"];

    local move=JY.Wugong[wugong]["移动范围" .. level]

    for i=1,move do
        Byte.set16(WAR.Map4,((y0-i)*CC.WarWidth+x0)*2,1);
        Byte.set16(WAR.Map4,((y0+i)*CC.WarWidth+x0)*2,1);
        Byte.set16(WAR.Map4,((y0)*CC.WarWidth+(x0-i))*2,1);
        Byte.set16(WAR.Map4,((y0)*CC.WarWidth+(x0+i))*2,1);
    end
end

--选择面攻击
--x1,y1 攻击点，如果为空则手工选择
function War_FightSelectType3(wugong,level,x1,y1)            --选择面攻击
    local x0=WAR.Person[WAR.CurID]["坐标X"];
    local y0=WAR.Person[WAR.CurID]["坐标Y"];
    War_CalMoveStep(WAR.CurID,JY.Wugong[wugong]["移动范围" .. level],1);

    if x1==nil and y1==nil then
        x1,y1=War_SelectMove();              --选择攻击对象
    end

    if x1 ==nil then
        lib.WaitKey();
        --WarDrawMap(0);
        --ShowScreen();
		Cls();
        return false;
    end

    WAR.Person[WAR.CurID]["人方向"]=War_Direct(x0,y0,x1,y1);

    local move=JY.Wugong[wugong]["杀伤范围" .. level]

    for i=-move,move do
        for j=-move,move do
            Byte.set16(WAR.Map4,((y1+j)*CC.WarWidth+(x1+i))*2,1);
        end
    end
end

--计算人方向
--(x1,y1) 人位置     -(x2,y2) 目标位置
--返回： 方向
function War_Direct(x1,y1,x2,y2)             --计算人方向
    local x=x2-x1;
    local y=y2-y1;

    if math.abs(y)>math.abs(x) then
        if y>0 then
            return 3;
        else
            return 0
        end
    else
        if x>0 then
            return 1;
        else
            return 2;
        end
    end
end


--显示战斗动画
--pid 人id
--wugong  武功编号， 0 表示用毒解毒等，使用普通攻击效果
--wogongtype =0 医疗用毒解毒，1,2,3,4 武功类型  -1 暗器
--eft  武功动画效果id  eft.idx/grp中的效果

function War_ShowFight(pid,wugong,wugongtype,eft)              --显示战斗动画

    local fightdelay,fightframe,sounddelay;
    if wugongtype>=0 then
        fightdelay=JY.Person[pid]["出招动画延迟" .. wugongtype+1];
        fightframe=JY.Person[pid]["出招动画帧数" .. wugongtype+1];
        sounddelay=JY.Person[pid]["武功音效延迟" .. wugongtype+1];
    else            ---暗器，这些数据什么意思？？
        fightdelay=0;
        fightframe=-1;
        sounddelay=-1;
    end

    local framenum=fightdelay+CC.Effect[eft];            --总帧数

    local startframe=0;               --计算fignt***中当前出招起始帧
    if wugongtype>=0 then
        for i=0,wugongtype-1 do
            startframe=startframe+4*JY.Person[pid]["出招动画帧数" .. i+1];
        end
    end

    local starteft=0;          --计算起始武功效果帧
    for i=0,eft-1 do
        starteft=starteft+CC.Effect[i];
    end

    for i=0,framenum-1 do
        local t1=lib.GetTime();
        if fightframe>=0 then
            WAR.Person[WAR.CurID]["贴图类型"]=1;
            if i<fightframe then
                WAR.Person[WAR.CurID]["贴图"]=(startframe+WAR.Person[WAR.CurID]["人方向"]*fightframe+i)*2;
            end
        else       --暗器，不使用fight中图像
            WAR.Person[WAR.CurID]["贴图类型"]=0;
            WAR.Person[WAR.CurID]["贴图"]=WarCalPersonPic(WAR.CurID);
        end

        if i==sounddelay then
            PlayWavAtk(JY.Wugong[wugong]["出招音效"]);
        end
        if i==fightdelay then
            PlayWavE(eft);
        end

        WarDrawMap(0);
        if i>=fightdelay then           --显示武功效果
            starteft=starteft+1;            --此处似乎是eft第一个数据有问题，应该是10，现为9，因此加1
            WarDrawEffect(starteft*2);
        end
        ShowScreen();
		local t2=lib.GetTime();
    	if t2-t1<CC.Frame then
            lib.Delay(CC.Frame-(t2-t1));
	    end

    end

    WAR.Person[WAR.CurID]["贴图类型"]=0;
    WAR.Person[WAR.CurID]["贴图"]=WarCalPersonPic(WAR.CurID);
    WarDrawMap(0);
    ShowScreen();
    lib.Delay(200);

    WarDrawMap(2);          --全黑显示命中人物
    ShowScreen();
    lib.Delay(200);

    for i=1,20 do           --显示命中的点数
	    local t1=lib.GetTime();
        WarDrawMap(3,i*2);
        ShowScreen();
	    local t2=lib.GetTime();
		if (t2-t1)<15 then
	        lib.Delay(15-(t2-t1));
		end
    end

    WarDrawMap(0);
    ShowScreen();
end

--武功伤害生命
--enemyid 敌人战斗id，
--wugong  我方使用武功
--返回：伤害点数
function War_WugongHurtLife(emenyid,wugong,level)             --计算武功伤害生命
    local pid=WAR.Person[WAR.CurID]["人物编号"];
    local eid=WAR.Person[emenyid]["人物编号"];

    --计算武学常识
    local mywuxue=0;
    local emenywuxue=0;
    for i=0,WAR.PersonNum-1 do
        local id =WAR.Person[i]["人物编号"]
        if WAR.Person[i]["死亡"]==false and JY.Person[id]["武学常识"]>80 then
            if WAR.Person[WAR.CurID]["我方"]==WAR.Person[i]["我方"] then
                mywuxue=mywuxue+JY.Person[id]["武学常识"];
            else
                emenywuxue=emenywuxue+JY.Person[id]["武学常识"];
            end
        end
    end

    --计算实际使用武功等级
    while true do
        if math.modf((level+1)/2)*JY.Wugong[wugong]["消耗内力点数"] > JY.Person[pid]["内力"] then
            level=level-1;
        else
            break;
        end
    end

    if level<=0 then     --防止出现左右互博时第一次攻击完毕，第二次攻击没有内力的情况。
	    level=1;
    end

    --武功武器配合增加攻击力
    local fightnum=0;
    for i,v in ipairs(CC.ExtraOffense) do
        if v[1]==JY.Person[pid]["武器"] and v[2]==wugong then
            fightnum=v[3];
            break;
        end
    end

    --计算攻击力
    fightnum=fightnum+(JY.Person[pid]["攻击力"]*3+JY.Wugong[wugong]["攻击力" .. level ])/2;

    if JY.Person[pid]["武器"]>=0 then
        fightnum=fightnum+JY.Thing[JY.Person[pid]["武器"]]["加攻击力"];
    end
    if JY.Person[pid]["防具"]>=0 then
        fightnum=fightnum+JY.Thing[JY.Person[pid]["防具"]]["加攻击力"];
    end
    fightnum=fightnum+mywuxue;

    --计算防御力
    local defencenum=JY.Person[eid]["防御力"];
    if JY.Person[eid]["武器"]>=0 then
        defencenum=defencenum+JY.Thing[JY.Person[eid]["武器"]]["加防御力"];
    end
    if JY.Person[eid]["防具"]>=0 then
        defencenum=defencenum+JY.Thing[JY.Person[eid]["防具"]]["加防御力"];
    end
    defencenum= defencenum+ emenywuxue;

    --计算实际伤害
    local hurt=(fightnum-3*defencenum)*2/3+Rnd(20)-Rnd(20);
    if hurt <0 then
        hurt=Rnd(10)+1;
    end
    hurt=hurt+JY.Person[pid]["体力"]/15+JY.Person[eid]["受伤程度"]/20;

    --考虑距离因素
    local offset=math.abs(WAR.Person[WAR.CurID]["坐标X"]-WAR.Person[emenyid]["坐标X"])+
                 math.abs(WAR.Person[WAR.CurID]["坐标Y"]-WAR.Person[emenyid]["坐标Y"]);

    if offset <10 then
        hurt=hurt*(100-(offset-1)*3)/100;
    else
        hurt=hurt*2/3;
    end

    hurt=math.modf(hurt);
    if hurt<=0 then
        hurt=Rnd(8)+1;
    end

    JY.Person[eid]["生命"]=JY.Person[eid]["生命"]-hurt;
    WAR.Person[WAR.CurID]["经验"]=WAR.Person[WAR.CurID]["经验"]+math.modf(hurt/5);

    if JY.Person[eid]["生命"]<0 then                 --打死敌人获得额外经验
        JY.Person[eid]["生命"]=0;
        WAR.Person[WAR.CurID]["经验"]=WAR.Person[WAR.CurID]["经验"]+JY.Person[eid]["等级"]*10;
    end

    AddPersonAttrib(eid,"受伤程度",math.modf(hurt/10));

    --敌人中毒点数
    local poisonnum=level*JY.Wugong[wugong]["敌人中毒点数"]+JY.Person[pid]["攻击带毒"];

    if JY.Person[eid]["抗毒能力"]< poisonnum and JY.Person[eid]["抗毒能力"]<90 then
         AddPersonAttrib(eid,"中毒程度",math.modf(poisonnum/15));
    end

    return hurt;
end

--武功伤害内力
--enemyid 敌人战斗id，
--wugong  我方使用武功
--返回：伤害点数
function War_WugongHurtNeili(enemyid,wugong,level)           --计算武功伤害内力
   local pid=WAR.Person[WAR.CurID]["人物编号"];
   local eid=WAR.Person[enemyid]["人物编号"];

   local addvalue=JY.Wugong[wugong]["加内力" .. level];
   local decvalue=JY.Wugong[wugong]["杀内力" .. level];

   AddPersonAttrib(pid,"内力最大值",Rnd(math.modf(addvalue/2)));
   AddPersonAttrib(pid,"内力",addvalue+Rnd(3)-Rnd(3));
   return -AddPersonAttrib(eid,"内力",-(decvalue+Rnd(3)-Rnd(3)));
end

---用毒菜单
function War_PoisonMenu()              ---用毒菜单
    return War_ExecuteMenu(1);
end

--计算敌人中毒点数
--pid 使毒人，
--emenyid  中毒人
function War_PoisonHurt(pid,emenyid)     --计算敌人中毒点数
    local v=math.modf((JY.Person[pid]["用毒能力"]-JY.Person[emenyid]["抗毒能力"])/4);
    if v<0 then
        v=0;
    end
    return AddPersonAttrib(emenyid,"中毒程度",v);
end



---解毒菜单
function War_DecPoisonMenu()          ---解毒菜单
    return  War_ExecuteMenu(2);
end




---医疗菜单
function War_DoctorMenu()            ---医疗菜单
    return  War_ExecuteMenu(3);
end


---执行医疗，解毒用毒
---flag=1 用毒， 2 解毒，3 医疗 4 暗器
---thingid 暗器物品id
function War_ExecuteMenu(flag,thingid)            ---执行医疗，解毒用毒暗器
    local pid=WAR.Person[WAR.CurID]["人物编号"];


    for i=0,CC.WarWidth*CC.WarHeight-1 do    --清零
        Byte.set16(WAR.Map4,i*2,0);
    end

    local step;
    if flag==1 then
        step=math.modf(JY.Person[pid]["用毒能力"]/15)+1;         --用毒步数
    elseif flag==2 then
        step=math.modf(JY.Person[pid]["解毒能力"]/15)+1;         --解毒步数
    elseif flag==3 then
        step=math.modf(JY.Person[pid]["医疗能力"]/15)+1;         --医疗步数
    elseif flag==4 then
        step=math.modf(JY.Person[pid]["暗器技巧"]/15)+1;         --暗器步数
    end

    War_CalMoveStep(WAR.CurID,step,1);

    local x1,y1=War_SelectMove();              --选择攻击对象

    if x1 ==nil then
        lib.WaitKey();
		Cls();
        return 0;
    else
        return War_ExecuteMenu_Sub(x1,y1,flag,thingid);
    end
end



function War_ExecuteMenu_Sub(x1,y1,flag,thingid)     ---执行医疗，解毒用毒暗器的子函数，自动医疗也可调用
    local pid=WAR.Person[WAR.CurID]["人物编号"];
    local x0=WAR.Person[WAR.CurID]["坐标X"];
    local y0=WAR.Person[WAR.CurID]["坐标Y"];

    lib.PicLoadFile(string.format(CC.FightPicFile,JY.Person[pid]["头像代号"]),4);

	WAR.Person[WAR.CurID]["人方向"]=War_Direct(x0,y0,x1,y1);
	local offset=(y1*CC.WarWidth+x1)*2;
	Byte.set16(WAR.Map4,offset,1);


	local emeny=Byte.get16(WAR.Map2,offset);
	if emeny>=0 then          --有人
		 if flag==1 then
			 if WAR.Person[WAR.CurID]["我方"] ~= WAR.Person[emeny]["我方"] then       --是敌人
				 WAR.Person[emeny]["点数"]=War_PoisonHurt(pid,WAR.Person[emeny]["人物编号"])
				 Byte.set16(WAR.Map4,offset,5);
			 end
		 elseif flag==2 then
			 if WAR.Person[WAR.CurID]["我方"] == WAR.Person[emeny]["我方"] then       --不是敌人
				 WAR.Person[emeny]["点数"]=ExecDecPoison(pid,WAR.Person[emeny]["人物编号"])
				 Byte.set16(WAR.Map4,offset,6);
			 end
		 elseif flag==3 then
			 if WAR.Person[WAR.CurID]["我方"] == WAR.Person[emeny]["我方"] then       --不是敌人
				 WAR.Person[emeny]["点数"]=ExecDoctor(pid,WAR.Person[emeny]["人物编号"])
				 Byte.set16(WAR.Map4,offset,4);
			 end
		 elseif flag==4 then
			 if WAR.Person[WAR.CurID]["我方"] ~= WAR.Person[emeny]["我方"] then       --是敌人
				 WAR.Person[emeny]["点数"]=War_AnqiHurt(pid,WAR.Person[emeny]["人物编号"],thingid)
				 Byte.set16(WAR.Map4,offset,2);
			 end
		 end

	end

	if flag==1 then
		War_ShowFight(pid,0,0,30);
	elseif flag==2 then
		War_ShowFight(pid,0,0,36);
	elseif flag==3 then
		War_ShowFight(pid,0,0,0);
	elseif flag==4 then
		if emeny>=0 then
			War_ShowFight(pid,0,-1,JY.Thing[thingid]["暗器动画编号"]);
		end
	end


	for i=0,WAR.PersonNum-1 do
		WAR.Person[i]["点数"]=0;
	end
	if flag==4 then
		if emeny>=0 then
			instruct_32(thingid,-1);            --物品数量减少
			return 1;
		else
			return 0;                   --暗器打空，则等于没有打
		end
	else
		WAR.Person[WAR.CurID]["经验"]=WAR.Person[WAR.CurID]["经验"]+1;
		AddPersonAttrib(pid,"体力",-2);
	end

	return 1;

end



--物品菜单
function War_ThingMenu()            --战斗物品菜单
    WAR.ShowHead=0;
    local thing={};
    local thingnum={};

    for i = 0,CC.MyThingNum-1 do
        thing[i]=-1;
        thingnum[i]=0;
    end

    local num=0;
    for i = 0,CC.MyThingNum-1 do
        local id = JY.Base["物品" .. i+1];
        if id>=0 then
            if JY.Thing[id]["类型"]==3 or JY.Thing[id]["类型"]==4 then
                thing[num]=id;
                thingnum[num]=JY.Base["物品数量" ..i+1];
                num=num+1;
            end
        end
    end

    local r=SelectThing(thing,thingnum);
	WAR.ShowHead=1;
	Cls();
    if r>=0 then
        if UseThing(r)==1 then
		    return 1;
		else
		    return 0;
		end
    else
        return 0;
    end

end


---使用暗器
function War_UseAnqi(id)          ---战斗使用暗器

    return War_ExecuteMenu(4,id);


end



function War_AnqiHurt(pid,emenyid,thingid)         --计算暗器伤害
    local num;
    if JY.Person[emenyid]["受伤程度"]==0 then
        num=JY.Thing[thingid]["加生命"]/4-Rnd(5);
    elseif JY.Person[emenyid]["受伤程度"]<=33 then
        num=JY.Thing[thingid]["加生命"]/3-Rnd(5);
    elseif JY.Person[emenyid]["受伤程度"]<=66 then
        num=JY.Thing[thingid]["加生命"]/2-Rnd(5);
    else
        num=JY.Thing[thingid]["加生命"]/2-Rnd(5);
    end

    num=math.modf((num-JY.Person[pid]["暗器技巧"]*2)/3);
    AddPersonAttrib(emenyid,"受伤程度",math.modf(-num/4));      --此处的num为负值

    local r=AddPersonAttrib(emenyid,"生命",math.modf(num));

    if JY.Thing[thingid]["加中毒解毒"]>0 then
        num=math.modf((JY.Thing[thingid]["加中毒解毒"]+JY.Person[pid]["暗器技巧"])/2);
        num=num-JY.Person[emenyid]["抗毒能力"];
        num=limitX(num,0,CC.PersonAttribMax["用毒能力"]);
        AddPersonAttrib(emenyid,"中毒程度",num);
    end
    return r;
end

--休息
function War_RestMenu()           --休息
    local pid=WAR.Person[WAR.CurID]["人物编号"];
    local v=3+Rnd(3);
    AddPersonAttrib(pid,"体力",v);
    if JY.Person[pid]["体力"]>30 then
        v=3+Rnd(math.modf(JY.Person[pid]["体力"]/10)-2);
        AddPersonAttrib(pid,"生命",v);
        v=3+Rnd(math.modf(JY.Person[pid]["体力"]/10)-2);
        AddPersonAttrib(pid,"内力",v);
    end
    return 1;
end


--等待，把当前战斗人调到队尾
function War_WaitMenu()            --等待，把当前战斗人调到队尾

    for i =WAR.CurID, WAR.PersonNum-2 do
        local tmp=WAR.Person[i+1];
        WAR.Person[i+1]=WAR.Person[i];
        WAR.Person[i]=tmp;
        --调换人物
    end

    WarSetPerson();     --设置战斗人物位置

    for i=0,WAR.PersonNum-1 do
        WAR.Person[i]["贴图"]=WarCalPersonPic(i);
    end

    return 1;

end



function War_StatusMenu()          --战斗中显示状态
    WAR.ShowHead=0;
	Menu_Status();
	WAR.ShowHead=1;
	Cls();
end

function War_AutoMenu()           --设置自动战斗
    WAR.AutoFight=1;
    War_Auto();
    return 1;
end

------------------------------------------------------------------------------------
-----------------------------------自动战斗------------------------------------------



function War_Auto()             --自动战斗主函数
    lib.Delay(300);

    local autotype=War_Think();         --思考如何战斗

    if autotype==0 then  --休息
        War_AutoEscape();  --先跑开
        War_RestMenu();
    elseif autotype==1 then
        War_AutoFight();      --自动战斗
    elseif autotype==2 then    --吃药加生命
        War_AutoEscape();
        War_AutoEatDrug(2);
    elseif autotype==3 then    --吃药加内力
        War_AutoEscape();
         War_AutoEatDrug(3);
    elseif autotype==4 then    --吃药加体力
        War_AutoEscape();
        War_AutoEatDrug(4);
    elseif autotype==5 then    --自己医疗
        War_AutoEscape();
        War_AutoDoctor();
    end

    return 0;
end

--思考如何战斗
--返回：0 休息， 1 战斗，2 使用物品增加生命， 3 使用物品增加内力 4 吃药加体力， 5 医疗

function War_Think()           --思考如何战斗
    local pid=WAR.Person[WAR.CurID]["人物编号"];
    local r=-1;         --考虑的结果

    if JY.Person[pid]["体力"] <10 then         --休息
        r=War_ThinkDrug(4);
        if r>=0 then
            return r;
        end
        return 0;
    end

    if JY.Person[pid]["生命"]<20 or JY.Person[pid]["受伤程度"]>50 then
        r=War_ThinkDrug(2);       --考虑增加生命
        if r>=0 then
            return r;
        end
    end

    local rate=-1;         --增加生命的百分比
    if JY.Person[pid]["生命"] <JY.Person[pid]["生命最大值"] /5 then
        rate=90;
    elseif JY.Person[pid]["生命"] <JY.Person[pid]["生命最大值"] /4 then
        rate=70;
    elseif JY.Person[pid]["生命"] <JY.Person[pid]["生命最大值"] /3 then
        rate=50;
    elseif JY.Person[pid]["生命"] <JY.Person[pid]["生命最大值"] /2 then
        rate=25;
    end

    if Rnd(100)<rate then
        r=War_ThinkDrug(2);       --考虑增加生命
        if r>=0 then
            return r;
        else             --没有增加生命的药，考虑是否自己医疗
		    r=War_ThinkDoctor();
		    if r>=0 then
		       return r;
		    end
        end
    end

    rate=-1;         --增加内力的百分比
    if JY.Person[pid]["内力"] <JY.Person[pid]["内力最大值"] /5 then
        rate=75;
    elseif JY.Person[pid]["内力"] <JY.Person[pid]["内力最大值"] /4 then
        rate=50;
    end

    if Rnd(100)<rate then
        r=War_ThinkDrug(3);       --考虑增加内力
        if r>=0 then
            return r;
        end
    end

    local minNeili=math.huge;          --所有武功的最小内力

    for i =1,10 do
        if JY.Wugong[JY.Person[pid]["武功" .. i]]["消耗内力点数"] <minNeili then
            minNeili=JY.Wugong[JY.Person[pid]["武功" .. i]]["消耗内力点数"] ;
        end
    end

    if JY.Person[pid]["内力"]>=minNeili then
        r=1;
    else
        r=0;
    end

    return r;
end

--能否吃药增加参数
--flag=2 生命，3内力；4体力
function War_ThinkDrug(flag)             --能否吃药增加参数
    local pid=WAR.Person[WAR.CurID]["人物编号"];
    local str;
    local r=-1;

    if flag==2 then
        str="加生命";
    elseif flag==3 then
        str="加内力";
    elseif flag==4 then
        str="加体力";
    else
        return r;
    end

    if WAR.Person[WAR.CurID]["我方"]==true then
        for i =1, CC.MyThingNum do
            local thingid=JY.Base["物品" ..i];
            if thingid>=0 then
                if JY.Thing[thingid]["类型"]==3 and JY.Thing[thingid][str]>0 then
                    r=flag;                     ---有增加生命的药，则动作：使用物品加生命
                    break;
                end
            end
        end
    else
        for i =1, 4 do
            local thingid=JY.Person[pid]["携带物品" ..i];
            if thingid>=0 then
                if JY.Thing[thingid]["类型"]==3 and JY.Thing[thingid][str]>0 then
                    r=flag;                     ---有增加生命的药，则动作：使用物品加生命
                    break;
                end
            end
        end
    end

    return r;

end


--考虑是否自己医疗
function War_ThinkDoctor()          --考虑是否给自己医疗
    local pid=WAR.Person[WAR.CurID]["人物编号"];

	if JY.Person[pid]["体力"]<50 or JY.Person[pid]["医疗能力"]<20 then
	    return -1;
	end

	if JY.Person[pid]["受伤程度"]>JY.Person[pid]["医疗能力"]+20 then
	    return -1;
	end

	local rate = -1;
	local v=JY.Person[pid]["生命最大值"]-JY.Person[pid]["生命"];
	if JY.Person[pid]["医疗能力"] < v/4 then
        rate=30;
	elseif JY.Person[pid]["医疗能力"] < v/3 then
	    rate=50;
	elseif JY.Person[pid]["医疗能力"] < v/2 then
	    rate=70;
	else
	    rate=90;
	end

	if Rnd(100) <rate then
	    return 5;
	end

	return -1;
end

---自动战斗
function War_AutoFight()             ---执行自动战斗
    local pid=WAR.Person[WAR.CurID]["人物编号"];

    --选择武功

    local wugongnum=War_AutoSelectWugong();



	local r=War_AutoMove(wugongnum);         -- 往敌人方向移动
	if r==1 then   --如果在攻击范围
		War_AutoExecuteFight(wugongnum);     --攻击
	else
   	    War_RestMenu();           --休息
	end
end



function War_AutoSelectWugong()           --自动选择合适的武功
    local pid=WAR.Person[WAR.CurID]["人物编号"];

    local probability={};       --每种武功选择的概率

    local wugongnum=0;
	for i =1, 10 do             --计算每种可选择武功的总攻击力
        local wugongid=JY.Person[pid]["武功" .. i];
        if wugongid>0 then
		       --选择杀生命的武功，暂不考虑杀内力的。必须消耗内力比现有内力小，起码可以发出一级的武功。
		    if JY.Wugong[wugongid]["消耗内力点数"]<=JY.Person[pid]["内力"] and JY.Wugong[wugongid]["伤害类型"]==0 then
				local level=math.modf(JY.Person[pid]["武功等级" .. i]/100)+1;
				--总攻击力即为概率
                probability[i]=(JY.Person[pid]["攻击力"]*3+JY.Wugong[wugongid]["攻击力" .. level ])/2;
            else
			    probability[i]=0;
			end
		else
		    wugongnum=i-1;
			break;
        end
    end

    local maxoffense=0;       --计算最大攻击力
	for i =1, wugongnum do
        if  probability[i]>maxoffense then
            maxoffense=probability[i];
        end
    end

    local mynum=0;             --计算我方和敌人个数
	local enemynum=0;
	for i=0, WAR.PersonNum-1 do
	    if WAR.Person[i]["死亡"]==false then
		    if WAR.Person[i]["我方"]==WAR.Person[WAR.CurID]["我方"] then
			    mynum=mynum+1;
			else
			    enemynum=enemynum+1;
			end
		end
	end

    local factor=0;       --敌人人数影响因子，敌人多则对线面等攻击多人武功的选择概率增加
	if enemynum>mynum then
	    factor=2;
	else
	    factor=1;
	end

	for i =1, wugongnum do       --考虑其他概率效果
        local wugongid=JY.Person[pid]["武功" .. i];
        if probability[i]>0 then
		    if probability[i]<maxoffense/2 then       --去掉攻击力小的武功
			    probability[i]=0
			end
			local extranum=0;           --武功武器配合的攻击力
			for j,v in ipairs(CC.ExtraOffense) do
				if v[1]==JY.Person[pid]["武器"] and v[2]==wugongid then
					extranum=v[3];
					break;
				end
			end
    		local level=math.modf(JY.Person[pid]["武功等级" .. i]/100)+1;
			probability[i]=probability[i]+JY.Wugong[wugongid]["攻击范围"]*factor*JY.Wugong[wugongid]["杀伤范围" ..level]*20;
        end
    end

	local s={};           --按照概率依次累加
	local maxnum=0;
    for i=1,wugongnum do
        s[i]=maxnum;
		maxnum=maxnum+probability[i]
	end
	s[wugongnum+1]=maxnum;

    local v=Rnd(maxnum);            --产生随机数
	local selectid=0;
    for i=1,wugongnum do            --根据产生的随机数，寻找落在哪个武功区间
	    if v>=s[i] and v< s[i+1] then
		    selectid=i;
			break;
		end
	end

    return selectid;
end


function War_AutoSelectEnemy()             --选择战斗对手
    local enemyid=War_AutoSelectEnemy_near()
    WAR.Person[WAR.CurID]["自动选择对手"]=enemyid;
    return enemyid;
end


function War_AutoSelectEnemy_near()              --选择最近对手

    War_CalMoveStep(WAR.CurID,100,1);           --标记每个位置的步数

    local maxDest=math.huge;
    local nearid=-1;
    for i=0,WAR.PersonNum-1 do           --查找最近步数的敌人
        if WAR.Person[WAR.CurID]["我方"] ~=WAR.Person[i]["我方"] then
            if WAR.Person[i]["死亡"]==false then
                local step=Byte.get16(WAR.Map3,(WAR.Person[i]["坐标Y"]*CC.WarWidth+WAR.Person[i]["坐标X"])*2);
                if step<maxDest then
                    nearid=i;
                    maxDest=step;
                end
            end
        end
    end
    return nearid;
end

--自动往敌人方向移动
--人物武功编号，不是武功id
--返回 1=可以攻击敌人， 0 不能攻击

----有些慢，先就这样吧！
function War_AutoMove(wugongnum)              --自动往敌人方向移动
    local pid=WAR.Person[WAR.CurID]["人物编号"];
    local wugongid=JY.Person[pid]["武功"  ..wugongnum];
    local level=math.modf(JY.Person[pid]["武功等级".. wugongnum]/100)+1;

    local wugongtype=JY.Wugong[wugongid]["攻击范围"];
    local scope=JY.Wugong[wugongid]["移动范围" ..level]+JY.Wugong[wugongid]["杀伤范围" ..level]


    local x,y;
	local move=128;

    War_CalMoveStep(WAR.CurID,WAR.Person[WAR.CurID]["移动步数"],0);   --计算移动步数

    local maxenemy=0;
	for i=0,CC.WarWidth-1 do
		for j=0,CC.WarHeight-1 do
		    local cur_step=Byte.get16(WAR.Map3,(j*CC.WarWidth+i)*2);
			if cur_step <128 then
				local num=War_AutoCalMaxEnemy(i,j,wugongid,level);       --计算这个位置可以攻击到的最多敌人个数
				if num>maxenemy then
					maxenemy=num
					x=i;
					y=j;
                    move=cur_step;
				elseif num==maxenemy then            --两个位置人数相等，则取移动距离最短
				    if cur_step < move then
						maxenemy=num
						x=i;
						y=j;
						move=cur_step;
					end
				end
			end
		end
	end

    if maxenemy>0 then
        War_MovePerson(x,y);    --移动到相应的位置
		return 1;
	else   --任何移动都直接攻击不到敌人，寻找一条可以移动到攻击到敌人位置的路线
	    x,y=War_GetCanFightEnemyXY(scope);

	    local minDest=math.huge;
        if x==nil then   --无法走到可以攻击敌人的地方，可能敌人被围住，或者被敌人围住。
            local enemyid=War_AutoSelectEnemy()   --选择最近敌人
	        War_CalMoveStep(WAR.CurID,100,0);   --计算移动步数 假设最大100步
            for i=0,CC.WarWidth-1 do
                for j=0,CC.WarHeight-1 do
				    local dest=Byte.get16(WAR.Map3,(j*CC.WarWidth+i)*2);
                    if dest <128 then
                        local dx=math.abs(i-WAR.Person[enemyid]["坐标X"])
                        local dy=math.abs(j-WAR.Person[enemyid]["坐标Y"])
                        if minDest>(dx+dy) then        --此时x,y是距离敌人的最短路径，虽然可能被围住
                            minDest=dx+dy;
                            x=i;
                            y=j;
                        elseif minDest==(dx+dy) then
                            if Rnd(2)==0 then
                                x=i;
                                y=j;
                            end
                        end
                    end
                end
            end
		else
            minDest=0;        --可以走到
		end

		if minDest<math.huge then
		    while true do    --从目的位置反着找到可以移动的位置，作为移动的次序
                local i=Byte.get16(WAR.Map3,(y*CC.WarWidth+x)*2);
                if i<=WAR.Person[WAR.CurID]["移动步数"] then
                    break;
                end
                if Byte.get16(WAR.Map3,(y*CC.WarWidth+x-1)*2)==i-1 then
                    x=x-1;
                elseif Byte.get16(WAR.Map3,(y*CC.WarWidth+x+1)*2)==i-1 then
                    x=x+1;
                elseif Byte.get16(WAR.Map3,((y-1)*CC.WarWidth+x)*2)==i-1 then
                    y=y-1;
                elseif Byte.get16(WAR.Map3,((y+1)*CC.WarWidth+x)*2)==i-1 then
                    y=y+1;
                end
            end
            War_MovePerson(x,y);    --移动到相应的位置
        end
    end

    return 0;
end


--得到可以走到攻击到敌人的最近位置。
--scope可以攻击的范围
--返回 x,y。如果无法走到攻击位置，返回空
function War_GetCanFightEnemyXY(scope)             --得到可以走到攻击到敌人的最近位置
	War_CalMoveStep(WAR.CurID,100,0);   --计算移动步数 假设最大100步
	local minStep=math.huge;
	local x,y;
    for n=0,WAR.PersonNum-1 do
	    if WAR.Person[WAR.CurID]["我方"] ~= WAR.Person[n]["我方"] and WAR.Person[n]["死亡"]==false then
			for i=0,CC.WarWidth-1 do
				for j=0,CC.WarHeight-1 do
					local step=Byte.get16(WAR.Map3,(j*CC.WarWidth+i)*2);
					if step <128 then
						local dx=math.abs(i-WAR.Person[n]["坐标X"]);
						local dy=math.abs(j-WAR.Person[n]["坐标Y"]);
						if minStep>step and (dx+dy)<=scope then
							minStep=step;
							x=i;                             --此时x,y是可以攻击到敌人的最短路径
							y=j;
						 elseif minStep==step and (dx+dy)<=scope then
							 if Rnd(2)==0 then
								x=i;
								y=j;
							end
						end
					end
				end
		 	end
		end
    end
	if minStep<math.huge then
	    return x,y;
    end
end





function War_AutoCalMaxEnemy(x,y,wugongid,level)       --计算从(x,y)开始攻击最多能够击中几个敌人

    local wugongtype=JY.Wugong[wugongid]["攻击范围"];
    local movescope=JY.Wugong[wugongid]["移动范围" ..level];
	local fightscope=JY.Wugong[wugongid]["杀伤范围" ..level];

	local maxnum=0;
	local xmax,ymax;

if wugongtype==0 or wugongtype==3 then
	for i=0,CC.WarWidth-1 do
		for j=0,CC.WarHeight-1 do
			local dx=math.abs(i-x);
			local dy=math.abs(j-y);
			if (dx+dy)<=movescope then  --在武功移动范围内
			    local enemynum=0;
                for n=0,WAR.PersonNum-1 do   --计算武功攻击范围内的敌人个数
                     if n~=WAR.CurID and WAR.Person[n]["死亡"]==false and WAR.Person[n]["我方"] ~=WAR.Person[WAR.CurID]["我方"] then
                         local xx=math.abs(WAR.Person[n]["坐标X"]-i);
                         local yy=math.abs(WAR.Person[n]["坐标Y"]-j);
						 if xx<=fightscope and yy <=fightscope then
						      enemynum=enemynum+1;
						 end
					 end
				end

				if enemynum>maxnum then        --记录最多敌人和位置
				    maxnum=enemynum;
					xmax=i;
					ymax=j;
				end
			end
		end
	end

elseif wugongtype==1 then    --线攻击
		for direct=0,3 do           -- 对每个方向循环，找出敌人最多的
    		local enemynum=0;
			for i=1,movescope do
                local xnew=x+CC.DirectX[direct+1]*i;
                local ynew=y+CC.DirectY[direct+1]*i;

				if xnew>=0 and xnew<CC.WarWidth and ynew>=0 and ynew<CC.WarHeight then
					local id=Byte.get16(WAR.Map2,(ynew*CC.WarWidth+xnew)*2);
					if id>=0 then
						if WAR.Person[WAR.CurID]["我方"] ~= WAR.Person[id]["我方"] then
							enemynum=enemynum+1;                  --武功攻击范围内的敌人个数
						end
					end
				end
			end
			if enemynum>maxnum then        --记录最多敌人和位置
				maxnum=enemynum;
				xmax=x+CC.DirectX[direct+1];       --线攻击记录一个代表方向的坐标
				ymax=y+CC.DirectY[direct+1];
			end
		end

elseif wugongtype==2 then --十字攻击
 		local enemynum=0;
		for direct=0,3 do           -- 对每个方向循环
			for i=1,movescope do
                local xnew=x+CC.DirectX[direct+1]*i;
                local ynew=y+CC.DirectY[direct+1]*i;
				if xnew>=0 and xnew<CC.WarWidth and ynew>=0 and ynew<CC.WarHeight then
					local id=Byte.get16(WAR.Map2,(ynew*CC.WarWidth+xnew)*2);
					if id>=0 then
						if WAR.Person[WAR.CurID]["我方"] ~= WAR.Person[id]["我方"] then
							enemynum=enemynum+1;                  --武功攻击范围内的敌人个数
						end
					end
				end
			end
        end
		maxnum=enemynum;
end
	return maxnum,xmax,ymax;
end

--自动执行战斗，此时的位置一定可以打到敌人
function War_AutoExecuteFight(wugongnum)            --自动执行战斗，显示攻击动画
    local pid=WAR.Person[WAR.CurID]["人物编号"];
    local x0=WAR.Person[WAR.CurID]["坐标X"];
    local y0=WAR.Person[WAR.CurID]["坐标Y"];
    local wugongid=JY.Person[pid]["武功"  ..wugongnum];
    local level=math.modf(JY.Person[pid]["武功等级".. wugongnum]/100)+1;

    local maxnum,x,y=War_AutoCalMaxEnemy(x0,y0,wugongid,level);

    War_Fight_Sub(WAR.CurID,wugongnum,x,y);

end

--逃跑
function War_AutoEscape()                --逃跑
    local pid=WAR.Person[WAR.CurID]["人物编号"];
    if JY.Person[pid]["体力"]<=5  then
	    return
	end

    local maxDest=0;
    local x,y;

    War_CalMoveStep(WAR.CurID,WAR.Person[WAR.CurID]["移动步数"],0);   --计算移动步数

    for i=0,CC.WarWidth-1 do
        for j=0,CC.WarHeight-1 do
            if Byte.get16(WAR.Map3,(j*CC.WarWidth+i)*2) <128 then
                local minDest=math.huge;
                for k=0,WAR.PersonNum-1 do
                    if WAR.Person[WAR.CurID]["我方"]~=WAR.Person[k]["我方"] and WAR.Person[k]["死亡"]==false then
                        local dx=math.abs(i-WAR.Person[k]["坐标X"])
                        local dy=math.abs(j-WAR.Person[k]["坐标Y"])
                        if minDest>(dx+dy) then        --计算当前距离敌人最近的位置
                            minDest=dx+dy;
                        end
                    end
                end

                if minDest>maxDest then           --找一个最远的位置
                    maxDest=minDest;
                    x=i;
                    y=j;
                end
            end
        end
    end

    if maxDest>0 then
        War_MovePerson(x,y);    --移动到相应的位置
    end

end


---吃药加生命
----flag=2 生命，3内力；4体力
function War_AutoEatDrug(flag)          ---吃药加参数
    local pid=WAR.Person[WAR.CurID]["人物编号"];
    local life=JY.Person[pid]["生命"];
    local maxlife=JY.Person[pid]["生命最大值"];
    local selectid;
    local minvalue=math.huge;

    local shouldadd;
    local maxattrib;
    local str;
    if flag==2 then
        maxattrib=JY.Person[pid]["生命最大值"];
        shouldadd=maxattrib-JY.Person[pid]["生命"];
        str="加生命";
    elseif flag==3 then
        maxattrib=JY.Person[pid]["内力最大值"];
        shouldadd=maxattrib-JY.Person[pid]["内力"];
        str="加内力";
    elseif flag==4 then
        maxattrib=CC.PersonAttribMax["体力"];
        shouldadd=maxattrib-JY.Person[pid]["体力"];
        str="加体力";
    else
        return ;
    end

    if WAR.Person[WAR.CurID]["我方"]==true then
        local extra=0;
        for i =1, CC.MyThingNum do
            local thingid=JY.Base["物品" ..i];
            if thingid>=0 then
                local add=JY.Thing[thingid][str];
                if JY.Thing[thingid]["类型"]==3 and add>0 then
                    lib.Debug(string.format("%s tihngid=%d",str,thingid));
                    local v=shouldadd-add;
                    if v<0 then               --可以加满, 用其他方法找合适药品
                        extra=1;
                        break;
                    else
                        if v<minvalue then        --寻找加生命后生命最大的
                            minvalue=v;
                            selectid=thingid;
                        end
                    end
                end
            end
        end
        if extra==1 then
            minvalue=math.huge;
            for i =1, CC.MyThingNum do
                local thingid=JY.Base["物品" ..i];
                if thingid>=0 then
                    local add=JY.Thing[thingid][str];
                    if JY.Thing[thingid]["类型"]==3 and add>0 then
                        local v=add-shouldadd;
                        if v>=0 then               --可以加满生命
                            if v<minvalue then
                                minvalue=v;
                                selectid=thingid;
                            end
                        end
                    end
                end
            end
        end
        if UseThingEffect(selectid,pid)==1 then       --使用有效果
            instruct_32(selectid,-1);            --物品数量减少
        end
    else
        local extra=0;
        for i =1, 4 do
            local thingid=JY.Person[pid]["携带物品" ..i];
            if thingid>=0 then
                local add=JY.Thing[thingid][str];
                if JY.Thing[thingid]["类型"]==3 and add>0 then
                    local v=shouldadd-add;
                    if v<0 then               --可以加满生命, 用其他方法找合适药品
                        extra=1;
                        break;
                    else
                        if v<minvalue then        --寻找加生命后生命最大的
                            minvalue=v;
                            selectid=thingid;
                        end
                    end
                end
            end
        end
        if extra==1 then
            minvalue=math.huge;
            for i =1, 4 do
                local thingid=JY.Person[pid]["携带物品" ..i];
                if thingid>=0 then
                    local add=JY.Thing[thingid][str];
                    if JY.Thing[thingid]["类型"]==3 and add>0 then
                        local v=add-shouldadd;
                        if v>=0 then               --可以加满生命
                            if v<minvalue then
                                minvalue=v;
                                selectid=thingid;
                            end
                        end
                    end
                end
            end
        end

        if UseThingEffect(selectid,pid)==1 then       --使用有效果
            instruct_41(pid,selectid,-1);            --物品数量减少
        end
    end

    lib.Delay(500);
end


--自动医疗
function War_AutoDoctor()            --自动医疗
    local x1=WAR.Person[WAR.CurID]["坐标X"];
    local y1=WAR.Person[WAR.CurID]["坐标Y"];

    War_ExecuteMenu_Sub(x1,y1,3,-1);
end




