#include "nxHumanGameView.h"

#include <nxCore/nxMM.h>
#include <nxCore/nxLog.h>
#include <nxEvent/nxEventManager.h>
#include <nxEvent/nxEventData.h>

static SDL_Surface* screen;
static nxSceneNode sceneNodes[NX_MAX_SCENENODES];
static nxUInt currentSceneNodeIdx;

nxGameView* nxHumanGameView_new()
{
	nxGameView* res = (nxGameView*)nxMalloc(sizeof(nxGameView));

	res->init = nxHumanGameView_init;
	res->update = nxHumanGameView_update;
	res->draw = nxHumanGameView_draw;
	res->shutdown = nxHumanGameView_shutdown;

	screen = NX_NULL;
	for(int i = 0;i<NX_MAX_SCENENODES;i++)
	{
		sceneNodes[i].id = -1;
	}
	currentSceneNodeIdx = 0;

	nxEventManager_addHandler(nxHumanGameView_handleEvent);

	return res;
}

nxInt nxHumanGameView_init(nxGameView* obj)
{
	if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 ) 
	{ 
		return 1; 
	} 

	if( (screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_OPENGL )) == 0 ) 
	{ 
		return 1; 
	} 

	if( init_GL() == 0 ) 
	{ 
		return 1; 
	} 

	SDL_WM_SetCaption( "OpenGL Test", NULL ); 

	return 0;
}

void nxHumanGameView_update(nxGameView* obj)
{
	SDL_Event event;
	while( SDL_PollEvent( &event ) ) 
	{ 
		if( event.type == SDL_QUIT ) 
		{ 
			nxEvent endGameEvent = {NX_EVT_ENDGAME, NX_NULL};
			nxEventManager_queueEvent(endGameEvent);
		} 
		else if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				nxEvent endGameEvent = {NX_EVT_ENDGAME, NX_NULL};
				nxEventManager_queueEvent(endGameEvent);
			}
			else if (event.key.keysym.sym == SDLK_f)
			{
				if (event.key.keysym.mod & KMOD_CTRL)
				{
					nxHumanGameView_toggleFullscreen(obj);
				}
			}
			else if(event.key.keysym.sym == SDLK_w)
			{
			}
			else if(event.key.keysym.sym == SDLK_a)
			{
			}
			else if(event.key.keysym.sym == SDLK_s)
			{
			}
			else if(event.key.keysym.sym == SDLK_d)
			{
			}
		}
		else if (event.type == SDL_KEYUP)
		{
			if(event.key.keysym.sym == SDLK_w)
			{
			}
			else if(event.key.keysym.sym == SDLK_a)
			{
			}
			else if(event.key.keysym.sym == SDLK_s)
			{
			}
			else if(event.key.keysym.sym == SDLK_d)
			{
			}
		}
	}
}

void nxHumanGameView_draw(nxGameView* obj)
{
	glClear( GL_COLOR_BUFFER_BIT ); 
	glLoadIdentity();


//
	float SQUARE_WIDTH=100.0f;
	float SQUARE_HEIGHT=50.0f;
	float x = 10.0f;
	float y = 10.0f;
	glTranslatef( x, y, 0 );
	glBegin( GL_QUADS ); 
		glColor4f( 1.0, 1.0, 1.0, 1.0 );
		glVertex3f( 0, 0, 0 ); 
		glVertex3f( SQUARE_WIDTH, 0, 0 ); 
		glVertex3f( SQUARE_WIDTH, SQUARE_HEIGHT, 0 ); 
		glVertex3f( 0, SQUARE_HEIGHT, 0 ); 
	glEnd();
//
	SDL_GL_SwapBuffers();
}

nxInt init_GL()
{
	glClearColor( 0, 0, 0, 0 );
	glMatrixMode( GL_PROJECTION ); 
	glLoadIdentity(); 
	glOrtho( 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1 );

	glMatrixMode( GL_MODELVIEW ); 
	glLoadIdentity();

	if( glGetError() != GL_NO_ERROR ) 
	{ 
		return 0; 
	}

	return 1;
}

void nxHumanGameView_shutdown(nxGameView* obj)
{
	nxFree(obj);
}

void nxHumanGameView_handleEvent(nxEvent evt)
{
	if(evt.type == NX_EVT_CREATEENT)
	{
		nxCreateEntityEventData* castData = (nxCreateEntityEventData*)evt.data;

		NX_LOG("nxHumanGameView", "handling create event.");
		//TODO:Now create a scenenode object from the entity object
		sceneNodes[currentSceneNodeIdx].id = castData->entity.id;
		sceneNodes[currentSceneNodeIdx].pos = castData->entity.pos;
		currentSceneNodeIdx++;
	}
}

void nxHumanGameView_toggleFullscreen()
{
	SDL_WM_ToggleFullScreen(screen);
}