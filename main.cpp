#include "gamelib.h"
#include "SDL.h"
SDL_Surface* theMainSurf = 0;
int GameLock = 0;

lua_State* lp = 0;


SDL_Surface* tmp = 0;
//int mmm=PGE_BLUE;
extern JYKey mpkey[];
extern int anykeypress;
extern int ghh;


int main(int argc, char* argv[])
{
	lua_State* L = lua_open();
	lp = L;
	luaL_openlibs(L);
	RegJyLib(L);

	JyInit(L);

	return 0;
}

//关闭所有已打开的资源, 退出
static void Quit()
{
	CloseVedio();
	SDL_Quit();

	exit(0);
}

