#ifndef SDLAPPH
#define SDLAPPH
#if defined (UIQ2) || defined (UIQ3) || defined (S60V3)
#define EPOC_AS_APP
#endif
#if defined (__AVKON_ELAF__)
#include <aknapp.h>
class CSDLApp:public CAknApplication
#elif defined (UIQ) || defined(UIQ3)
#include <qikapplication.h>
class CSDLApp:public CQikApplication
#else
#include <eikapp.h>
class CSDLApp:public CEikApplication
#endif
{
public:
	CSDLApp(){};
	~CSDLApp();
	CApaDocument*		CreateDocumentL();
	static TFileName	GetExecutablePath();
	static char*		GetExecutablePathCStr();

	/**
	 * This has a default empty implementation.
	 * Is called just before SDL_Main is called to allow init of system vars
	 */
	virtual	void PreInitializeAppL();
#if defined(UIQ3)
	/**
			 * Returns the resource id to be used to declare the views supported by this UIQ3 app
			 * @return TInt, resource id
			 */
	virtual TInt		ViewResourceId() = 0;
#endif			
};
#endif

