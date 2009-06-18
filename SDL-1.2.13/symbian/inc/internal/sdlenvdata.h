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

/*
    SDLdata.h
    Symbian OS services for SDL

    Markus Mertama
*/

#include"sdlepocapi.h"
#include"epoc_sdl.h"
#include"vectorbuffer.h"

class CEventQueue;
class CDsa;
class CSdlAppServ;
class CEikAppUi;

const TInt KVectorSize = 64;

NONSHARABLE_CLASS(EpocSdlEnvData)
    {
    public:
    void Free();
    void Delete();
    CEventQueue*            		iEventQueue;
    TMainFunc						iMain;
    TInt            				iEpocEnvFlags;
    int                     		iArgc;
    char**                  		iArgv;
    CDsa*                   		iDsa;
    CSdlAppServ*            		iAppSrv;
    TThreadId               		iId;
    CArrayFix<TSdlCleanupItem>*     iCleanupItems; 
    CEikAppUi*					    iAppUi;
    CSDL*							iSdl;
    CIdle* 							iCaller;
    TRequestStatus*				    iCallerStatus;
    CSDL::TAppOrientation   		iAppOrientation;   
    TInt iStackSize;
    };
  
 NONSHARABLE_CLASS(EnvUtils)
	{
public:
	static TBool IsOwnThreaded();
	static void DisableKeyBlocking();
	static TBool Rendezvous(RThread& aThread, TRequestStatus& aStatus);
	static TInt RunSingleThread();
	static TInt SetAppOrientation(CSDL::TAppOrientation);
	static CSDL::TAppOrientation AppOrientation();
	};
	
 class CEventQueue : public CBase, public MEventQueue
    {
    public:
        void ConstructL();
        ~CEventQueue();
    public:
        TInt Append(const TWsEvent& aEvent);
        const TWsEvent& Shift();
        void Lock();
        void Unlock();
        TBool HasData();
    private:
        TVector<TWsEvent, KVectorSize> iVector;
        RCriticalSection iCS;
    };
    


  