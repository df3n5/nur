#include <stdio.h>

#include <AL/alut.h>

#include <nxLogic/nxGameLogic.h>
#include <nxView/nxHumanGameView.h>
#include <nxEvent/nxEventManager.h>
#include <nxCore/nxMM.h>
#include <nxCore/nxLog.h>
#include <nxLogic/nxProcessManager.h>

#define FRAMES_PER_SECOND 60

static int FRAME_TIME = 1000 / FRAMES_PER_SECOND;

int main(int argc,char **argv)
{
    //Audio Init
    if(alutInit (&argc, argv) == AL_FALSE)
    {
        nxAssertFail(alutGetErrorString (alutGetError ()));
    }

	nxEventManager_init0();

    nxProcessManager_init(nxProcessManager_getInstance());

    nxGameView* gameView = nxHumanGameView_alloc();
    gameView->init(gameView);

    nxGameLogic* gameLogic = nxGameLogic_alloc();
	nxGameLogic_init0(gameLogic);

	nxInt finished = 0;
	nxUInt startTime = SDL_GetTicks();
	nxUInt lastFrameTime = 0;
	while( ! finished) {
        nxProcessManager_update(nxProcessManager_getInstance(), lastFrameTime);

		nxEventManager_handleEvents();
		finished = nxGameLogic_update(gameLogic, lastFrameTime);

		gameView->update(gameView, lastFrameTime);
		gameView->draw(gameView);

        nxUInt now = SDL_GetTicks();
        nxUInt timeDelta = now - startTime;

		//limit frame rate
		if( timeDelta < FRAME_TIME )
		{ 
			SDL_Delay( FRAME_TIME - timeDelta ); 
		}

        lastFrameTime = SDL_GetTicks() - startTime;
        startTime = SDL_GetTicks();
	}

	nxGameLogic_shutdown(gameLogic);
	gameView->shutdown(gameView);

    if (!alutExit ())
    {
        nxAssertFail(alutGetErrorString (alutGetError ()));
    }

    return 0;
}
