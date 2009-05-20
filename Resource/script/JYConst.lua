
--设置全局变量CC，保存游戏中使用的常数
CC={}; 
CC.ScreenW=640;          --显示窗口宽高
CC.ScreenH=480;

-- Windows 键码定义，msdn
VK_ESCAPE=0x1B
VK_Y=0x59
VK_N=0x4e
VK_SPACE=0x20
VK_RETURN=0x0d
VK_UP=0x26
VK_DOWN=0x28
VK_LEFT=0x25
VK_RIGHT=0x27
VK_F1=0x70
		   
function SetGlobalConst()

   -- 游戏中颜色定义
    C_STARTMENU=RGB(132, 0, 4)            -- 开始菜单颜色
    C_RED=RGB(216, 20, 24)                -- 开始菜单选中项颜色

    C_WHITE=RGB(236, 236, 236);           --游戏内常用的几个颜色值
    C_ORANGE=RGB(252, 148, 16);
    C_GOLD=RGB(236, 200, 40);

   -- 游戏状态定义
    GAME_START =0       --开始画面
    GAME_FIRSTMMAP = 1  --第一次显示主地图
    GAME_MMAP =2;       --主地图
    GAME_FIRSTSMAP = 3  --第一次显示主地图
    GAME_SMAP =4;       --场景地图
    GAME_WMAP =5;       --战斗地图
		GAME_DEAD =6;       --死亡画面
    GAME_END  =7;       --结束

   --游戏数据全局变量
        --定义游戏中使用的常量，这些可以在修改游戏时修改之

   CC.SrcCharSet=0;         --源代码的字符集 0 gb  1 big5，用于转换R×。 如果源码被转换为big5，则应设为1
   CC.OSCharSet=-1;         --OS 字符集，-1 系统判断，0 GB, 1 Big5
   CC.FontName="SimSun";    --显示字体


   --定义记录文件名。S和D由于是固定大小，因此不再定义idx了。
   CC.R_IDXFilename={[0]="data\\Ranger.idx","data\\r1.idx","data\\r2.idx","data\\r3.idx",};
   CC.R_GRPFilename={[0]="data\\Ranger.grp","data\\r1.grp","data\\r2.grp","data\\r3.grp",};
   CC.S_Filename={[0]="data\\allsin.grp","data\\s1.grp","data\\s2.grp","data\\s3.grp",};
   CC.D_Filename={[0]="data\\alldef.grp","data\\d1.grp","data\\d2.grp","data\\d3.grp",};

   CC.PaletteFile="data\\mmap.col";

   CC.FirstFile="data\\title.big";
   CC.DeadFile="data\\dead.big";
   CC.MMapFile={"data\\earth.002", "data\\surface.002","data\\building.002",
		            "data\\buildx.002","data\\buildy.002"};

   --各种贴图文件名。这里只给出文件名，扩展名必须为idx/grp
   CC.MMAPPicFile="data\\mmap";
   CC.HeadPicFile="data\\hdgrp";
   CC.ThingPicFile="data\\thing";
   CC.SMAPPicFile="data\\smap";
   CC.WMAPPicFile="data\\wmap";
   CC.EffectFile="data\\eft";
   CC.FightPicFile="data\\fight%03d";        --此处为字符串格式，类似于C中printf的格式。

   CC.MIDIFile="sound\\game%02d.ogg";
   CC.ATKFile="sound\\atk%02d.wav";
   CC.EFile="sound\\e%02d.wav";

   CC.WarFile="data\\war.sta";
   CC.WarMapFile={"data\\warfld.idx","data\\warfld.grp"};

   --定义记录文件R×结构。  lua不支持结构，无法直接从二进制文件中读取，因此需要这些定义，用table中不同的名字来仿真结构。
   CC.TeamNum=6;          --队伍人数
   CC.MyThingNum=200      --主角物品数量

   CC.Base_S={};         --保存基本数据的结构，以便以后存取
   CC.Base_S["乘船"]={0,0,2}   -- 起始位置(从0开始)，数据类型(0有符号 1无符号，2字符串)，长度
   CC.Base_S["无用"]={2,0,2};
   CC.Base_S["人X"]={4,0,2};
   CC.Base_S["人Y"]={6,0,2};
   CC.Base_S["人X1"]={8,0,2};
   CC.Base_S["人Y1"]={10,0,2};
   CC.Base_S["人方向"]={12,0,2};
   CC.Base_S["船X"]={14,0,2};
   CC.Base_S["船Y"]={16,0,2};
   CC.Base_S["船X1"]={18,0,2};
   CC.Base_S["船Y1"]={20,0,2};
   CC.Base_S["船方向"]={22,0,2};

   for i=1,CC.TeamNum do
        CC.Base_S["队伍" .. i]={24+2*(i-1),0,2};
   end

   for i=1,CC.MyThingNum do
        CC.Base_S["物品" .. i]={36+4*(i-1),0,2};
        CC.Base_S["物品数量" .. i]={36+4*(i-1)+2,0,2};
   end

    CC.PersonSize=182;   --每个人物数据占用字节
    CC.Person_S={};      --保存人物数据的结构，以便以后存取
    CC.Person_S["代号"]={0,0,2}
    CC.Person_S["头像代号"]={2,0,2}
    CC.Person_S["生命增长"]={4,0,2}
    CC.Person_S["无用"]={6,0,2}
    CC.Person_S["姓名"]={8,2,10}
    CC.Person_S["外号"]={18,2,10}
    CC.Person_S["性别"]={28,0,2}
    CC.Person_S["等级"]={30,0,2}
    CC.Person_S["经验"]={32,1,2}
    CC.Person_S["生命"]={34,0,2}
    CC.Person_S["生命最大值"]={36,0,2}
    CC.Person_S["受伤程度"]={38,0,2}
    CC.Person_S["中毒程度"]={40,0,2}
    CC.Person_S["体力"]={42,0,2}
    CC.Person_S["物品修炼点数"]={44,0,2}
    CC.Person_S["武器"]={46,0,2}
    CC.Person_S["防具"]={48,0,2}

     for i=1,5 do
        CC.Person_S["出招动画帧数" .. i]={50+2*(i-1),0,2};
        CC.Person_S["出招动画延迟" .. i]={60+2*(i-1),0,2};
        CC.Person_S["武功音效延迟" .. i]={70+2*(i-1),0,2};
     end

    CC.Person_S["内力性质"]={80,0,2}
    CC.Person_S["内力"]={82,0,2}
    CC.Person_S["内力最大值"]={84,0,2}
    CC.Person_S["攻击力"]={86,0,2}
    CC.Person_S["轻功"]={88,0,2}
    CC.Person_S["防御力"]={90,0,2}
    CC.Person_S["医疗能力"]={92,0,2}
    CC.Person_S["用毒能力"]={94,0,2}
    CC.Person_S["解毒能力"]={96,0,2}
    CC.Person_S["抗毒能力"]={98,0,2}

    CC.Person_S["拳掌功夫"]={100,0,2}
    CC.Person_S["御剑能力"]={102,0,2}
    CC.Person_S["耍刀技巧"]={104,0,2}
    CC.Person_S["特殊兵器"]={106,0,2}
    CC.Person_S["暗器技巧"]={108,0,2}


    CC.Person_S["武学常识"]={110,0,2}
    CC.Person_S["品德"]={112,0,2}
    CC.Person_S["攻击带毒"]={114,0,2}
    CC.Person_S["左右互搏"]={116,0,2}
    CC.Person_S["声望"]={118,0,2}

    CC.Person_S["资质"]={120,0,2}
    CC.Person_S["修炼物品"]={122,0,2}
    CC.Person_S["修炼点数"]={124,0,2}

     for i=1,10 do
        CC.Person_S["武功" .. i]={126+2*(i-1),0,2};
        CC.Person_S["武功等级" .. i]={146+2*(i-1),0,2};
     end

     for i=1,4 do
        CC.Person_S["携带物品" .. i]={166+2*(i-1),0,2};
        CC.Person_S["携带物品数量" .. i]={174+2*(i-1),0,2};
     end

    CC.ThingSize=190;   --每个人物数据占用字节
    CC.Thing_S={};
    CC.Thing_S["代号"]={0,0,2}
    CC.Thing_S["名称"]={2,2,20}
    CC.Thing_S["名称2"]={22,2,20}
    CC.Thing_S["物品说明"]={42,2,30}
    CC.Thing_S["练出武功"]={72,0,2}
    CC.Thing_S["暗器动画编号"]={74,0,2}
    CC.Thing_S["使用人"]={76,0,2}
    CC.Thing_S["装备类型"]={78,0,2}
    CC.Thing_S["显示物品说明"]={80,0,2}
    CC.Thing_S["类型"]={82,0,2}
    CC.Thing_S["未知5"]={84,0,2}
    CC.Thing_S["未知6"]={86,0,2}
    CC.Thing_S["未知7"]={88,0,2}
    CC.Thing_S["加生命"]={90,0,2}
    CC.Thing_S["加生命最大值"]={92,0,2}
    CC.Thing_S["加中毒解毒"]={94,0,2}
    CC.Thing_S["加体力"]={96,0,2}
    CC.Thing_S["改变内力性质"]={98,0,2}
    CC.Thing_S["加内力"]={100,0,2}

    CC.Thing_S["加内力最大值"]={102,0,2}
    CC.Thing_S["加攻击力"]={104,0,2}
    CC.Thing_S["加轻功"]={106,0,2}
    CC.Thing_S["加防御力"]={108,0,2}
    CC.Thing_S["加医疗能力"]={110,0,2}

    CC.Thing_S["加用毒能力"]={112,0,2}
    CC.Thing_S["加解毒能力"]={114,0,2}
    CC.Thing_S["加抗毒能力"]={116,0,2}
    CC.Thing_S["加拳掌功夫"]={118,0,2}
    CC.Thing_S["加御剑能力"]={120,0,2}

    CC.Thing_S["加耍刀技巧"]={122,0,2}
    CC.Thing_S["加特殊兵器"]={124,0,2}
    CC.Thing_S["加暗器技巧"]={126,0,2}
    CC.Thing_S["加武学常识"]={128,0,2}
    CC.Thing_S["加品德"]={130,0,2}

    CC.Thing_S["加攻击次数"]={132,0,2}
    CC.Thing_S["加攻击带毒"]={134,0,2}
    CC.Thing_S["仅修炼人物"]={136,0,2}
    CC.Thing_S["需内力性质"]={138,0,2}
    CC.Thing_S["需内力"]={140,0,2}

    CC.Thing_S["需攻击力"]={142,0,2}
    CC.Thing_S["需轻功"]={144,0,2}
    CC.Thing_S["需用毒能力"]={146,0,2}
    CC.Thing_S["需医疗能力"]={148,0,2}
    CC.Thing_S["需解毒能力"]={150,0,2}

    CC.Thing_S["需拳掌功夫"]={152,0,2}
    CC.Thing_S["需御剑能力"]={154,0,2}
    CC.Thing_S["需耍刀技巧"]={156,0,2}
    CC.Thing_S["需特殊兵器"]={158,0,2}
    CC.Thing_S["需暗器技巧"]={160,0,2}

    CC.Thing_S["需资质"]={162,0,2}
    CC.Thing_S["需经验"]={164,0,2}
    CC.Thing_S["练出物品需经验"]={166,0,2}
    CC.Thing_S["需材料"]={168,0,2}

      for i=1,5 do
        CC.Thing_S["练出物品" .. i]={170+2*(i-1),0,2};
        CC.Thing_S["需要物品数量" .. i]={180+2*(i-1),0,2};
     end

    CC.SceneSize=52;   --每个场景数据占用字节
    CC.Scene_S={};
    CC.Scene_S["代号"]={0,0,2}
    CC.Scene_S["名称"]={2,2,10}
    CC.Scene_S["出门音乐"]={12,0,2}
    CC.Scene_S["进门音乐"]={14,0,2}
    CC.Scene_S["跳转场景"]={16,0,2}
    CC.Scene_S["进入条件"]={18,0,2}
    CC.Scene_S["外景入口X1"]={20,0,2}
    CC.Scene_S["外景入口Y1"]={22,0,2}
    CC.Scene_S["外景入口X2"]={24,0,2}
    CC.Scene_S["外景入口Y2"]={26,0,2}
    CC.Scene_S["入口X"]={28,0,2}
    CC.Scene_S["入口Y"]={30,0,2}
    CC.Scene_S["出口X1"]={32,0,2}
    CC.Scene_S["出口X2"]={34,0,2}
    CC.Scene_S["出口X3"]={36,0,2}
    CC.Scene_S["出口Y1"]={38,0,2}
    CC.Scene_S["出口Y2"]={40,0,2}
    CC.Scene_S["出口Y3"]={42,0,2}
    CC.Scene_S["跳转口X1"]={44,0,2}
    CC.Scene_S["跳转口Y1"]={46,0,2}
    CC.Scene_S["跳转口X2"]={48,0,2}
    CC.Scene_S["跳转口Y2"]={50,0,2}

    CC.WugongSize=136;   --每个武功数据占用字节
    CC.Wugong_S={};
    CC.Wugong_S["代号"]={0,0,2}
    CC.Wugong_S["名称"]={2,2,10}
    CC.Wugong_S["未知1"]={12,0,2}
    CC.Wugong_S["未知2"]={14,0,2}
    CC.Wugong_S["未知3"]={16,0,2}
    CC.Wugong_S["未知4"]={18,0,2}
    CC.Wugong_S["未知5"]={20,0,2}
    CC.Wugong_S["出招音效"]={22,0,2}
    CC.Wugong_S["武功类型"]={24,0,2}
    CC.Wugong_S["武功动画&音效"]={26,0,2}
    CC.Wugong_S["伤害类型"]={28,0,2}
    CC.Wugong_S["攻击范围"]={30,0,2}
    CC.Wugong_S["消耗内力点数"]={32,0,2}
    CC.Wugong_S["敌人中毒点数"]={34,0,2}

     for i=1,10 do
        CC.Wugong_S["攻击力" .. i]={36+2*(i-1),0,2};
        CC.Wugong_S["移动范围" .. i]={56+2*(i-1),0,2};
        CC.Wugong_S["杀伤范围" .. i]={76+2*(i-1),0,2};
        CC.Wugong_S["加内力" .. i]={96+2*(i-1),0,2};
        CC.Wugong_S["杀内力" .. i]={116+2*(i-1),0,2};
     end

   CC.ShopSize=30;   --每个小宝商店数据占用字节
   CC.Shop_S={};
   for i=1,5 do
      CC.Shop_S["物品" .. i]={0+2*(i-1),0,2};
      CC.Shop_S["物品数量" .. i]={10+2*(i-1),0,2};
      CC.Shop_S["物品价格" .. i]={20+2*(i-1),0,2};
   end

   CC.ShopScene={};       --小宝商店场景数据，sceneid 场景id，d_shop 小宝位置D*, d_leave 小宝离开D*，一般在场景出口，路过触发
   CC.ShopScene[0]={sceneid=1,d_shop=16,d_leave={17,18}, };
   CC.ShopScene[1]={sceneid=3,d_shop=14,d_leave={15,16}, };
   CC.ShopScene[2]={sceneid=40,d_shop=20,d_leave={21,22}, };
   CC.ShopScene[3]={sceneid=60,d_shop=16,d_leave={17,18}, };
   CC.ShopScene[4]={sceneid=61,d_shop=9,d_leave={10,11,12}, };

  --其他常量
   CC.MWidth=480;       --主地图宽
   CC.MHeight=480;      --主地图高

   CC.SWidth=64;     --子场景地图大小
   CC.SHeight=64;

   CC.DNum=200;       --D*每个场景的事件数

   CC.XScale=18;    --贴图一半的宽高
   CC.YScale=9;


   CC.Frame=50;     --每帧毫秒数

   CC.DirectX={0,1,-1,0};  --不同方向x，y的加减值，用于走路改变坐标值
   CC.DirectY={-1,0,0,1};

   CC.MyStartPic=2501;      --主角走路起始贴图

   CC.Level=30;                  ---人物等级和每等级经验
   CC.Exp={    50,    150,     300 ,500   , 750 ,
               1050,  1400,   1800 ,2250  , 2750 ,
               3850,  5050,   6350 ,7750  , 9250 ,
               10850, 12550, 14350 ,16750 , 18250 ,
               21400, 24700, 28150 ,31750 , 35500 ,
	           39400, 43450, 47650 ,52000 , 60000  };

    CC.SceneWater={};    --场景人不能进入的贴图
    local tmpWater={ {0x166,0x16a},{0x176,0x17c},{0x1ca,0x1d0},{0x1fa,0x262},{0x332,0x338},
                     {0x346,0x346},{0x3a6,0x3a8},{0x3f8,0x3fe},{0x52c,0x544},};
    for i,v in ipairs(tmpWater) do      --把这些数据变换成数组，其中空项就是可以进入的贴图
        for j=v[1],v[2],2 do
            CC.SceneWater[j]=1;
        end
    end

    CC.WarWater={};    --战斗地图人不能进入的贴图
    local tmpWater={ {0x166,0x16a},{0x176,0x17c},{0x1ca,0x1d0},{0x1fa,0x262},{0x332,0x338},
                     {0x346,0x346},{0x3a6,0x3a8},{0x3f8,0x3fe},{0x52c,0x544},};
    for i,v in ipairs(tmpWater) do      --把这些数据变换成数组，其中空项就是可以进入的贴图
        for j=v[1],v[2],2 do
            CC.WarWater[j]=1;
        end
    end

    --离队人员列表: {人员id，离队调用函数}      ----如果有新的离队人员加入，直接在这里增加
    CC.PersonExit={{1,oldevent_950},{2,oldevent_952},{9,oldevent_954},{16,oldevent_956},{17,oldevent_958},
                   {25,oldevent_960},{28,oldevent_962},{29,oldevent_964},{35,oldevent_966},{36,oldevent_968},
                   {37,oldevent_970},{38,oldevent_972},{44,oldevent_974},{45,oldevent_976},{47,oldevent_978},
                   {48,oldevent_980},{49,oldevent_982},{51,oldevent_984},{53,oldevent_986},{54,oldevent_988},
                   {58,oldevent_990},{59,oldevent_992},{61,oldevent_994},{63,oldevent_996},{76,oldevent_998},  }

    --所有可加入人员离队需要清除的D*事件，清除后这些人就找不到了。得到武林帖指令使用
    CC.AllPersonExit={ {0,0},{49,2},{4,1},{44,0},{44,1},{37,5},{30,0},{59,0},{40,3},{56,1},{1,7},{1,8},{1,10},
                       {40,7},{40,8},{77,0},{54,0},{62,3},{62,4},{60,2},{60,15},{52,1},{61,0},{61,8},{78,0},
                       {18,0},{18,1},{69,0},{69,1},{45,0},{52,2},{42,6},{42,7},{8,8},{7,6},{80,1}, };

    CC.BookNum=14;               --天书个数
    CC.BookStart=144;            --14天书起始物品id

    CC.MoneyID=174;              --金钱物品id

   CC.Effect={[0]=9,14,17,9,13,                    --eft.idx/grp贴图各个武功效果贴图个数
                 17,17,17,18,19,
                 19,15,13,10,10,
                 15,21,16,9,11,
                 8,9,8,8,7,
                 8,8,9,12,19,
                 11,14,12,17,8,
                 11,9,13,10,19,
                 14,17,19,14,21,
                 16,13,18,14,17,
                 17,16,7,   };

    CC.ExtraOffense={{106,57,100},             --武功武器配合增加攻击力， 依次为：武器物品id，武功id，攻击力增加
                   {107,49,50},
                   {108,49,50},
                   {110,54,80},
                   {115,63,50},
                   {116,67,70},
                   {119,68,100},}

    CC.NewPersonName="徐小侠";                --新游戏的数据
    CC.NewGameSceneID=70;                      --场景ID
    CC.NewGameSceneX=19;                       --场景坐标
    CC.NewGameSceneY=20;
    CC.NewPersonPic=3445;                      --开始主角pic

   CC.PersonAttribMax={};             --人物属性最大值
   CC.PersonAttribMax["经验"]=60000;
   CC.PersonAttribMax["物品修炼点数"]=60000;
   CC.PersonAttribMax["修炼点数"]=60000;
   CC.PersonAttribMax["生命最大值"]=999;
   CC.PersonAttribMax["受伤程度"]=100;
   CC.PersonAttribMax["中毒程度"]=100;
   CC.PersonAttribMax["内力最大值"]=999;
   CC.PersonAttribMax["体力"]=100;
   CC.PersonAttribMax["攻击力"]=100;
   CC.PersonAttribMax["防御力"]=100;
   CC.PersonAttribMax["轻功"]=100;
   CC.PersonAttribMax["医疗能力"]=100;
   CC.PersonAttribMax["用毒能力"]=100;
   CC.PersonAttribMax["解毒能力"]=100;
   CC.PersonAttribMax["抗毒能力"]=100;
   CC.PersonAttribMax["拳掌功夫"]=100;
   CC.PersonAttribMax["御剑能力"]=100;
   CC.PersonAttribMax["耍刀技巧"]=100;
   CC.PersonAttribMax["特殊兵器"]=100;
   CC.PersonAttribMax["暗器技巧"]=100;
   CC.PersonAttribMax["武学常识"]=100;
   CC.PersonAttribMax["品德"]=100;
   CC.PersonAttribMax["资质"]=100;
   CC.PersonAttribMax["攻击带毒"]=100;

    CC.WarDataSize=186;         --战斗数据大小  war.sta数据结构
    CC.WarData_S={};        --战斗数据结构
    CC.WarData_S["代号"]={0,0,2};
    CC.WarData_S["名称"]={2,2,10};
    CC.WarData_S["地图"]={12,0,2};
    CC.WarData_S["经验"]={14,0,2};
    CC.WarData_S["音乐"]={16,0,2};
    for i=1,6 do
        CC.WarData_S["手动选择参战人"  .. i]={18+(i-1)*2,0,2};
        CC.WarData_S["自动选择参战人"  .. i]={30+(i-1)*2,0,2};
        CC.WarData_S["我方X"  .. i]={42+(i-1)*2,0,2};
        CC.WarData_S["我方Y"  .. i]={54+(i-1)*2,0,2};
    end
    for i=1,20 do
        CC.WarData_S["敌人"  .. i]={66+(i-1)*2,0,2};
        CC.WarData_S["敌方X"  .. i]={106+(i-1)*2,0,2};
        CC.WarData_S["敌方Y"  .. i]={146+(i-1)*2,0,2};
    end

    CC.WarWidth=64;        --战斗地图大小
    CC.WarHeight=64;




end
