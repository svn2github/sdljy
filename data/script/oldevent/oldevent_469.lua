--function oldevent_469()

    if instruct_28(0,90,100,0,37) ==true then    --  28(1C):判断WWW品德90-100否则跳转到:Label0
        instruct_1(1593,55,0);   --  1(1):[郭靖]说: 你在江湖上的作为，我夫妇*俩已有所闻，这书你就拿去*吧．
        instruct_0();   --  0(0)::空语句(清屏)
        instruct_2(148,1);   --  2(2):得到物品[射鵰英雄传][1]
        instruct_3(-2,1,-2,-2,467,-1,-1,-2,-2,-2,-2,-2,-2);   --  3(3):修改事件定义:当前场景:场景事件编号 [1]
        instruct_3(-2,2,-2,-2,468,-1,-1,-2,-2,-2,-2,-2,-2);   --  3(3):修改事件定义:当前场景:场景事件编号 [2]
        do return; end
    end    --:Label0

    instruct_1(1594,55,0);   --  1(1):[郭靖]说: 你目前的所作所为，还不配*称为一个侠者，再努力吧．*或者你想试试我的武功？
    instruct_0();   --  0(0)::空语句(清屏)

    if instruct_5(1,0) ==false then    --  5(5):是否选择战斗？是则跳转到:Label1
        do return; end
    end    --:Label1

    instruct_1(1595,0,1);   --  1(1):[WWW]说: 晚辈不才，*斗胆向前辈请教．
    instruct_0();   --  0(0)::空语句(清屏)

    if instruct_6(76,8,0,1) ==false then    --  6(6):战斗[76]是则跳转到:Label2
        instruct_0();   --  0(0)::空语句(清屏)
        instruct_13();   --  13(D):重新显示场景
        instruct_1(1599,0,1);   --  1(1):[WWW]说: 晚辈功夫不济，下回再登门*拜访二位．
        instruct_0();   --  0(0)::空语句(清屏)
        do return; end
    end    --:Label2

    instruct_0();   --  0(0)::空语句(清屏)
    instruct_13();   --  13(D):重新显示场景
    instruct_1(1596,55,0);   --  1(1):[郭靖]说: 你的功夫这麽强，希望你不*要滥用才是．这书拿去吧．
    instruct_0();   --  0(0)::空语句(清屏)
    instruct_1(1597,56,0);   --  1(1):[黄蓉]说: 等会儿，我也想领教一下，*看看他是不是真的有实力从*我桃花岛上取走这本书．
    instruct_0();   --  0(0)::空语句(清屏)

    if instruct_6(77,8,0,1) ==false then    --  6(6):战斗[77]是则跳转到:Label3
        instruct_0();   --  0(0)::空语句(清屏)
        instruct_13();   --  13(D):重新显示场景
        instruct_1(1599,0,1);   --  1(1):[WWW]说: 晚辈功夫不济，下回再登门*拜访二位．
        instruct_0();   --  0(0)::空语句(清屏)
        do return; end
    end    --:Label3

    instruct_0();   --  0(0)::空语句(清屏)
    instruct_13();   --  13(D):重新显示场景
    instruct_1(1598,0,1);   --  1(1):[WWW]说: 承蒙两位前辈高抬贵手，这*书我就拿走了．
    instruct_0();   --  0(0)::空语句(清屏)
    instruct_2(148,1);   --  2(2):得到物品[射鵰英雄传][1]
    instruct_3(-2,1,-2,-2,470,-1,-1,-2,-2,-2,-2,-2,-2);   --  3(3):修改事件定义:当前场景:场景事件编号 [1]
    instruct_3(-2,2,-2,-2,471,-1,-1,-2,-2,-2,-2,-2,-2);   --  3(3):修改事件定义:当前场景:场景事件编号 [2]
    instruct_56(20);   --  56(38):提高声望值20
--end

