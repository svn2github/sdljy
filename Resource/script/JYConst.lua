
--����ȫ�ֱ���CC��������Ϸ��ʹ�õĳ���
CC={}; 
CC.ScreenW=640;          --��ʾ���ڿ��
CC.ScreenH=480;

-- Windows ���붨�壬msdn
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

   -- ��Ϸ����ɫ����
    C_STARTMENU=RGB(132, 0, 4)            -- ��ʼ�˵���ɫ
    C_RED=RGB(216, 20, 24)                -- ��ʼ�˵�ѡ������ɫ

    C_WHITE=RGB(236, 236, 236);           --��Ϸ�ڳ��õļ�����ɫֵ
    C_ORANGE=RGB(252, 148, 16);
    C_GOLD=RGB(236, 200, 40);

   -- ��Ϸ״̬����
    GAME_START =0       --��ʼ����
    GAME_FIRSTMMAP = 1  --��һ����ʾ����ͼ
    GAME_MMAP =2;       --����ͼ
    GAME_FIRSTSMAP = 3  --��һ����ʾ����ͼ
    GAME_SMAP =4;       --������ͼ
    GAME_WMAP =5;       --ս����ͼ
		GAME_DEAD =6;       --��������
    GAME_END  =7;       --����

   --��Ϸ����ȫ�ֱ���
        --������Ϸ��ʹ�õĳ�������Щ�������޸���Ϸʱ�޸�֮

   CC.SrcCharSet=0;         --Դ������ַ��� 0 gb  1 big5������ת��R���� ���Դ�뱻ת��Ϊbig5����Ӧ��Ϊ1
   CC.OSCharSet=-1;         --OS �ַ�����-1 ϵͳ�жϣ�0 GB, 1 Big5
   CC.FontName="SimSun";    --��ʾ����


   --�����¼�ļ�����S��D�����ǹ̶���С����˲��ٶ���idx�ˡ�
   CC.R_IDXFilename={[0]="data\\Ranger.idx","data\\r1.idx","data\\r2.idx","data\\r3.idx",};
   CC.R_GRPFilename={[0]="data\\Ranger.grp","data\\r1.grp","data\\r2.grp","data\\r3.grp",};
   CC.S_Filename={[0]="data\\allsin.grp","data\\s1.grp","data\\s2.grp","data\\s3.grp",};
   CC.D_Filename={[0]="data\\alldef.grp","data\\d1.grp","data\\d2.grp","data\\d3.grp",};

   CC.PaletteFile="data\\mmap.col";

   CC.FirstFile="data\\title.big";
   CC.DeadFile="data\\dead.big";
   CC.MMapFile={"data\\earth.002", "data\\surface.002","data\\building.002",
		            "data\\buildx.002","data\\buildy.002"};

   --������ͼ�ļ���������ֻ�����ļ�������չ������Ϊidx/grp
   CC.MMAPPicFile="data\\mmap";
   CC.HeadPicFile="data\\hdgrp";
   CC.ThingPicFile="data\\thing";
   CC.SMAPPicFile="data\\smap";
   CC.WMAPPicFile="data\\wmap";
   CC.EffectFile="data\\eft";
   CC.FightPicFile="data\\fight%03d";        --�˴�Ϊ�ַ�����ʽ��������C��printf�ĸ�ʽ��

   CC.MIDIFile="sound\\game%02d.ogg";
   CC.ATKFile="sound\\atk%02d.wav";
   CC.EFile="sound\\e%02d.wav";

   CC.WarFile="data\\war.sta";
   CC.WarMapFile={"data\\warfld.idx","data\\warfld.grp"};

   --�����¼�ļ�R���ṹ��  lua��֧�ֽṹ���޷�ֱ�ӴӶ������ļ��ж�ȡ�������Ҫ��Щ���壬��table�в�ͬ������������ṹ��
   CC.TeamNum=6;          --��������
   CC.MyThingNum=200      --������Ʒ����

   CC.Base_S={};         --����������ݵĽṹ���Ա��Ժ��ȡ
   CC.Base_S["�˴�"]={0,0,2}   -- ��ʼλ��(��0��ʼ)����������(0�з��� 1�޷��ţ�2�ַ���)������
   CC.Base_S["����"]={2,0,2};
   CC.Base_S["��X"]={4,0,2};
   CC.Base_S["��Y"]={6,0,2};
   CC.Base_S["��X1"]={8,0,2};
   CC.Base_S["��Y1"]={10,0,2};
   CC.Base_S["�˷���"]={12,0,2};
   CC.Base_S["��X"]={14,0,2};
   CC.Base_S["��Y"]={16,0,2};
   CC.Base_S["��X1"]={18,0,2};
   CC.Base_S["��Y1"]={20,0,2};
   CC.Base_S["������"]={22,0,2};

   for i=1,CC.TeamNum do
        CC.Base_S["����" .. i]={24+2*(i-1),0,2};
   end

   for i=1,CC.MyThingNum do
        CC.Base_S["��Ʒ" .. i]={36+4*(i-1),0,2};
        CC.Base_S["��Ʒ����" .. i]={36+4*(i-1)+2,0,2};
   end

    CC.PersonSize=182;   --ÿ����������ռ���ֽ�
    CC.Person_S={};      --�����������ݵĽṹ���Ա��Ժ��ȡ
    CC.Person_S["����"]={0,0,2}
    CC.Person_S["ͷ�����"]={2,0,2}
    CC.Person_S["��������"]={4,0,2}
    CC.Person_S["����"]={6,0,2}
    CC.Person_S["����"]={8,2,10}
    CC.Person_S["���"]={18,2,10}
    CC.Person_S["�Ա�"]={28,0,2}
    CC.Person_S["�ȼ�"]={30,0,2}
    CC.Person_S["����"]={32,1,2}
    CC.Person_S["����"]={34,0,2}
    CC.Person_S["�������ֵ"]={36,0,2}
    CC.Person_S["���˳̶�"]={38,0,2}
    CC.Person_S["�ж��̶�"]={40,0,2}
    CC.Person_S["����"]={42,0,2}
    CC.Person_S["��Ʒ��������"]={44,0,2}
    CC.Person_S["����"]={46,0,2}
    CC.Person_S["����"]={48,0,2}

     for i=1,5 do
        CC.Person_S["���ж���֡��" .. i]={50+2*(i-1),0,2};
        CC.Person_S["���ж����ӳ�" .. i]={60+2*(i-1),0,2};
        CC.Person_S["�书��Ч�ӳ�" .. i]={70+2*(i-1),0,2};
     end

    CC.Person_S["��������"]={80,0,2}
    CC.Person_S["����"]={82,0,2}
    CC.Person_S["�������ֵ"]={84,0,2}
    CC.Person_S["������"]={86,0,2}
    CC.Person_S["�Ṧ"]={88,0,2}
    CC.Person_S["������"]={90,0,2}
    CC.Person_S["ҽ������"]={92,0,2}
    CC.Person_S["�ö�����"]={94,0,2}
    CC.Person_S["�ⶾ����"]={96,0,2}
    CC.Person_S["��������"]={98,0,2}

    CC.Person_S["ȭ�ƹ���"]={100,0,2}
    CC.Person_S["��������"]={102,0,2}
    CC.Person_S["ˣ������"]={104,0,2}
    CC.Person_S["�������"]={106,0,2}
    CC.Person_S["��������"]={108,0,2}


    CC.Person_S["��ѧ��ʶ"]={110,0,2}
    CC.Person_S["Ʒ��"]={112,0,2}
    CC.Person_S["��������"]={114,0,2}
    CC.Person_S["���һ���"]={116,0,2}
    CC.Person_S["����"]={118,0,2}

    CC.Person_S["����"]={120,0,2}
    CC.Person_S["������Ʒ"]={122,0,2}
    CC.Person_S["��������"]={124,0,2}

     for i=1,10 do
        CC.Person_S["�书" .. i]={126+2*(i-1),0,2};
        CC.Person_S["�书�ȼ�" .. i]={146+2*(i-1),0,2};
     end

     for i=1,4 do
        CC.Person_S["Я����Ʒ" .. i]={166+2*(i-1),0,2};
        CC.Person_S["Я����Ʒ����" .. i]={174+2*(i-1),0,2};
     end

    CC.ThingSize=190;   --ÿ����������ռ���ֽ�
    CC.Thing_S={};
    CC.Thing_S["����"]={0,0,2}
    CC.Thing_S["����"]={2,2,20}
    CC.Thing_S["����2"]={22,2,20}
    CC.Thing_S["��Ʒ˵��"]={42,2,30}
    CC.Thing_S["�����书"]={72,0,2}
    CC.Thing_S["�����������"]={74,0,2}
    CC.Thing_S["ʹ����"]={76,0,2}
    CC.Thing_S["װ������"]={78,0,2}
    CC.Thing_S["��ʾ��Ʒ˵��"]={80,0,2}
    CC.Thing_S["����"]={82,0,2}
    CC.Thing_S["δ֪5"]={84,0,2}
    CC.Thing_S["δ֪6"]={86,0,2}
    CC.Thing_S["δ֪7"]={88,0,2}
    CC.Thing_S["������"]={90,0,2}
    CC.Thing_S["���������ֵ"]={92,0,2}
    CC.Thing_S["���ж��ⶾ"]={94,0,2}
    CC.Thing_S["������"]={96,0,2}
    CC.Thing_S["�ı���������"]={98,0,2}
    CC.Thing_S["������"]={100,0,2}

    CC.Thing_S["���������ֵ"]={102,0,2}
    CC.Thing_S["�ӹ�����"]={104,0,2}
    CC.Thing_S["���Ṧ"]={106,0,2}
    CC.Thing_S["�ӷ�����"]={108,0,2}
    CC.Thing_S["��ҽ������"]={110,0,2}

    CC.Thing_S["���ö�����"]={112,0,2}
    CC.Thing_S["�ӽⶾ����"]={114,0,2}
    CC.Thing_S["�ӿ�������"]={116,0,2}
    CC.Thing_S["��ȭ�ƹ���"]={118,0,2}
    CC.Thing_S["����������"]={120,0,2}

    CC.Thing_S["��ˣ������"]={122,0,2}
    CC.Thing_S["���������"]={124,0,2}
    CC.Thing_S["�Ӱ�������"]={126,0,2}
    CC.Thing_S["����ѧ��ʶ"]={128,0,2}
    CC.Thing_S["��Ʒ��"]={130,0,2}

    CC.Thing_S["�ӹ�������"]={132,0,2}
    CC.Thing_S["�ӹ�������"]={134,0,2}
    CC.Thing_S["����������"]={136,0,2}
    CC.Thing_S["����������"]={138,0,2}
    CC.Thing_S["������"]={140,0,2}

    CC.Thing_S["�蹥����"]={142,0,2}
    CC.Thing_S["���Ṧ"]={144,0,2}
    CC.Thing_S["���ö�����"]={146,0,2}
    CC.Thing_S["��ҽ������"]={148,0,2}
    CC.Thing_S["��ⶾ����"]={150,0,2}

    CC.Thing_S["��ȭ�ƹ���"]={152,0,2}
    CC.Thing_S["����������"]={154,0,2}
    CC.Thing_S["��ˣ������"]={156,0,2}
    CC.Thing_S["���������"]={158,0,2}
    CC.Thing_S["�谵������"]={160,0,2}

    CC.Thing_S["������"]={162,0,2}
    CC.Thing_S["�辭��"]={164,0,2}
    CC.Thing_S["������Ʒ�辭��"]={166,0,2}
    CC.Thing_S["�����"]={168,0,2}

      for i=1,5 do
        CC.Thing_S["������Ʒ" .. i]={170+2*(i-1),0,2};
        CC.Thing_S["��Ҫ��Ʒ����" .. i]={180+2*(i-1),0,2};
     end

    CC.SceneSize=52;   --ÿ����������ռ���ֽ�
    CC.Scene_S={};
    CC.Scene_S["����"]={0,0,2}
    CC.Scene_S["����"]={2,2,10}
    CC.Scene_S["��������"]={12,0,2}
    CC.Scene_S["��������"]={14,0,2}
    CC.Scene_S["��ת����"]={16,0,2}
    CC.Scene_S["��������"]={18,0,2}
    CC.Scene_S["�⾰���X1"]={20,0,2}
    CC.Scene_S["�⾰���Y1"]={22,0,2}
    CC.Scene_S["�⾰���X2"]={24,0,2}
    CC.Scene_S["�⾰���Y2"]={26,0,2}
    CC.Scene_S["���X"]={28,0,2}
    CC.Scene_S["���Y"]={30,0,2}
    CC.Scene_S["����X1"]={32,0,2}
    CC.Scene_S["����X2"]={34,0,2}
    CC.Scene_S["����X3"]={36,0,2}
    CC.Scene_S["����Y1"]={38,0,2}
    CC.Scene_S["����Y2"]={40,0,2}
    CC.Scene_S["����Y3"]={42,0,2}
    CC.Scene_S["��ת��X1"]={44,0,2}
    CC.Scene_S["��ת��Y1"]={46,0,2}
    CC.Scene_S["��ת��X2"]={48,0,2}
    CC.Scene_S["��ת��Y2"]={50,0,2}

    CC.WugongSize=136;   --ÿ���书����ռ���ֽ�
    CC.Wugong_S={};
    CC.Wugong_S["����"]={0,0,2}
    CC.Wugong_S["����"]={2,2,10}
    CC.Wugong_S["δ֪1"]={12,0,2}
    CC.Wugong_S["δ֪2"]={14,0,2}
    CC.Wugong_S["δ֪3"]={16,0,2}
    CC.Wugong_S["δ֪4"]={18,0,2}
    CC.Wugong_S["δ֪5"]={20,0,2}
    CC.Wugong_S["������Ч"]={22,0,2}
    CC.Wugong_S["�书����"]={24,0,2}
    CC.Wugong_S["�书����&��Ч"]={26,0,2}
    CC.Wugong_S["�˺�����"]={28,0,2}
    CC.Wugong_S["������Χ"]={30,0,2}
    CC.Wugong_S["������������"]={32,0,2}
    CC.Wugong_S["�����ж�����"]={34,0,2}

     for i=1,10 do
        CC.Wugong_S["������" .. i]={36+2*(i-1),0,2};
        CC.Wugong_S["�ƶ���Χ" .. i]={56+2*(i-1),0,2};
        CC.Wugong_S["ɱ�˷�Χ" .. i]={76+2*(i-1),0,2};
        CC.Wugong_S["������" .. i]={96+2*(i-1),0,2};
        CC.Wugong_S["ɱ����" .. i]={116+2*(i-1),0,2};
     end

   CC.ShopSize=30;   --ÿ��С���̵�����ռ���ֽ�
   CC.Shop_S={};
   for i=1,5 do
      CC.Shop_S["��Ʒ" .. i]={0+2*(i-1),0,2};
      CC.Shop_S["��Ʒ����" .. i]={10+2*(i-1),0,2};
      CC.Shop_S["��Ʒ�۸�" .. i]={20+2*(i-1),0,2};
   end

   CC.ShopScene={};       --С���̵곡�����ݣ�sceneid ����id��d_shop С��λ��D*, d_leave С���뿪D*��һ���ڳ������ڣ�·������
   CC.ShopScene[0]={sceneid=1,d_shop=16,d_leave={17,18}, };
   CC.ShopScene[1]={sceneid=3,d_shop=14,d_leave={15,16}, };
   CC.ShopScene[2]={sceneid=40,d_shop=20,d_leave={21,22}, };
   CC.ShopScene[3]={sceneid=60,d_shop=16,d_leave={17,18}, };
   CC.ShopScene[4]={sceneid=61,d_shop=9,d_leave={10,11,12}, };

  --��������
   CC.MWidth=480;       --����ͼ��
   CC.MHeight=480;      --����ͼ��

   CC.SWidth=64;     --�ӳ�����ͼ��С
   CC.SHeight=64;

   CC.DNum=200;       --D*ÿ���������¼���

   CC.XScale=18;    --��ͼһ��Ŀ��
   CC.YScale=9;


   CC.Frame=50;     --ÿ֡������

   CC.DirectX={0,1,-1,0};  --��ͬ����x��y�ļӼ�ֵ��������·�ı�����ֵ
   CC.DirectY={-1,0,0,1};

   CC.MyStartPic=2501;      --������·��ʼ��ͼ

   CC.Level=30;                  ---����ȼ���ÿ�ȼ�����
   CC.Exp={    50,    150,     300 ,500   , 750 ,
               1050,  1400,   1800 ,2250  , 2750 ,
               3850,  5050,   6350 ,7750  , 9250 ,
               10850, 12550, 14350 ,16750 , 18250 ,
               21400, 24700, 28150 ,31750 , 35500 ,
	           39400, 43450, 47650 ,52000 , 60000  };

    CC.SceneWater={};    --�����˲��ܽ������ͼ
    local tmpWater={ {0x166,0x16a},{0x176,0x17c},{0x1ca,0x1d0},{0x1fa,0x262},{0x332,0x338},
                     {0x346,0x346},{0x3a6,0x3a8},{0x3f8,0x3fe},{0x52c,0x544},};
    for i,v in ipairs(tmpWater) do      --����Щ���ݱ任�����飬���п�����ǿ��Խ������ͼ
        for j=v[1],v[2],2 do
            CC.SceneWater[j]=1;
        end
    end

    CC.WarWater={};    --ս����ͼ�˲��ܽ������ͼ
    local tmpWater={ {0x166,0x16a},{0x176,0x17c},{0x1ca,0x1d0},{0x1fa,0x262},{0x332,0x338},
                     {0x346,0x346},{0x3a6,0x3a8},{0x3f8,0x3fe},{0x52c,0x544},};
    for i,v in ipairs(tmpWater) do      --����Щ���ݱ任�����飬���п�����ǿ��Խ������ͼ
        for j=v[1],v[2],2 do
            CC.WarWater[j]=1;
        end
    end

    --�����Ա�б�: {��Աid����ӵ��ú���}      ----������µ������Ա���룬ֱ������������
    CC.PersonExit={{1,oldevent_950},{2,oldevent_952},{9,oldevent_954},{16,oldevent_956},{17,oldevent_958},
                   {25,oldevent_960},{28,oldevent_962},{29,oldevent_964},{35,oldevent_966},{36,oldevent_968},
                   {37,oldevent_970},{38,oldevent_972},{44,oldevent_974},{45,oldevent_976},{47,oldevent_978},
                   {48,oldevent_980},{49,oldevent_982},{51,oldevent_984},{53,oldevent_986},{54,oldevent_988},
                   {58,oldevent_990},{59,oldevent_992},{61,oldevent_994},{63,oldevent_996},{76,oldevent_998},  }

    --���пɼ�����Ա�����Ҫ�����D*�¼����������Щ�˾��Ҳ����ˡ��õ�������ָ��ʹ��
    CC.AllPersonExit={ {0,0},{49,2},{4,1},{44,0},{44,1},{37,5},{30,0},{59,0},{40,3},{56,1},{1,7},{1,8},{1,10},
                       {40,7},{40,8},{77,0},{54,0},{62,3},{62,4},{60,2},{60,15},{52,1},{61,0},{61,8},{78,0},
                       {18,0},{18,1},{69,0},{69,1},{45,0},{52,2},{42,6},{42,7},{8,8},{7,6},{80,1}, };

    CC.BookNum=14;               --�������
    CC.BookStart=144;            --14������ʼ��Ʒid

    CC.MoneyID=174;              --��Ǯ��Ʒid

   CC.Effect={[0]=9,14,17,9,13,                    --eft.idx/grp��ͼ�����书Ч����ͼ����
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

    CC.ExtraOffense={{106,57,100},             --�书����������ӹ������� ����Ϊ��������Ʒid���书id������������
                   {107,49,50},
                   {108,49,50},
                   {110,54,80},
                   {115,63,50},
                   {116,67,70},
                   {119,68,100},}

    CC.NewPersonName="��С��";                --����Ϸ������
    CC.NewGameSceneID=70;                      --����ID
    CC.NewGameSceneX=19;                       --��������
    CC.NewGameSceneY=20;
    CC.NewPersonPic=3445;                      --��ʼ����pic

   CC.PersonAttribMax={};             --�����������ֵ
   CC.PersonAttribMax["����"]=60000;
   CC.PersonAttribMax["��Ʒ��������"]=60000;
   CC.PersonAttribMax["��������"]=60000;
   CC.PersonAttribMax["�������ֵ"]=999;
   CC.PersonAttribMax["���˳̶�"]=100;
   CC.PersonAttribMax["�ж��̶�"]=100;
   CC.PersonAttribMax["�������ֵ"]=999;
   CC.PersonAttribMax["����"]=100;
   CC.PersonAttribMax["������"]=100;
   CC.PersonAttribMax["������"]=100;
   CC.PersonAttribMax["�Ṧ"]=100;
   CC.PersonAttribMax["ҽ������"]=100;
   CC.PersonAttribMax["�ö�����"]=100;
   CC.PersonAttribMax["�ⶾ����"]=100;
   CC.PersonAttribMax["��������"]=100;
   CC.PersonAttribMax["ȭ�ƹ���"]=100;
   CC.PersonAttribMax["��������"]=100;
   CC.PersonAttribMax["ˣ������"]=100;
   CC.PersonAttribMax["�������"]=100;
   CC.PersonAttribMax["��������"]=100;
   CC.PersonAttribMax["��ѧ��ʶ"]=100;
   CC.PersonAttribMax["Ʒ��"]=100;
   CC.PersonAttribMax["����"]=100;
   CC.PersonAttribMax["��������"]=100;

    CC.WarDataSize=186;         --ս�����ݴ�С  war.sta���ݽṹ
    CC.WarData_S={};        --ս�����ݽṹ
    CC.WarData_S["����"]={0,0,2};
    CC.WarData_S["����"]={2,2,10};
    CC.WarData_S["��ͼ"]={12,0,2};
    CC.WarData_S["����"]={14,0,2};
    CC.WarData_S["����"]={16,0,2};
    for i=1,6 do
        CC.WarData_S["�ֶ�ѡ���ս��"  .. i]={18+(i-1)*2,0,2};
        CC.WarData_S["�Զ�ѡ���ս��"  .. i]={30+(i-1)*2,0,2};
        CC.WarData_S["�ҷ�X"  .. i]={42+(i-1)*2,0,2};
        CC.WarData_S["�ҷ�Y"  .. i]={54+(i-1)*2,0,2};
    end
    for i=1,20 do
        CC.WarData_S["����"  .. i]={66+(i-1)*2,0,2};
        CC.WarData_S["�з�X"  .. i]={106+(i-1)*2,0,2};
        CC.WarData_S["�з�Y"  .. i]={146+(i-1)*2,0,2};
    end

    CC.WarWidth=64;        --ս����ͼ��С
    CC.WarHeight=64;




end
