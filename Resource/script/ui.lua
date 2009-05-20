
TalkEx = lib.TalkEx
--复杂版本对话
--s 字符串，必须加上*作为分行，如果里面没有*,则会自动加上
function luaTalkEx(s,headid,flag)          --复杂版本对话

    local xy={ [0]={headx=10,heady=10,talkx=120,talky=10,showhead=1},             --显示头像和对话的坐标
                   {headx=520,heady=360,talkx=220,talky=360,showhead=1},
                   {headx=10,heady=10,talkx=120,talky=10,showhead=0},
                   {headx=520,heady=360,talkx=220,talky=360,showhead=0},
                   {headx=520,heady=10,talkx=220,talky=10,showhead=1},
                   {headx=10,heady=360,talkx=110,talky=360,showhead=1}, }

    if flag<0 or flag>5 then
        flag=0;
    end

    if xy[flag].showhead==0 then
        headid=-1;
    end

	if string.find(s,"*") ==nil then
	    s=GenTalkString(s,12);
	end

    lib.GetKey();

    local startp=1
    local endp;
    local dy=0;
    while true do
        if dy==0 then
		    Cls();
            if headid>=0 then
                DrawBox(xy[flag].headx+1, xy[flag].heady, xy[flag].headx + 110-1, xy[flag].heady + 110,C_WHITE);
                lib.PicLoadCache(1,headid*2,xy[flag].headx+5,xy[flag].heady+5,1,1);
            end
            DrawBox(xy[flag].talkx+1, xy[flag].talky, xy[flag].talkx + 300-1, xy[flag].talky + 110,C_WHITE);
        end
        endp=string.find(s,"*",startp);
        if endp==nil then
            DrawString(xy[flag].talkx + 3, xy[flag].talky + 10 + dy * 30,string.sub(s,startp),C_WHITE,24);
            ShowScreen();
            WaitKey();
            break;
        else
            DrawString(xy[flag].talkx + 3, xy[flag].talky + 10 + dy * 30,string.sub(s,startp,endp-1),C_WHITE,24);
        end
        dy=dy+1;
        startp=endp+1;
        if dy>=3 then
            ShowScreen();
            WaitKey();
            dy=0;
        end
    end

	Cls();
end

--产生对话显示需要的字符串，即每隔n个中文字符加一个星号
function GenTalkString(str,n)              --产生对话显示需要的字符串
    local tmpstr="";
    for s in string.gmatch(str .. "*","(.-)%*") do           --去掉对话中的所有*. 字符串尾部加一个星号，避免无法匹配
        tmpstr=tmpstr .. s;
    end

    local newstr="";
    while #tmpstr>0 do
		local w=0;
		while w<#tmpstr do
		    local v=string.byte(tmpstr,w+1);          --当前字符的值
			if v>=128 then
			    w=w+2;
			else
			    w=w+1;
			end
			if w >= 2*n-1 then     --为了避免跨段中文字符
			    break;
			end
		end

        if w<#tmpstr then
		    if w==2*n-1 and string.byte(tmpstr,w+1)<128 then
				newstr=newstr .. string.sub(tmpstr,1,w+1) .. "*";
				tmpstr=string.sub(tmpstr,w+2,-1);
			else
				newstr=newstr .. string.sub(tmpstr,1,w)  .. "*";
				tmpstr=string.sub(tmpstr,w+1,-1);
			end
		else
		    newstr=newstr .. tmpstr;
			break;
		end
	end
    return newstr;
end

--最简单版本对话
function Talk(s,personid)            --最简单版本对话
    local flag;
    if personid==0 then
        flag=1;
	else
	    flag=0;
	end
	TalkEx(s,JY.Person[personid]["头像代号"],flag);
end

--通用菜单函数
-- menuItem 表，每项保存一个子表，内容为一个菜单项的定义
--          菜单项定义为  { ItemName,     菜单项名称字符串
--                          ItemFunction, 菜单调用函数，如果没有则为nil
--                          Visible       是否可见  0 不可见 1 可见
--                       }
--          菜单调用函数说明：         itemfunction(newmenu,id)
--
--       返回值
--              0 正常返回，继续菜单循环 1 调用函数要求退出菜单，不进行菜单循环
--
-- numItem      总菜单项个数
-- numShow      显示菜单项目，如果总菜单项很多，一屏显示不下，则可以定义此值
--                =0表示显示全部菜单项

-- (x1,y1),(x2,y2)  菜单区域的左上角和右下角坐标，如果x2,y2=0,则根据字符串长度和显示菜单项自动计算x2,y2
-- isBox        是否绘制边框，0 不绘制，1 绘制。若绘制，则按照(x1,y1,x2,y2)的矩形绘制白色方框，并使方框内背景变暗
-- isEsc        Esc键是否起作用 0 不起作用，1起作用
-- Size         菜单项字体大小
-- color        正常菜单项颜色，均为RGB
-- selectColor  选中菜单项颜色,
--;
-- 返回值  0 Esc返回
--         >0 选中的菜单项(1表示第一项)
--         <0 选中的菜单项，调用函数要求退出父菜单，这个用于退出多层菜单

function ShowMenu(menuItem,numItem,numShow,x1,y1,x2,y2,isBox,isEsc,size,color,selectColor)
	local s
	local out=""
	local w = 0
	for i=1, numItem do
		local name = menuItem[i][1]
		local func = "0"
		--if (menuItem[i][2] ~= nil) then
		--	func = menuItem[i][2]
		--end
		local bvb = menuItem[i][3]
		
		s = string.format("%s,%s,%d|", name, "test", bvb);
		
		out = out .. s
		if string.len(name) > w then w=string.len(name) end
	end
	
	if x2==0 then x2 = x1+w*8 end
	
	local ret = lib.ShowMenu(out, numShow, x1, y1, x2, y2, isBox, isEsc, color, selectColor)
	Cls();
	return ret
end

function NewGame()     --选择新游戏，设置主角初始属性
    LoadRecord(0); --  载入新游戏数据
    JY.Person[0]["姓名"]=CC.NewPersonName;

    while true do
        JY.Person[0]["内力性质"]=Rnd(2);
        JY.Person[0]["内力最大值"]=Rnd(20)+21;
        JY.Person[0]["攻击力"]=Rnd(10)+21;
        JY.Person[0]["防御力"]=Rnd(10)+21;
        JY.Person[0]["轻功"]=Rnd(10)+21;
        JY.Person[0]["医疗能力"]=Rnd(10)+21;
        JY.Person[0]["用毒能力"]=Rnd(10)+21;
        JY.Person[0]["解毒能力"]=Rnd(10)+21;
        JY.Person[0]["抗毒能力"]=Rnd(10)+21;
        JY.Person[0]["拳掌功夫"]=Rnd(10)+21;
        JY.Person[0]["御剑能力"]=Rnd(10)+21;
        JY.Person[0]["耍刀技巧"]=Rnd(10)+21;
        JY.Person[0]["特殊兵器"]=Rnd(10)+21;
        JY.Person[0]["暗器技巧"]=Rnd(10)+21;
        JY.Person[0]["生命增长"]=Rnd(5)+3;
        JY.Person[0]["生命最大值"]= JY.Person[0]["生命增长"]*3+29;

        local rate=Rnd(10);
        if rate<2 then
            JY.Person[0]["资质"]=Rnd(35)+30;
        elseif rate<=7 then
            JY.Person[0]["资质"]=Rnd(20)+60;
        else
            JY.Person[0]["资质"]=Rnd(20)+75;
        end

        JY.Person[0]["生命"]=JY.Person[0]["生命最大值"];
        JY.Person[0]["内力"]=JY.Person[0]["内力最大值"];

        Cls();
        local x1=10;
        local y1=140;
        local h=20;
        local w=80;
        local i=0;

        local function DrawAttrib(str)    --定义内部函数
        		if (JY.Person[0][str]>=30) then
        			lib.FillColor(x1+i*w+40, y1, x1+i*w+80, y1+20, C_GOLD)
        		end
        		
            DrawString(x1+i*w,y1,str,C_RED,24);
            DrawString(x1+i*w+40,y1,string.format("%5d",JY.Person[0][str]),C_WHITE,24);
            i=i+1;
        end

        DrawAttrib("内力");    DrawAttrib("攻击力");   DrawAttrib("轻功");    DrawAttrib("防御力");
        i=0; y1=160;
        DrawAttrib("生命");    DrawAttrib("医疗能力");  DrawAttrib("用毒能力"); DrawAttrib("解毒能力");
        i=0; y1=180;
        DrawAttrib("拳掌功夫"); DrawAttrib("御剑能力");  DrawAttrib("耍刀技巧"); DrawAttrib("暗器技巧");
        i=0; y1=200;
        DrawAttrib("资质");    DrawAttrib("抗毒能力");  DrawAttrib("生命增长"); DrawAttrib("内力性质");

        DrawString(10,220,"这样的属性满意吗(Y/N)?",C_GOLD,24);
        ShowScreen();
        local ok;
        while true do
            local keypress=WaitKey();
            if keypress==VK_Y then
                ok=1;
                break;
            elseif keypress==VK_N then
                ok=0;
                break;
            end
        end

        if ok==1 then
            break;
        end
    end
end