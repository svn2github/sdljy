#include "quakedef.h"
#include "hidinputmonitor.h"

extern TPoint currentPosition;

static qboolean shift_down = qfalse;

int hid_to_quake[256] =
    {
    /*  00  */ 000,000,000,000,'a','b','c','d','e','f','g','h','i','j','k','l',
    /*  16  */ 'm','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2',
    /*  32  */ '3','4','5','6','7','8','9','0',K_ENTER,K_ESCAPE,K_BACKSPACE,K_TAB,K_SPACE,'+','´','{',
    /*  48  */ '¨','\'',000,']',000,000,',','.','-',000,000,000,000,000,000,000,
    /*  64  */ 000,000,000,000,000,000,000,000,000,000,000,K_PGUP,K_DEL,000,K_PGDN,K_RIGHTARROW,
    /*  80  */ K_LEFTARROW,K_DOWNARROW,K_UPARROW,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  96  */ 000,000,000,000,'<',000,000,000,000,000,000,000,000,000,000,000,
    /*  112 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  128 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  144 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  160 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  176 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  192 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  208 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  224 */ K_CTRL,K_SHIFT,K_ALT,'~',000,K_SHIFT,000,000,000,000,000,000,000,000,000,000,
    /*  240 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000
    };

int hid_to_quake_shifted[256] = 
    {
    /*  00  */ 000,000,000,000,'A','B','C','D','E','F','G','H','I','J','K','L',
    /*  16  */ 'M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','!','"',
    /*  32  */ '#','¤','%','&','/','(',')','=',K_ENTER,K_ESCAPE,K_BACKSPACE,K_TAB,K_SPACE,'?','\\','}',
    /*  48  */ '^','*',000,'[',000,000,';',':','_',000,000,000,000,000,000,000,
    /*  64  */ 000,000,000,000,000,000,000,000,000,000,000,K_PGUP,K_DEL,000,K_PGDN,K_RIGHTARROW,
    /*  80  */ K_LEFTARROW,K_DOWNARROW,K_UPARROW,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  96  */ 000,000,000,000,'>',000,000,000,000,000,000,000,000,000,000,000,
    /*  112 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  128 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  144 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  160 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  176 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  192 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  208 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,
    /*  224 */ K_CTRL,K_SHIFT,K_ALT,'~',000,K_SHIFT,000,000,000,000,000,000,000,000,000,000,
    /*  240 */ 000,000,000,000,000,000,000,000,000,000,000,000,000,000,000,000
    };

void CHIDEventMonitor::RunL()
    {
    THIDEvent hidEvent;
    iHIDClient->GetEvent( hidEvent );
    switch (hidEvent.Type())
        {
        case THIDEvent::EMouseEvent:
            {
            qboolean down = qfalse;
            TMouseEvent* mouse = hidEvent.Mouse();
            switch (mouse->Type())
                {
                case EEventRelativeXY:
                    currentPosition += mouse->iPosition;
                    break;
                case EEventButtonDown:
                    down = qtrue;
                case EEventButtonUp:
                    switch(mouse->iValue)
                        {
                        case EMouseButtonLeft:
                            Key_Event(K_MOUSE1, down);
                            break;
                        case EMouseButtonRight:
                            Key_Event(K_MOUSE1+1, down);
                            break;
                        case EMouseButtonMiddle:
                            Key_Event(K_MOUSE1+2, down);
                            break;
                        default:
                            break;
                        }
                    break;
                case EEventRelativeWheel:
                    if (mouse->iValue > 0)
                        {
                        Key_Event( K_MWHEELUP, qtrue);
                        Key_Event( K_MWHEELUP, qfalse );
                        }
                    else
                        {
                        Key_Event( K_MWHEELDOWN, qtrue);
                        Key_Event( K_MWHEELDOWN, qfalse );
                        }
                    break;
                default:
                    break;
                }
            }
            break;
        case THIDEvent::EKeyEvent:
            {
            THIDKeyEvent* key = hidEvent.Key();
            int hid_code = hid_to_quake[key->ScanCode()];
            switch (key->Type())
                {
                case EEventHIDKeyUp:
                    if (shift_down)
                        {
                        hid_code = hid_to_quake_shifted[key->ScanCode()];
                        }
                    if (hid_code == K_SHIFT)
                        {
                        shift_down = qfalse;
                        }
                    Key_Event(hid_code, qfalse);
                    
                    break;
                case EEventHIDKeyDown:
//                    Com_Printf( "EEventHIDKeyDown, code %d\n",key->ScanCode());
                    if (hid_code == K_SHIFT)
                        {
                        shift_down = qtrue;
                        }
                    if (shift_down)
                        {
                        hid_code = hid_to_quake_shifted[key->ScanCode()];
                        }
                    Key_Event(hid_code, qtrue);
                    break;
                default:
                    break;
                }
            }
            break;
        case THIDEvent::EConsumerEvent:
            {
            THIDConsumerEvent* consumer = hidEvent.Consumer();
            //Com_Printf( "consumer, code %d\n",consumer->ButtonCode());
            break;
            }
        default:
            break;
        }
    iHIDClient->EventReady( &iStatus );
    SetActive();
    }
    
void CHIDEventMonitor::DoCancel()
    {
    iHIDClient->EventReadyCancel();
    }

CHIDEventMonitor* CHIDEventMonitor::NewL()
    {
    CHIDEventMonitor* self = new (ELeave) CHIDEventMonitor;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CHIDEventMonitor::~CHIDEventMonitor()
    {
    Cancel();
    delete iHIDClient;
    }

CHIDEventMonitor::CHIDEventMonitor() 
        :CActive(CActive::EPriorityIdle)
    {
    }
        
void CHIDEventMonitor::ConstructL()
    {
    User::LeaveIfError(iHidLibrary.Load(_L("hidsrv.dll")));
    TLibraryFunction entry = iHidLibrary.Lookup(1);
    if (entry)
        {
        iHIDClient = (MHIDSrvClient*) entry();
        if (iHIDClient)
            {
            User::LeaveIfError(iHIDClient->Connect());
            iHIDClient->EventReady( &iStatus );
            CActiveScheduler::Add( this );
            SetActive();
            }
        }
    else
        {
        User::Leave(KErrNotFound);
        }
    }
