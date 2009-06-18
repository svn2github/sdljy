/*
    SDL - Simple DirectMedia Layer

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


*/

#include"sdlenvdata.h" 
#include"dsa.h"
#include"appsrv.h"

class CCurrentAppUi;
class CEikonEnv;

extern void ContainerWindowChanged();
extern void PrepareContainerWindowChanged();

EpocSdlEnvData* gEpocEnv;

TSdlCleanupItem::TSdlCleanupItem(TSdlCleanupOperation aOperation, TAny* aItem, TBool aThreadCleanup) :
iOperation(aOperation), iItem(aItem), iThread(aThreadCleanup ? RThread().Id() : gEpocEnv->iId)
    {
    }

TBool EnvUtils::IsOwnThreaded()
    {
    return gEpocEnv->iEpocEnvFlags & CSDL::EOwnThread;
    }

MEventQueue& EpocSdlEnv::EventQueue()
    {
    __ASSERT_DEBUG(gEpocEnv != NULL, PANIC(KErrNotReady));
    return *gEpocEnv->iEventQueue;
    }


TBool EpocSdlEnv::Flags(TInt aFlag)
    {
	const TInt flag = gEpocEnv->iEpocEnvFlags & aFlag;
	return flag == aFlag;
    }

TInt EpocSdlEnv::Argc()
    {
    __ASSERT_DEBUG(gEpocEnv != NULL, PANIC(KErrNotReady));
    return gEpocEnv->iArgc;
    }
    
    
char** EpocSdlEnv::Argv()
    {
    __ASSERT_DEBUG(gEpocEnv != NULL, PANIC(KErrNotReady));
    return gEpocEnv->iArgv;
    }
    
    
TBool EpocSdlEnv::IsDsaAvailable()
    {
    __ASSERT_DEBUG(gEpocEnv != NULL, PANIC(KErrNotReady));
    return gEpocEnv->iDsa != NULL && gEpocEnv->iDsa->IsDsaAvailable();
    }

void EpocSdlEnv::PostUpdate()
    {
    while(EnvUtils::RunSingleThread()){}
    }
  
void EpocSdlEnv::WaitDsaAvailable()
	{
	EpocSdlEnv::ObserverEvent(MSDLObserver::EEventWindowNotAvailable, 0);
	gEpocEnv->iAppSrv->Request(CSdlAppServ::EAppSrvStopThread);
	if(EpocSdlEnv::Flags(CSDL::EEnableFocusStop))
		{
		EpocSdlEnv::ObserverEvent(MSDLObserver::EEventSuspend, 0);
		}
	}
	
void EpocSdlEnv::Suspend()
	{
	if(gEpocEnv->iDsa != NULL && (gEpocEnv->iDsa->Stopped() || EpocSdlEnv::Flags(CSDL::EEnableFocusStop)))
		{
		gEpocEnv->iDsa->SetSuspend();
		if(EnvUtils::IsOwnThreaded())
			{
			RThread().Suspend();
			EpocSdlEnv::ObserverEvent(MSDLObserver::EEventResume, 0);
			}
		}
	}
	
void EpocSdlEnv::SetWaitDsa()
	{
	if(!IsDsaAvailable())
		{
		if(EnvUtils::IsOwnThreaded())
			{
			RThread th;
			th.Open(gEpocEnv->iId);
			th.Suspend();
			th.Close();
			}
		if(gEpocEnv->iDsa != NULL)
			gEpocEnv->iDsa->SetSuspend(); 
		}
	}
	
void EpocSdlEnv::Resume()
	{
	if(gEpocEnv->iDsa != NULL)
		{
		gEpocEnv->iDsa->Resume();
		}
	if(EnvUtils::IsOwnThreaded())
		{
		RThread th;
		th.Open(gEpocEnv->iId);
		th.Resume();
		th.Close();
		}
	const TInt value = gEpocEnv->iAppSrv->ObserverEvent(MSDLObserver::EEventResume, 0);
	gEpocEnv->iAppSrv->HandleObserverValue(MSDLObserver::EEventResume, value, ETrue);
	}
    

TInt EpocSdlEnv::AllocSwSurface(const TSize& aSize, TDisplayMode aMode)
	{
	return gEpocEnv->iDsa->AllocSurface(EFalse, aSize, aMode);
	}
	
TInt EpocSdlEnv::AllocHwSurface(const TSize& aSize, TDisplayMode aMode)
	{
	return gEpocEnv->iDsa->AllocSurface(ETrue, aSize, aMode);
	}
		
	
void EpocSdlEnv::UnlockHwSurface()
	{
	gEpocEnv->iDsa->UnlockHwSurface();
	}
	
TUint8* EpocSdlEnv::LockHwSurface()
	{
	return gEpocEnv->iDsa->LockHwSurface();
	}


void EpocSdlEnv::UpdateSwSurface()
	{
	gEpocEnv->iDsa->UpdateSwSurface();
	}
	
TBool EpocSdlEnv::AddUpdateRect(TUint8* aAddress, const TRect& aUpdateRect, const TRect& aRect)
	{
	return gEpocEnv->iDsa->AddUpdateRect(aAddress, aUpdateRect, aRect);
	}
	
void EpocSdlEnv::Request(TInt aService)
    {
    __ASSERT_DEBUG(gEpocEnv != NULL, PANIC(KErrNotReady));
    gEpocEnv->iAppSrv->Request(aService);
    }
    
  
void EpocSdlEnv::CreateZoomer(const TSize& aRequestedSize)
    { 
    __ASSERT_DEBUG(gEpocEnv != NULL, PANIC(KErrNotReady));
    if(gEpocEnv->iDsa != NULL && gEpocEnv->iDsa->WindowSize() != aRequestedSize)
    	{
    	TRAP_IGNORE(gEpocEnv->iDsa->CreateZoomerL(aRequestedSize));
    	}
    }
 

TSize EpocSdlEnv::ScreenSize()
    {
    __ASSERT_DEBUG(gEpocEnv != NULL, PANIC(KErrNotReady));
    return gEpocEnv->iDsa == NULL ? TSize(0, 0) : gEpocEnv->iDsa->ScreenRect().Size();
    }

TSize EpocSdlEnv::WindowSize()
    { 
    __ASSERT_DEBUG(gEpocEnv != NULL, PANIC(KErrNotReady));
    return gEpocEnv->iDsa == NULL ? TSize(0, 0) : gEpocEnv->iDsa->WindowSize();
    }   
    
TDisplayMode EpocSdlEnv::DisplayMode()
    {
    return gEpocEnv->iDsa == NULL ? ENone : gEpocEnv->iDsa->DisplayMode();
    }
    
TPointerCursorMode EpocSdlEnv::PointerMode()
    {
    return static_cast<TPointerCursorMode>
    (gEpocEnv->iAppSrv->RequestValue(CSdlAppServ::EAppSrvWindowPointerCursorMode));
    }


TInt EpocSdlEnv::SetAppOrientation(CSDL::TAppOrientation aOrientation)
    {
    gEpocEnv->iAppSrv->SetParam(aOrientation);
    return static_cast<CSDL::TAppOrientation>
        (gEpocEnv->iAppSrv->RequestValue(EpocSdlEnv::ESetAppOrientation));
    }
    
TInt EpocSdlEnv::SetPalette(TInt aFirstcolor, TInt aColorCount, TUint32* aPalette)  
	{
	return 	gEpocEnv->iDsa->SetPalette(aFirstcolor, aColorCount, aPalette);
	}

void EpocSdlEnv::PanicMain(TInt aErr)
    {
    gEpocEnv->iAppSrv->PanicMain(aErr);
    }
    
    
TInt EpocSdlEnv::AppendCleanupItem(const TSdlCleanupItem& aItem)
    {
    TRAPD(err, gEpocEnv->iCleanupItems->AppendL(aItem));
    return err;
    }
    
void EpocSdlEnv::RemoveCleanupItem(TAny* aItem)
    {
    for(TInt i = 0; i < gEpocEnv->iCleanupItems->Count(); i++)
        {
        if(gEpocEnv->iCleanupItems->At(i).iItem == aItem)
            gEpocEnv->iCleanupItems->Delete(i);
        }
    }
    
void EpocSdlEnv::CleanupItems()     
	{
	const TThreadId id = RThread().Id();
	TInt last = gEpocEnv->iCleanupItems->Count() - 1;
	TInt i;
	for(i = last; i >= 0 ; i--)
        {
        TSdlCleanupItem& item = gEpocEnv->iCleanupItems->At(i);
        if(item.iThread == id)
        	{
        	item.iThread = TThreadId(0); 
        	item.iOperation(item.iItem);
        	}
        }
    last = gEpocEnv->iCleanupItems->Count() - 1;
	for(i = last; i >= 0 ; i--)
        {
        TSdlCleanupItem& item = gEpocEnv->iCleanupItems->At(i);
        if(item.iThread == TThreadId(0))
        	{
        	gEpocEnv->iCleanupItems->Delete(i);
        	}
        }
	}
	
void EpocSdlEnv::FreeSurface()
	{
	Request(CSdlAppServ::EAppSrvDsaStatus);
	if(gEpocEnv->iDsa != NULL)
	gEpocEnv->iDsa->Free();
	}
  
void EpocSdlEnv::LockPalette(TBool aLock)
	{
	gEpocEnv->iDsa->LockPalette(aLock);
	}
    
void EpocSdlEnv::ObserverEvent(TInt aService, TInt aParam)
	{
	const TBool sdlThread = RThread().Id() == gEpocEnv->iId;
	const TInt valuea = gEpocEnv->iAppSrv->ObserverEvent(aService, aParam);
	gEpocEnv->iAppSrv->HandleObserverValue(aService, valuea, !sdlThread);
	if(sdlThread)
		{
		gEpocEnv->iAppSrv->SetParam(aParam);
		const TInt valuet = gEpocEnv->iAppSrv->RequestValue(aService);
		gEpocEnv->iAppSrv->HandleObserverValue(aService, valuet, EFalse);	
		}
	}
			
    
TPoint EpocSdlEnv::WindowCoordinates(const TPoint& aPoint)    
    {
    return gEpocEnv->iDsa == NULL  ? aPoint :
    	gEpocEnv->iDsa->WindowCoordinates(aPoint);	
    }
    
void EpocSdlEnv::PanicMain(const TDesC& aInfo, TInt aErr)
    {
    gEpocEnv->iAppSrv->PanicMain(aInfo, aErr);
    }
//Dsa is a low priority ao, it has to wait if its pending event, but ws
//event has been prioritized before it
//this is not called from app thread!
void EpocSdlEnv::WaitDeviceChange() 
    {
  	LockPalette(ETrue);
    gEpocEnv->iAppSrv->RequestValue(CSdlAppServ::EAppSrvWaitDsa);
    const TSize sz = WindowSize();
    const TInt param = reinterpret_cast<TInt>(&sz);
    PrepareContainerWindowChanged();   
    ObserverEvent(MSDLObserver::EEventScreenSizeChanged, param);
    ContainerWindowChanged();	
    LockPalette(EFalse);
   // RThread().Suspend();
    }  
    
/*
 * This function try to find best fit if screen content does not fit in screen
 */
TBool EpocSdlEnv::AutoOrientation(const TSize& aSize)
    {
    const CSDL::TAppOrientation mode = gEpocEnv->iAppOrientation;

    const TSize screenSize = gEpocEnv->iDsa->ScreenRect().Size();
    
    if(mode == CSDL::EDefaultAppOrientation &&
            gEpocEnv->iDsa &&
            screenSize.iWidth != screenSize.iHeight)
        {
        CSDL::TAppOrientation dir = CSDL::EPortraitAppOrientation;
        
        const TBool landscape = screenSize.iWidth > screenSize.iHeight;
        
        //First we check if it windows entrily inside either of orientations
        if(!EpocSdlEnv::Flags(CSDL::EImageResizeZoomOut) || 
                (screenSize.iWidth == aSize.iWidth && screenSize.iHeight == aSize.iHeight))
            {
            if(screenSize.iWidth <= aSize.iWidth && screenSize.iHeight <= aSize.iHeight)
                {
                if(landscape)
                    dir = CSDL::ELandscapeAppOrientation;
                return KErrNone == EpocSdlEnv::SetAppOrientation(dir);
                }
            else if(screenSize.iWidth <= aSize.iHeight && screenSize.iHeight <= aSize.iWidth)
                {
                if(!landscape)
                    dir = CSDL::ELandscapeAppOrientation;
                return KErrNone == EpocSdlEnv::SetAppOrientation(dir);
                }
            }
        //Then try to find more suitable dimensions
        const TRect rect1 = CDsa::CalcResizeRect(screenSize, aSize);
        const TRect rect2 = CDsa::CalcResizeRect(TSize(screenSize.iHeight, screenSize.iWidth), aSize);
        
        const TInt area1 = rect1.Size().iWidth * rect1.Size().iHeight;
        const TInt area2 = rect2.Size().iWidth * rect2.Size().iHeight;
       
        //then better fit is the bigger intersection area 
        if(area1 > area2)
            {
            //current is better
            if(landscape)
                dir = CSDL::ELandscapeAppOrientation;
            }
        else if(area1 < area2)
            {
            //we have to turn the screen
            if(!landscape)
                dir = CSDL::ELandscapeAppOrientation;
            }
        else
            {
            return EFalse;
            }   
        return KErrNone == EpocSdlEnv::SetAppOrientation(dir);
        }
    return EFalse;
    }

LOCAL_C TBool CheckSdl() 
    {
    TInt isExit = ETrue;
    RThread sdl;
    if(sdl.Open(gEpocEnv->iId) == KErrNone)
        {
        if(sdl.ExitType() == EExitPending)
            {
            isExit = EFalse;
            }
        sdl.Close();
        }
    return isExit;
    }
    
void EpocSdlEnvData::Free()
    {
    
   	if(RThread().Id() == gEpocEnv->iId)
    	{
    	if(iDsa != NULL)
    		iDsa->Free();
    	return;
    	}
    	
   	__ASSERT_ALWAYS(iArgv == NULL || CheckSdl(), PANIC(KErrNotReady));
    }
 
 void EpocSdlEnvData::Delete()
    {   
    
 	for(TInt i = 0; i <= iArgc; i++)
    	{
    	if(iArgv != NULL)
    		User::Free( iArgv[i] );
    	}
        
    User::Free(iArgv);	
     
    iArgv = NULL; 
    iArgc = 0;
    
    delete iEventQueue;
    
    if(iDsa != NULL)
    	iDsa->Free();
    
	delete iDsa;
	delete iAppSrv;
	
   	delete gEpocEnv->iCaller;   
    }

TInt EpocSdlEnv::ApplyGlesDsa()
 	{
 	CDsa* dsa = NULL;
 	TRAPD(err, dsa = gEpocEnv->iDsa->CreateGlesDsaL());
 	gEpocEnv->iDsa = dsa;
 	return err;
 	}
 	
 RWindow* EpocSdlEnv::Window()
 	{
 	return gEpocEnv == NULL ? NULL : (gEpocEnv->iDsa == NULL ? NULL : gEpocEnv->iDsa->Window());
 	}
 
void EpocSdlEnv::SetOrientation(CSDL::TOrientationMode aOrientation)
 	{
 	gEpocEnv->iDsa->SetOrientation(aOrientation);
 	}

TInt EpocSdlEnv::StackSize()
    {
    return gEpocEnv->iStackSize;
    }

