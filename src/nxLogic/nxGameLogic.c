#include "nxGameLogic.h"

#include <nxLogic/nxPhysics.h>

//#include <chipmunk.h> //XXX:prob shouldn't be here

static int finished = 0;

nxGameLogic* nxGameLogic_alloc()
{
	nxGameLogic* res = (nxGameLogic*)nxMalloc(sizeof(nxGameLogic));

	int i = 0;
	for(i=0;i<NX_MAX_ENTITIES;i++)
	{
		nxEntity_init0(&res->entities[i]);
	}

	res->currentEntityId = 0;

	res->physics = nxPhysics_alloc(res);

	nxEventManager_addHandler(nxGameLogic_handleEvent, (void*)res);

	return res;
}

void nxGameLogic_shutdown(nxGameLogic* obj)
{
	nxPhysics_shutdown(obj->physics);
	nxFree(obj);
}

nxInt nxGameLogic_init0(nxGameLogic* obj)
{
	nxPhysics_init0(obj->physics); //Important to do this first.

	if(nxGameLogic_addPlayerEntity(obj))
	{
		return 1;
	}

	if(nxGameLogic_addPlatformEntity(obj,
               300.0f,
               100.0f,
               NX_SCREEN_WIDTH-600.0f,
               0.0f ))
	{
		return 1;
	}

	if(nxGameLogic_addPlatformEntity(obj,
               0.0f,
               0.0f,
               NX_SCREEN_WIDTH,
               0.0f ))
	{
		return 1;
	}

	if(nxGameLogic_addEnemyEntity(obj,
                NX_ENT_FIKE,
                20.0f,
                20.0f,
                30.0f,
                20.0f))
	{
		return 1;
	}

	return 0;
}

nxInt nxGameLogic_aiUpdate(nxGameLogic* obj, nxUInt timestep)
{
    float fikeSpeed = 500.0f;

	for(int i = 0 ; i < NX_MAX_ENTITIES ; i++)
	{
		if(obj->entities[i].type == NX_ENT_FIKE)
		{
            static float speed = 500.0f;
            /*
            if(obj->entities[i].pos.x > obj->entities[obj->playerId].pos.x)
            {
                speed = -fikeSpeed;
            }

            */
            if(obj->entities[i].pos.x > (NX_SCREEN_WIDTH - 20.0f))
            {
                speed = -fikeSpeed;
            }
            else if (obj->entities[i].pos.x < 20.0f)
            {
                speed = fikeSpeed;
            }
            nxVector2 vel = { speed, 0.0f };
            //nxPhysics_applyImpulseToEntity(obj->physics, obj->entities[i].id, &vel);
            nxPhysics_setLinearVel(obj->physics, obj->entities[i].id, &vel);
        }
    }
}

nxInt nxGameLogic_update(nxGameLogic* obj, nxUInt timestep)
{
	nxPhysics_update(obj->physics, 0.001f * (nxFloat)timestep);

    nxGameLogic_aiUpdate(obj, 0.001f * (nxFloat)timestep);

	if(finished)
	{
		return 1;
	}
	for(int i = 0 ; i < NX_MAX_ENTITIES ; i++)
	{
		if(! obj->entities[i].valid)
		{
			continue;
		}
		else
		{
			//TODO:Only update info if something has changed.
			nxUpdateEntityEventData evtData = { obj->entities[i] };

			nxEvent evt = {NX_EVT_UPDATEENT, (void*)&evtData};

			//Fire event
			nxEventManager_triggerEvent(evt);

		}
	}

	return 0;
}

//TODO: Remove these and replace with a much nicer constructor like function in nxEntity.c
nxInt nxGameLogic_addPlayerEntity(nxGameLogic* obj)
{
	nxUInt id = obj->currentEntityId++;
	obj->playerId = id;
    nxEntity* entity = &(obj->entities[id]);

	entity->type = NX_ENT_PLAYER;
	entity->id = id;
	entity->valid = 1;
	entity->reversed = 0;
	entity->pos.x = (NX_SCREEN_WIDTH / 2);
	entity->pos.y = (NX_SCREEN_HEIGHT / 2);

	entity->crouching = 0;
	entity->xKeys = 0.0f;
	entity->yKeys = 0.0f;

	entity->rot = 0.0f;
	entity->hasDoubleJumped = 0;
	nxPhysics_addEntity(obj->physics, &(obj->entities[id]));

	nxCreateEntityEventData createEvData = { obj->entities[id], 1 };
	nxEvent createEv = {NX_EVT_CREATEENT, &createEvData};
	//Fire event
	nxEventManager_triggerEvent(createEv);

	return 0;
}

nxInt nxGameLogic_addPlatformEntity(nxGameLogic* obj,
        nxFloat x,
        nxFloat y,
        nxFloat w,
        nxFloat h)
{
	nxUInt id = obj->currentEntityId++;
    nxEntity* entity = &(obj->entities[id]);
	entity->type = NX_ENT_PLATFORM;
	entity->id = id;
	entity->valid = 1;
    entity->moving = 0;
	entity->pos.x = x;
	entity->pos.y = y;
	entity->width = w;
	entity->height = h;
	entity->rot = 0.0f;

	nxPhysics_addEntity(obj->physics, entity);

	nxCreateEntityEventData createEvData = { obj->entities[id], 0 };
	nxEvent createEv = {NX_EVT_CREATEENT, &createEvData};
	nxEventManager_triggerEvent(createEv);

	return 0;
}

nxInt nxGameLogic_addEnemyEntity(nxGameLogic* obj,
        nxUInt type,
        nxFloat x,
        nxFloat y,
        nxFloat w,
        nxFloat h)
{
	nxUInt id = obj->currentEntityId++;
    nxEntity* entity = &(obj->entities[id]);
	entity->type = type;
	entity->id = id;
	entity->valid = 1;
	entity->pos.x = x;
	entity->pos.y = y;
	entity->width = w;
	entity->height = h;
	nxPhysics_addEntity(obj->physics, entity);
	nxCreateEntityEventData createEvData = { obj->entities[id], 0 };
	nxEvent createEv = { NX_EVT_CREATEENT, &createEvData };
	nxEventManager_triggerEvent( createEv );

    return 0;
}

nxInt nxGameLogic_addBulletEntity(nxGameLogic* obj,
        nxVector2 pos,
        nxVector2 vel)
{
	nxUInt id = obj->currentEntityId++;
    nxEntity* entity = &(obj->entities[id]);
	entity->type = NX_ENT_BULLET;
	entity->id = id;
	entity->valid = 1;
    entity->moving = 0;
	entity->pos.x = pos.x;
	entity->pos.y = pos.y;
	entity->width = NX_BULLET_HALFWIDTH * 2;
	entity->height = NX_BULLET_HALFHEIGHT * 2;
	entity->rot = 0.0f;

	nxPhysics_addEntity(obj->physics, entity);
	nxPhysics_applyImpulseToEntity(obj->physics, entity->id, &vel);
    //nxPhysics_setLinearVel(obj->physics, entity->id, vel);

	nxCreateEntityEventData createEvData = { obj->entities[id], 0 };
	nxEvent createEv = { NX_EVT_CREATEENT, &createEvData };
	nxEventManager_triggerEvent( createEv );

	return 0;
}

void nxGameLogic_handleEvent(nxEvent evt, void* vobj)
{
	nxGameLogic* obj = (nxGameLogic*)vobj;

	//TODO : #define these somewhere
	nxFloat xSpeed = 150.0f;
	nxFloat ySpeed = 500.0f;
	nxFloat yDoubleJumpDelta = 100.0f;
	nxFloat bulletSpeed = 1000.0f;
	nxFloat bulletXOffset = -60.0f;
	nxFloat bulletYOffset = 25.0f;
	nxFloat crouchingYOffset = 10.0f;

	if(evt.type == NX_EVT_ENDGAME)
	{
		finished = 1;
	}
    else if(evt.type == NX_EVT_FIRE)
    {
//        nxUInt entityId = *((nxUInt*)evt.data);
        nxFireEventData* evtData = (nxFireEventData*)evt.data;
        nxUInt entityId = evtData->entityId;
        nxVector2 vel = { -bulletSpeed, 0.0f };
        nxVector2 pos = obj->entities[entityId].pos;

        nxFloat modifiedBulletXOffset = bulletXOffset;
        nxFloat modifiedBulletYOffset = (obj->entities[entityId].crouching) ? (bulletYOffset - crouchingYOffset) :  bulletYOffset ;

        if(obj->entities[entityId].reversed)
        {
            vel.x = bulletSpeed;
            pos.x -= modifiedBulletXOffset;
            pos.y += modifiedBulletYOffset;
        }
        else
        {
            pos.x += modifiedBulletXOffset;
            pos.y += modifiedBulletYOffset;
        }
        nxInt res = nxGameLogic_addBulletEntity(obj,
                pos,
                vel);
        if(res==1)
        {
            nxAssertFail("Can't add bullet entity.");
        }
    }
	else if(evt.type == NX_EVT_STARTMOVEUP)
	{
        nxVector2 currentVel;
        nxPhysics_getLinearVel(obj->physics, obj->playerId, &currentVel);
        nxVector2 vel = { currentVel.x, ySpeed };
        if(currentVel.y > (0.0f + NX_FLOAT_DELTA) ||
            currentVel.y < (0.0f - NX_FLOAT_DELTA) )
        {
            if( ! obj->entities[obj->playerId].hasDoubleJumped)
            {
                vel.y += yDoubleJumpDelta;
                //Doing a double jump
                //nxPhysics_setLinearVel(obj->physics, obj->playerId, vel);
                obj->entities[obj->playerId].hasDoubleJumped = 1;
                obj->entities[obj->playerId].yKeys = 1.0f;
            }
        }
        else
        {
            //nxPhysics_setLinearVel(obj->physics, obj->playerId, vel);
            obj->entities[obj->playerId].hasDoubleJumped = 0;
            obj->entities[obj->playerId].yKeys = 0.0f;
        }
        //obj->currentPlayerVel = vel;

        obj->entities[obj->playerId].yKeys = 1.0f;
	}
	else if(evt.type == NX_EVT_STARTMOVEDOWN)
	{
        obj->entities[obj->playerId].crouching = 1;
        nxCrouchEventData evtData = {obj->playerId};
        nxEvent evt = {NX_EVT_CROUCH, &evtData};
        nxEventManager_triggerEvent(evt);

	}
	else if(evt.type == NX_EVT_STARTMOVELEFT)
	{
        nxVector2 currentVel;
        nxPhysics_getLinearVel(obj->physics, obj->playerId, &currentVel);
        nxEntity* player = &(obj->entities[obj->playerId]);
        nxVector2 vel = { -xSpeed, currentVel.y };
        if((! player->runningLeft) &&
                (player->allowedRunLeft))
        {
            vel.x *= 2;
            player->allowedRunLeft = NX_FALSE;
            player->runningLeft = NX_TRUE;
        }
        else
        {
            player->allowedRunLeft = NX_TRUE;
            nxProcess* waitProcess = nxProcess_alloc(NX_PROCESS_WAIT);
            waitProcess->waitTime = 200;

            nxProcess* evtProcess = nxProcess_alloc(NX_PROCESS_TRIGGEREVENT);
            nxStopAllowedRunLeftEventData* evtData = (nxStopAllowedRunLeftEventData*)nxMalloc(sizeof(nxStopAllowedRunLeftEventData));
            evtData->entityId = obj->playerId;
            nxEvent ev = {NX_EVT_FIRE, (void*)&evtData};
            nxEvent* evt = nxEvent_alloc(NX_EVT_STOPALLOWEDRUNLEFT);
            evt->data = evtData;
            evtProcess->event = evt;

            waitProcess->next = evtProcess;
            nxProcessManager_attach(nxProcessManager_getInstance(), waitProcess);
        }
        nxPhysics_setLinearVel(obj->physics, obj->playerId, &vel);

        obj->entities[obj->playerId].xKeys = -1.0f;

        //Reverse if we were still or moving right
        if(currentVel.x >= 0.0f)
        {
            obj->entities[obj->playerId].reversed = 0;
        }

        obj->entities[obj->playerId].moving = 1;
//        obj->currentPlayerVel = vel;
	}
	else if(evt.type == NX_EVT_STARTMOVERIGHT)
	{
        nxVector2 currentVel;
        nxPhysics_getLinearVel(obj->physics, obj->playerId, &currentVel);
        nxEntity* player = &(obj->entities[obj->playerId]);

        /*
        float xSpeedNew = xSpeed;
        if(nxEntity_isDoubleTap(&(obj->entities[obj->playerId]), currentVel.x))
        {
            xSpeedNew *= 2;
        }
        */

        nxVector2 vel = { xSpeed, currentVel.y };
        if((! player->runningRight) &&
                (player->allowedRunRight))
        {
            vel.x *= 2;
            player->allowedRunRight = NX_FALSE;
            player->runningRight = NX_TRUE;
        }
        else
        {
            player->allowedRunRight = NX_TRUE;
            nxProcess* waitProcess = nxProcess_alloc(NX_PROCESS_WAIT);
            waitProcess->waitTime = 200;

            nxProcess* evtProcess = nxProcess_alloc(NX_PROCESS_TRIGGEREVENT);
            nxStopAllowedRunRightEventData* evtData = (nxStopAllowedRunRightEventData*)nxMalloc(sizeof(nxStopAllowedRunRightEventData));
            evtData->entityId = obj->playerId;
            nxEvent ev = {NX_EVT_FIRE, (void*)&evtData};
            nxEvent* evt = nxEvent_alloc(NX_EVT_STOPALLOWEDRUNRIGHT);
            evt->data = evtData;
            evtProcess->event = evt;

            waitProcess->next = evtProcess;
            nxProcessManager_attach(nxProcessManager_getInstance(), waitProcess);
        }

        nxPhysics_setLinearVel(obj->physics, obj->playerId, &vel);

        obj->entities[obj->playerId].xKeys = 1.0f;

        if(currentVel.x <= 0.0f)
        {
            obj->entities[obj->playerId].reversed = 1;
        }
//        obj->currentPlayerVel = vel;
        obj->entities[obj->playerId].moving = 1;
	}
	else if(evt.type == NX_EVT_ENDMOVEUP)
	{
        //nxVector2 currentVel;
        //nxPhysics_getLinearVel(obj->physics, obj->playerId, &currentVel);
        //currentVel.y = 0;
        //nxPhysics_setLinearVel(obj->physics, obj->playerId, currentVel);
        obj->entities[obj->playerId].yKeys = 0.0f;
	}
	else if(evt.type == NX_EVT_ENDMOVEDOWN)
	{
        obj->entities[obj->playerId].crouching = 0;

        nxUnCrouchEventData evtData = {obj->playerId};
        nxEvent evt = {NX_EVT_UNCROUCH, &evtData};
        nxEventManager_triggerEvent(evt);
	}
	else if(evt.type == NX_EVT_ENDMOVELEFT)
	{
        nxVector2 currentVel;
        nxPhysics_getLinearVel(obj->physics, obj->playerId, &currentVel);
        nxEntity* player = &(obj->entities[obj->playerId]);
    //    if(currentVel.x < 0)
        if(player->xKeys < 0)
        {
            currentVel.x = 0;
            nxPhysics_setLinearVel(obj->physics, obj->playerId, &currentVel);

            player->xKeys = 0.0f;
        }

        player->runningLeft = NX_FALSE;

        player->moving = 0;
	}
	else if(evt.type == NX_EVT_ENDMOVERIGHT)
	{
        nxVector2 currentVel;
        nxPhysics_getLinearVel(obj->physics, obj->playerId, &currentVel);

        nxEntity* player = &(obj->entities[obj->playerId]);
//        if(currentVel.x > 0)
        if(player->xKeys > 0)
        {
            currentVel.x = 0;
            nxPhysics_setLinearVel(obj->physics, obj->playerId, &currentVel);

            player->xKeys = 0.0f;
        }

        player->runningRight = NX_FALSE;

        player->moving = 0;
	}
	else if(evt.type == NX_EVT_PHYSICSUPDATEENT)
	{
		nxPhysicsUpdateEntityEventData* castData = (nxPhysicsUpdateEntityEventData*)evt.data;
        nxUInt entityId = castData->entityId;
        obj->entities[entityId].pos.x = castData->pos.x;
        obj->entities[entityId].pos.y = castData->pos.y;
        obj->entities[entityId].rot = castData->rot;
    }
	else if(evt.type == NX_EVT_STOPALLOWEDRUNLEFT)
	{
		nxStopAllowedRunLeftEventData* castData = (nxStopAllowedRunLeftEventData*)evt.data;
        nxUInt entityId = castData->entityId;
        obj->entities[entityId].allowedRunLeft = NX_FALSE;
    }
	else if(evt.type == NX_EVT_STOPALLOWEDRUNRIGHT)
	{
		nxStopAllowedRunRightEventData* castData = (nxStopAllowedRunRightEventData*)evt.data;
        nxUInt entityId = castData->entityId;
        obj->entities[entityId].allowedRunRight = NX_FALSE;
    }
}
