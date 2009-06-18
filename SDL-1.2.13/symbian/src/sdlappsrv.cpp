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

#include"appsrv.h"    
#include"sdlenvdata.h"
#include<sdl.h>

extern EpocSdlEnvData* gEpocEnv;

CSdlAppServ::CSdlAppServ() : CActive(CActive::EPriorityHigh), iMainId(RThread().Id())
    {
    }
    
    
MSDLObserver* CSdlAppServ::Observer()
	{
	return iObserver;
	}
	
	
void CSdlAppServ::SetObserver(MSDLObserver* aObserver)
	{
	iObserver = aObserver;
	}	
	
TInt CSdlAppServ::ObserverEvent(TInt aEvent, TInt aParam)
	{
	if(iObserver != NULL)
		{
		if(RThread().Id() == gEpocEnv->iId && EnvUtils::IsOwnThreaded())
			{
			return iObserver->SdlThreadEvent(aEvent, aParam);
			}
		else if(RThread().Id() == iMainId)
			{
			return iObserver->SdlEvent(aEvent, aParam);
			}
		PANIC(KErrNotSupported);
		}
	return 0;
	}
	
void CSdlAppServ::PanicMain(TInt aReason)    
    {
    iAppThread.Panic(RThread().Name(), aReason);
    }
    
void CSdlAppServ::PanicMain(const TDesC& aInfo, TInt aReason)    
    {
    iAppThread.Panic(aInfo, aReason);
    }    
    
void CSdlAppServ::ConstructL()
    {
    CActiveScheduler::Add(this);
    if(EnvUtils::IsOwnThreaded())
    	User::LeaveIfError(iSema.CreateLocal(1));
    iStatus = KRequestPending;
    iStatusPtr = &iStatus;
    SetActive();
    }
        
 CSdlAppServ::~CSdlAppServ()
    {
    Cancel();
    if(iSema.Handle() != NULL)
        iSema.Signal();
    iSema.Close();
    iAppThread.Close();
    }
    
TInt CSdlAppServ::Request(TInt aService)
    {
    if(EnvUtils::IsOwnThreaded())
    	{
    	if(RThread().Id() == iAppThread.Id())
    		return KErrBadHandle;
    	iSema.Wait();
    	}
    EnvUtils::RunSingleThread();
    iService = aService;
	iAppThread.RequestComplete(iStatusPtr, KErrNone); 
    return KErrNone;
    }
    
TInt CSdlAppServ::RequestValue(TInt aService)
    {
    Request(aService);
    Request(EAppSrvNoop);
    return iReturnValue;
    }
   
void CSdlAppServ::Init()
    {
    PANIC_IF_ERROR(iAppThread.Open(iMainId));
    }

void CSdlAppServ::SetParam(TInt aParam)
	{
	iReturnValue = aParam;
	}
	
void CSdlAppServ::HandleObserverValue(TInt aService, TInt aReturnValue, TBool aMainThread)
	{
	if(iObserver != NULL && aMainThread)
		{
		switch(aService)
			{
			case MSDLObserver::EEventScreenSizeChanged:
			if(aReturnValue == MSDLObserver::EScreenSizeChangedDefaultPalette)
				EpocSdlEnv::LockPalette(EFalse);
			break;
			}
		}
	if(!aMainThread && aService == MSDLObserver::EEventSuspend)
		{
		if(iObserver == NULL || 
		(gEpocEnv->iDsa != NULL && gEpocEnv->iDsa->Stopped() && aReturnValue != MSDLObserver::ESuspendNoSuspend))
			{
			EpocSdlEnv::Suspend();
			}
		}
	}

void CSdlAppServ::RunL()
    {
    if(iStatus == KErrNone)
        {
        switch(iService)
            {
            case CSdlAppServ::EAppSrvWaitDsa:
            	EpocSdlEnv::SetWaitDsa();
            	iReturnValue = EpocSdlEnv::IsDsaAvailable();
            	break;
           	 case CSdlAppServ::EAppSrvStopThread:
           	 	if(gEpocEnv->iDsa != NULL)
            		gEpocEnv->iDsa->SetSuspend();
            	break;
            case EpocSdlEnv::EDisableKeyBlocking:
                EnvUtils::DisableKeyBlocking();
                break;
         
            case EpocSdlEnv::ESetAppOrientation:
                iReturnValue = EnvUtils::SetAppOrientation(static_cast<CSDL::TAppOrientation>(iReturnValue));
                break;    
                
           
            case EAppSrvWindowPointerCursorMode:
                iReturnValue = gEpocEnv->iDsa != NULL ?
                 gEpocEnv->iDsa->Session().PointerCursorMode() : KErrNotReady; 
                break;
                
            case EAppSrvDsaStatus:
            	if(gEpocEnv->iDsa != NULL)
            		gEpocEnv->iDsa->Stop();
                iReturnValue = KErrNone;
                break;
            case CDsa::ERequestUpdate:
            	gEpocEnv->iDsa->UnlockHWSurfaceRequestComplete();
            	break;
            case EAppSrvNoop:
                break;
            case MSDLObserver::EEventResume:
            case MSDLObserver::EEventSuspend:
            case MSDLObserver::EEventScreenSizeChanged:
            case MSDLObserver::EEventWindowReserved:
            case MSDLObserver::EEventKeyMapInit:
            case MSDLObserver::EEventWindowNotAvailable:
            case MSDLObserver::EEventMainExit:
            	iReturnValue = ObserverEvent(iService, iReturnValue);
            	HandleObserverValue(iService, iReturnValue, ETrue);
            	break;
            default:
                PANIC(KErrNotSupported);
            }
        iStatus = KRequestPending;
        iStatusPtr = &iStatus;
        SetActive();
        }
    if(EnvUtils::IsOwnThreaded())
    	iSema.Signal();
    }
    
void CSdlAppServ::DoCancel()
    {
    if(EnvUtils::IsOwnThreaded())
    	iSema.Wait();
    TRequestStatus* s = &iStatus;
    iAppThread.RequestComplete(s, KErrCancel); 
    }
    
 