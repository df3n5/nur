#include "nxHumanGameView.h"

//#include <FTGL/ftgl.h>

static SDL_Surface* screen;
static nxSceneNode sceneNodes[NX_MAX_SCENENODES];
static nxUInt currentSceneNodeId;
static nxUInt playerId;

nxGameView* nxHumanGameView_alloc()
{
	nxGameView* res = (nxGameView*)nxMalloc(sizeof(nxGameView));

	res->init = nxHumanGameView_init0;
	res->update = nxHumanGameView_update;
	res->draw = nxHumanGameView_draw;
	res->shutdown = nxHumanGameView_shutdown;

	screen = NX_NULL;
	for(int i = 0; i<NX_MAX_SCENENODES ;i++)
	{
		nxSceneNode_init0(&sceneNodes[i]);
	}
	currentSceneNodeId = 0;

	playerId = -1;

	nxEventManager_addHandler(nxHumanGameView_handleEvent, (void*)res);

    for(int i = 0 ; i < NX_MAX_SOUNDS ; i++)
    {
        res->soundSources[i] = NX_NULL;
    }

	return res;
}

nxInt nxHumanGameView_init0(nxGameView* obj)
{
	if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
	{
        nxAssertFail(SDL_GetError());
		return 1;
	}

	if( (screen = SDL_SetVideoMode( NX_SCREEN_WIDTH, NX_SCREEN_HEIGHT, NX_SCREEN_BPP, SDL_OPENGL )) == 0 )
	{
        nxAssertFail(SDL_GetError());
		return 1;
	}

	if( init_GL() == 0 )
	{
        nxAssertFail("Can't initialise opengl.");
		return 1;
	}

    if( nxHumanGameView_init0Audio(obj) )
    {
        return 1;
    }

    if( nxHumanGameView_loadBackgrounds(obj) )
    {
        return 1;
    }

    /*
    int audio_rate = 22050;
    Uint16 audio_format = AUDIO_S16SYS;
    int audio_channels = 2;
    int audio_buffers = 4096;

    if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0)
    {
        fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
        nxAssertFail("Can't initialise audio.");
    }
    */

	SDL_WM_SetCaption( "nur", NULL );

	return 0;
}

void nxHumanGameView_update(nxGameView* obj, nxUInt deltaMilliseconds)
{
	SDL_Event event;
	while( SDL_PollEvent( &event ) )
	{
		if( event.type == SDL_QUIT )
		{
			nxEvent endGameEvent = {NX_EVT_ENDGAME, NX_NULL};
			nxEventManager_triggerEvent(endGameEvent);
		}
		else if (event.type == SDL_KEYDOWN)
		{
			//HERE, need to use malloc
			nxMovementEventData evtData = {playerId};

			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				nxEvent endGameEvent = {NX_EVT_ENDGAME, (void*)NX_NULL};
				nxEventManager_triggerEvent(endGameEvent);
			}
            else if (event.key.keysym.sym == SDLK_SPACE)
			{
                nxFireEventData evtData = {playerId};
				nxEvent ev = {NX_EVT_FIRE, (void*)&evtData};
				nxEventManager_triggerEvent(ev);
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
				nxEvent evt = {NX_EVT_STARTMOVEUP, &evtData};
				nxEventManager_triggerEvent(evt);
			}
			else if(event.key.keysym.sym == SDLK_a)
			{
				nxEvent evt = {NX_EVT_STARTMOVELEFT, &evtData};
				nxEventManager_triggerEvent(evt);
			}
			else if(event.key.keysym.sym == SDLK_s)
			{
				nxEvent evt = {NX_EVT_STARTMOVEDOWN, &evtData};
				nxEventManager_triggerEvent(evt);
			}
			else if(event.key.keysym.sym == SDLK_d)
			{
                nxEvent evt = {NX_EVT_STARTMOVERIGHT, &evtData};
                nxEventManager_triggerEvent(evt);
			}
		}
		else if (event.type == SDL_KEYUP)
		{
			nxMovementEventData evtData = {playerId};
			if(event.key.keysym.sym == SDLK_w)
			{
				nxEvent evt = {NX_EVT_ENDMOVEUP, &evtData};
				nxEventManager_triggerEvent(evt);
			}
			else if(event.key.keysym.sym == SDLK_a)
			{
				nxEvent evt = {NX_EVT_ENDMOVELEFT, &evtData};
				nxEventManager_triggerEvent(evt);
			}
			else if(event.key.keysym.sym == SDLK_s)
			{
				nxEvent evt = {NX_EVT_ENDMOVEDOWN, &evtData};
				nxEventManager_triggerEvent(evt);
			}
			else if(event.key.keysym.sym == SDLK_d)
			{
				nxEvent evt = {NX_EVT_ENDMOVERIGHT, &evtData};
				nxEventManager_triggerEvent(evt);
			}
		}
        else if(event.type == SDL_MOUSEBUTTONDOWN)
        {
            int x, y;
            nxUInt state = SDL_GetMouseState(&x, &y);
            if(SDL_BUTTON_LEFT == SDL_BUTTON(state))
            {
                nxFireEventData evtData = {playerId};
				nxEvent ev = {NX_EVT_FIRE, (void*)&evtData};
				nxEventManager_triggerEvent(ev);
            }
        }
	}

    //Now update the times of nodes
	for(int i = 0 ; i < NX_MAX_SCENENODES ; i++)
	{
		if(sceneNodes[i].valid)
        {
            if(sceneNodes[i].moving)
            {
                sceneNodes[i].animTime += deltaMilliseconds;
                //printf("ime is : %d \n", sceneNodes[i].animTime);

                if(sceneNodes[i].animTime >= sceneNodes[i].maxAnimTime)
                {
                    sceneNodes[i].animTime = 0;
                }
            }
            else
            {
                sceneNodes[i].animTime = 0;
            }
        }
    }
}

void nxHumanGameView_draw(nxGameView* obj)
{
	glClear( GL_COLOR_BUFFER_BIT );

	glLoadIdentity();

	for(int i = 0 ; i < NX_MAX_SCENENODES ; i++)
	{
		if(sceneNodes[i].valid)
        {
			glPushMatrix();
			nxHumanGameView_drawSceneNode(&sceneNodes[i]);
			glPopMatrix();
		}
	}

    /*
	//do test
	glBegin( GL_LINES );
		glColor4f( 1.0, 0.0, 0.0, 1.0 );
		glVertex3f( 0, 0, 0 );
		glVertex3f( NX_SCREEN_WIDTH, NX_SCREEN_HEIGHT, 0 );
		glVertex3f( 0, NX_SCREEN_HEIGHT, 0 );
		glVertex3f( NX_SCREEN_WIDTH, 0, 0 );
	glEnd();
	//end test
    */

    /*
    FTGLfont *font = ftglCreatePixmapFont("../media/fonts/FreeSerif.ttf");

    if(!font)
        return -1;

    ftglSetFontFaceSize(font, 72, 72);
    ftglRenderFont(font, "Hello World!", FTGL_RENDER_ALL);

   // ftglDestroyFont(font);
    */

	SDL_GL_SwapBuffers();
}

nxInt init_GL()
{

    glEnable( GL_TEXTURE_2D );

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glBlendFunc(GL_DST_COLOR,GL_ZERO);

	glClearColor( 0, 0, 0, 0 );
	//glClearColor( 1, 1, 1, 1 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, NX_SCREEN_WIDTH, NX_SCREEN_HEIGHT, 0, -1, 1 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	if( glGetError() != GL_NO_ERROR )
	{
		return 0;
	}


	glClearColor( 1, 1, 1, 1 );
	return 1;
}

nxInt nxHumanGameView_init0Audio(nxGameView* obj)
{
    if( SDL_Init(SDL_INIT_AUDIO) < 0 )
    {
        char buf[255];
        sprintf(buf, "Unable to start SDL_mixer audio: %s\n", SDL_GetError());
        nxAssertFail(buf);
    }

    int audio_rate = 22050;
    Uint16 audio_format = AUDIO_S16SYS;
    int audio_channels = 2;
    int audio_buffers = 4096;

    if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0)
    {
        char buf[255];
        sprintf(buf, "Unable to initialize audio: %s\n", Mix_GetError());
        nxAssertFail(buf);
    }

    obj->soundSources[0] = Mix_LoadWAV("../media/audio/level_t.wav");
    if(obj->soundSources[0] == NULL)
    {
        char buf[255];
        sprintf(buf, "Unable to load WAV file: %s\n", Mix_GetError());
        nxAssertFail(buf);
    }

    /*
    int channel = Mix_PlayChannel(-1, obj->soundSources[0], 0);
    if(channel == -1)
    {
        char buf[255];
        sprintf(buf, "Unable to play WAV file: %s\n", Mix_GetError());
        nxAssertFail(buf);
    }
    */

	return 0;
}

nxInt nxHumanGameView_loadBackgrounds(void* vobj)
{
	nxGameView* obj = (nxGameView*) vobj;

    nxUInt id = currentSceneNodeId++;
    sceneNodes[id].id = NX_MAX_SCENENODES; //not used
    sceneNodes[id].valid = 1;
    sceneNodes[id].pos.x = NX_SCREEN_WIDTH * 0.5f;
    sceneNodes[id].pos.y = NX_SCREEN_HEIGHT * 0.5f;
    sceneNodes[id].width = NX_SCREEN_WIDTH;
    sceneNodes[id].height = NX_SCREEN_HEIGHT;
    sceneNodes[id].type = NX_SN_BACKGROUND;
    sceneNodes[id].hasTex = 1;
    sceneNodes[id].isAnimated = NX_FALSE;
    sceneNodes[id].texId = nxTextureLoader_loadImageFromFilename("../media/tex/t.png");
    //sceneNodes[id].texId = nxTextureLoader_loadImageFromFilename("../media/levels/level_t.png");
}

void nxHumanGameView_shutdown(nxGameView* obj)
{
    nxTextureLoader_shutdown();
    for(int i = 0 ; i < NX_MAX_SOUNDS ; i++)
    {
        if(obj->soundSources[i] != NX_NULL)
        {
            Mix_FreeChunk(obj->soundSources[i]);
        }
    }

    Mix_CloseAudio();
    SDL_Quit();
	nxFree(obj);
}

void nxHumanGameView_handleEvent(nxEvent evt, void* vobj)
{
	nxGameView* obj = (nxGameView*) vobj;

	if(evt.type == NX_EVT_CREATEENT)
	{
        int id = currentSceneNodeId++;
		nxCreateEntityEventData* castData = (nxCreateEntityEventData*)evt.data;

        //printf("id is : %d, type is %d \n", id, castData->entity.type);

		//Now create a scenenode object from the entity object
		sceneNodes[id].id = castData->entity.id;
		sceneNodes[id].valid = 1;
		sceneNodes[id].pos = castData->entity.pos;
		sceneNodes[id].rot = castData->entity.rot;
        sceneNodes[id].width = castData->entity.width;
        sceneNodes[id].height = castData->entity.height;
        sceneNodes[id].hasTex = 0; //Default to having no tex
        sceneNodes[id].animTime = 0;
        sceneNodes[id].moving = 0;

		switch(castData->entity.type)
		{
			case NX_ENT_PLAYER:
				playerId = castData->entity.id;
				sceneNodes[id].type = NX_SN_PLAYER;
                sceneNodes[id].crouchingTexId = nxTextureLoader_loadImageFromFilename("../media/tex/man_crouching.png");
                sceneNodes[id].mainTexId = nxTextureLoader_loadImageFromFilename("../media/tex/man_still.png");
                sceneNodes[id].texId = sceneNodes[id].mainTexId;
                sceneNodes[id].animTexIds[0] = nxTextureLoader_loadImageFromFilename("../media/tex/man_walking_0.png");
                sceneNodes[id].animTexIds[1] = nxTextureLoader_loadImageFromFilename("../media/tex/man_walking_1.png");
                sceneNodes[id].hasTex = 1;
                sceneNodes[id].isAnimated = NX_TRUE;
                sceneNodes[id].animFrameTime = 200;
                sceneNodes[id].maxAnimTime = 400;
				break;
			case NX_ENT_PLATFORM:
				sceneNodes[id].type = NX_SN_PLATFORM;
                sceneNodes[id].height = castData->entity.height+1;
				break;
			case NX_ENT_BULLET:
				sceneNodes[id].type = NX_SN_BULLET;
                sceneNodes[id].texId = nxTextureLoader_loadImageFromFilename("../media/tex/bullet_0.png");
                sceneNodes[id].hasTex = 1;
				break;
			case NX_ENT_FIKE:
				sceneNodes[id].type = NX_SN_FIKE;
                sceneNodes[id].texId = nxTextureLoader_loadImageFromFilename("../media/tex/fike.png");
                sceneNodes[id].hasTex = 1;
				break;
			default:
				break;
		}
	}
	else if(evt.type == NX_EVT_UPDATEENT)
	{
		nxUpdateEntityEventData* castData = (nxUpdateEntityEventData*)evt.data;

		//printf("castData->entity.pos with id %d : <%f,%f>\n", castData->entity.id, castData->entity.pos.x, castData->entity.pos.y);

        nxSceneNode* node = &(sceneNodes[nxHumanGameView_getSceneNodeIdxWithEntityId(castData->entity.id)]);
		node->pos = castData->entity.pos;
		node->rot = castData->entity.rot;
		node->reversed = castData->entity.reversed;
		node->moving = castData->entity.moving;
	}
    else if(evt.type == NX_EVT_STARTMOVEUP)
    {
        //Play sound now.
    //    alSourcePlay (obj->soundSources[0]);
    }
    else if(evt.type == NX_EVT_CROUCH)
    {
		nxCrouchEventData* castData = (nxCrouchEventData*)evt.data;
        nxSceneNode* node = &(sceneNodes[nxHumanGameView_getSceneNodeIdxWithEntityId(castData->entityId)]);
		node->texId = node->crouchingTexId;
    }
    else if(evt.type == NX_EVT_UNCROUCH)
    {
		nxUnCrouchEventData* castData = (nxUnCrouchEventData*)evt.data;
        nxSceneNode* node = &(sceneNodes[nxHumanGameView_getSceneNodeIdxWithEntityId(castData->entityId)]);
		node->texId = node->mainTexId;
    }
}

void nxHumanGameView_toggleFullscreen()
{
	SDL_WM_ToggleFullScreen(screen);
}

void nxHumanGameView_drawSceneNode(nxSceneNode* node)
{
	//float x = 10.0f;
	//float y = 10.0f;
	nxFloat x = node->pos.x;
	nxFloat y = NX_SCREEN_HEIGHT - node->pos.y; //Conversion between physics and graphics coords
//	nxFloat rot = node->rot;
    nxFloat rot = -nxMath_radToDeg(node->rot);


    if(node->hasTex)
    {
        glEnable( GL_TEXTURE_2D );
        if(node->isAnimated)
        {
            if(node->moving)
            {
                //printf(" id %d node->animTime %d node->animFrameTime %d \n", node->id, node->animTime, node->animFrameTime);
                nxUInt frame = node->animTime / node->animFrameTime;

                //TODO:Move between tex ids based on time.
                glBindTexture(GL_TEXTURE_2D, node->animTexIds[frame]);
            }
            else
            {
                // load the texture
                glBindTexture(GL_TEXTURE_2D, node->texId);
            }
        }
        else
        {
            // load the texture
            glBindTexture(GL_TEXTURE_2D, node->texId);
        }
    }
    else
    {
        glDisable( GL_TEXTURE_2D );
    }


	switch(node->type)
	{
		case(NX_SN_PLAYER):
			glTranslatef( x, y, 0 );
			//glTranslatef( x+NX_PLAYER_HALFWIDTH, y+NX_PLAYER_HALFHEIGHT, 0 );
			//glRotatef( nxMath_radToDeg(rot), 0.0f, 0.0f, 1.0f );
			glRotatef( rot, 0.0f, 0.0f, 1.0f );
            glColor4f( 1.0, 1.0, 1.0, 1.0 );
            if(! node->reversed )
            {
                glBegin( GL_QUADS );
                    glTexCoord2f( 0.0f, 0.0f );
                    glVertex3f( -NX_PLAYER_HALFWIDTH, -NX_PLAYER_HALFHEIGHT, 0 );
                    glTexCoord2f( 0.0f, 1.0f );
                    glVertex3f( -NX_PLAYER_HALFWIDTH, NX_PLAYER_HALFHEIGHT, 0 );
                    glTexCoord2f( 1.0f, 1.0f );
                    glVertex3f( NX_PLAYER_HALFWIDTH, NX_PLAYER_HALFHEIGHT, 0 );
                    glTexCoord2f( 1.0f, 0.0f );
                    glVertex3f( NX_PLAYER_HALFWIDTH, -NX_PLAYER_HALFHEIGHT, 0 );
                glEnd();
            }
            else
            {
                glBegin( GL_QUADS );
                    glTexCoord2f( 1.0f, 0.0f );
                    glVertex3f( -NX_PLAYER_HALFWIDTH, -NX_PLAYER_HALFHEIGHT, 0 );
                    glTexCoord2f( 1.0f, 1.0f );
                    glVertex3f( -NX_PLAYER_HALFWIDTH, NX_PLAYER_HALFHEIGHT, 0 );
                    glTexCoord2f( 0.0f, 1.0f );
                    glVertex3f( NX_PLAYER_HALFWIDTH, NX_PLAYER_HALFHEIGHT, 0 );
                    glTexCoord2f( 0.0f, 0.0f );
                    glVertex3f( NX_PLAYER_HALFWIDTH, -NX_PLAYER_HALFHEIGHT, 0 );
                glEnd();
            }
			break;
		case(NX_SN_PLATFORM):
            {
            nxFloat halfWidth = node->width * 0.5f;
            nxFloat halfHeight = node->height * 0.5f;
			glTranslatef( x+halfWidth, y+halfHeight, 0 );
			//glRotatef( nxMath_radToDeg(rot), 0.0f, 0.0f, 1.0f );
			glRotatef( rot, 0.0f, 0.0f, 1.0f );
            glColor4f( 1.0f, 0.41, 0.7, 1.0 );
            glBegin( GL_QUADS );
                glTexCoord2f( 0.0f, 0.0f );
                glVertex3f( -halfWidth, -halfHeight, 0 );
                glTexCoord2f( 0.0f, 1.0f );
                glVertex3f( -halfWidth, halfHeight, 0 );
                glTexCoord2f( 1.0f, 1.0f );
                glVertex3f( halfWidth, halfHeight, 0 );
                glTexCoord2f( 1.0f, 0.0f );
                glVertex3f( halfWidth, -halfHeight, 0 );
            glEnd();
			break;
            }
		case(NX_SN_BULLET):
            {
            nxFloat halfWidth = node->width * 0.5f;
            nxFloat halfHeight = node->height * 0.5f;
			glTranslatef( x+halfWidth, y+halfHeight, 0 );
			//glRotatef( nxMath_radToDeg(rot), 0.0f, 0.0f, 1.0f );
			glRotatef( rot, 0.0f, 0.0f, 1.0f );
            glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
            glBegin( GL_QUADS );
                glTexCoord2f( 0.0f, 0.0f );
                glVertex3f( -halfWidth, -halfHeight, 0 );
                glTexCoord2f( 0.0f, 1.0f );
                glVertex3f( -halfWidth, halfHeight, 0 );
                glTexCoord2f( 1.0f, 1.0f );
                glVertex3f( halfWidth, halfHeight, 0 );
                glTexCoord2f( 1.0f, 0.0f );
                glVertex3f( halfWidth, -halfHeight, 0 );
            glEnd();
			break;
            }
        case(NX_SN_FIKE):
            {
                nxFloat halfWidth = node->width * 0.5f;
                nxFloat halfHeight = node->height * 0.5f;

                //glTranslatef( x+halfWidth, y+halfHeight, 0 );
                glTranslatef( x, y, 0 );
                //glRotatef( -nxMath_radToDeg(rot), 0.0f, 0.0f, 1.0f );
                glRotatef( rot, 0.0f, 0.0f, 1.0f );
                glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
                glBegin( GL_QUADS );
                    glTexCoord2f( 0.0f, 0.0f );
                    glVertex3f( -halfWidth, -halfHeight, 0 );
                    glTexCoord2f( 0.0f, 1.0f );
                    glVertex3f( -halfWidth, halfHeight, 0 );
                    glTexCoord2f( 1.0f, 1.0f );
                    glVertex3f( halfWidth, halfHeight, 0 );
                    glTexCoord2f( 1.0f, 0.0f );
                    glVertex3f( halfWidth, -halfHeight, 0 );
                glEnd();
                break;
            }
        case(NX_SN_BACKGROUND):
            {
                nxHumanGameView_drawRectangle(x,y,node->width,node->height, rot);
                break;
            }
        default:
            {
                nxAssertFail("SN definition not found.");
            }
    }
}

void nxHumanGameView_drawRectangle(nxFloat x, nxFloat y, nxFloat width, nxFloat height, nxFloat rot)
{
    nxFloat halfWidth = width * 0.5f;
    nxFloat halfHeight = height * 0.5f;

    glTranslatef( x, y, 0 );
    glRotatef( rot, 0.0f, 0.0f, 1.0f );
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glBegin( GL_QUADS );
        glTexCoord2f( 0.0f, 0.0f );
        glVertex3f( -halfWidth, -halfHeight, 0 );
        glTexCoord2f( 0.0f, 1.0f );
        glVertex3f( -halfWidth, halfHeight, 0 );
        glTexCoord2f( 1.0f, 1.0f );
        glVertex3f( halfWidth, halfHeight, 0 );
        glTexCoord2f( 1.0f, 0.0f );
        glVertex3f( halfWidth, -halfHeight, 0 );
    glEnd();
}

nxUInt nxHumanGameView_getSceneNodeIdxWithEntityId(nxUInt entityId)
{
    for(int i=0;i<NX_MAX_SCENENODES;i++)
    {
        if(sceneNodes[i].id == entityId)
        {
            return i;
        }
    }
}
