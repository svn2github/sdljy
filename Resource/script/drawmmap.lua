function Game_MMap()      --主地图
    local direct = -1;
    local keypress = lib.GetKey();


		local x,y = JY.Base["人X"],JY.Base["人Y"];              --按照方向键要到达的坐标
    if keypress ~= -1 then
	    JY.Mytick=0;
	    if keypress == VK_MOUSE then
				JY.DestX, JY.DestY = lib.GetCurSorPos(x, y)
			elseif keypress==VK_ESCAPE then
				MMenu();
				if JY.Status==GAME_FIRSTMMAP then
					return ;
				end
			elseif keypress==VK_UP then
				direct=0;
			elseif keypress==VK_DOWN then
				direct=3;
			elseif keypress==VK_LEFT then
				direct=2;
			elseif keypress==VK_RIGHT then
				direct=1;
			end
    end


    
    if (JY.DestX ~= x or JY.DestY ~= y ) and (JY.DestX > -1 and JY.DestY > -1) then
    
    	if JY.DestX > x then
    		x = JY.Base["人X"] + 1
    	elseif JY.DestX < x then
    		x = JY.Base["人X"] - 1
    	end
    	
    	if JY.DestY < y then
    		y = JY.Base["人Y"] - 1
    	elseif JY.DestY > y then
    		y = JY.Base["人Y"] + 1
    	end
    	
    	AddMyCurrentPic();
    else
	    if direct ~= -1 then   --按下了光标键
	        AddMyCurrentPic();         --增加主角贴图编号，产生走路效果
	        x=JY.Base["人X"]+CC.DirectX[direct+1];
	        y=JY.Base["人Y"]+CC.DirectY[direct+1];
	        JY.Base["人方向"]=direct;
	
	    end
    end

    JY.SubScene=-1;
    for i = 0,JY.SceneNum-1 do   --判断是否进入子场景
        if CanEnterScene(i,x,y) then
        		JY.DestX, JY.DestY = -1,-1;
            JY.SubScene=i;
            break;
        end
    end

    if lib.GetMMap(x,y,3)==0 and lib.GetMMap(x,y,4)==0 then     --没有建筑，可以到达
        JY.Base["人X"]=x;
        JY.Base["人Y"]=y;
    else
    	JY.DestX, JY.DestY = -1,-1;
    end
    
    JY.Base["人X"]=limitX(JY.Base["人X"],10,CC.MWidth-10);           --限制坐标不能超出范围
    JY.Base["人Y"]=limitX(JY.Base["人Y"],10,CC.MHeight-10);

    lib.DrawMMap(JY.Base["人X"],JY.Base["人Y"],GetMyPic());             --显示主地图
    DrawString(10,CC.ScreenH-15,string.format("%d %d, %d %d",
    		JY.Base["人X"],JY.Base["人Y"],JY.DestX,JY.DestY) ,C_GOLD,16);
    ShowScreen();

    if JY.SubScene >= 0 then          --进入子场景
			lib.ShowSlow(50,1)
			JY.Status=GAME_SMAP;
			JY.MMAPMusic=-1;
			
			lib.PicInit();
			lib.PicLoadFile(CC.SMAPPicFile,0);
			lib.PicLoadFile(CC.HeadPicFile,1);
			lib.PicLoadFile(CC.ThingPicFile,2);
			
			PlayMIDI(JY.Scene[JY.SubScene]["进门音乐"]);
			JY.MyPic=GetMyPic();
			JY.Base["人X1"]=JY.Scene[JY.SubScene]["入口X"]
			JY.Base["人Y1"]=JY.Scene[JY.SubScene]["入口Y"]
			JY.SubSceneX=0;
			JY.SubSceneY=0;
			DrawSMap();
			lib.ShowSlow(50,0)
        --lib.WaitKey();
    end
    
end