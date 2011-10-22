#include "nxPhysics.h"

#include <stdio.h>
#include <nxGameLogic.h>

nxPhysics* nxPhysics_new(nxGameLogic* gameLogic)
{
	nxPhysics* res = (nxPhysics*) nxMalloc(sizeof(nxPhysics));

    res->_gameLogic = gameLogic;
    res->_nextEntityId = 0;
    res->_currentPlatformId = 0;
	return res;
}

void nxPhysics_shutdown(nxPhysics* obj)
{
    for(int i=0;i<NX_MAX_ENTITIES;i++)
    {
        if(obj->_physicsEntities[i].valid)
        {
            cpShapeFree(obj->_physicsEntities[i].shape);
            cpBodyFree(obj->_physicsEntities[i].body);
        }
    }
	cpShapeFree(obj->_ground);
	cpSpaceFree(obj->_space);
	nxFree(obj);
}

nxInt nxPhysics_init(nxPhysics* obj)
{
	//Space init
	cpVect gravity = cpv(0, -200.0f);
	obj->_space = cpSpaceNew();
    cpSpaceSetIterations(obj->_space, 10);
	cpSpaceSetGravity(obj->_space, gravity);
    obj->_space->enableContactGraph = cpTrue;

    cpShape* shape;

	//Ground init
	shape = cpSegmentShapeNew(obj->_space->staticBody, cpv(0, 0), cpv(NX_SCREEN_WIDTH, 0), 0);
	cpSpaceAddShape(obj->_space, shape);
    shape->e = 1.0f; shape->u = 0.0f;
    obj->_ground = shape;

	//Left wall init
	shape = cpSegmentShapeNew(obj->_space->staticBody, cpv(0, 0), cpv(0, NX_SCREEN_HEIGHT), 0);
	cpSpaceAddShape(obj->_space, shape);
    shape->e = 1.0f; shape->u = 1.0f;
    obj->_leftWall = shape;

	//Right wall init
	shape = cpSegmentShapeNew(obj->_space->staticBody, cpv(NX_SCREEN_WIDTH, 0), cpv(NX_SCREEN_WIDTH, NX_SCREEN_HEIGHT), 0);
	cpSpaceAddShape(obj->_space, shape);
    shape->e = 1.0f; shape->u = 1.0f;
    obj->_rightWall = shape;

    /*
	// Add our one way segment
	shape = cpSpaceAddShape(obj->_space, cpSegmentShapeNew(obj->_space->staticBody, cpv(300,100), cpv(NX_SCREEN_WIDTH-300,100), 10.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction(shape, 0.0f);
	cpShapeSetCollisionType(shape, NX_PLATFORM_COLLISION_TYPE);
	
	obj->_oneWayPlatforms[0].valid = 1;
	obj->_oneWayPlatforms[0].n = cpv(0, 1); // let objects pass upwards
	cpShapeSetUserData(shape, &obj->_oneWayPlatforms[0]);

    cpSpaceAddCollisionHandler(obj->_space, NX_PLATFORM_COLLISION_TYPE, NX_PLAYER_COLLISION_TYPE, 
            NULL, platformPreSolve, NULL, NULL, NULL);
            */

    /*
	//Create ball
	cpFloat radius = NX_BALL_HALFWIDTH;
	cpFloat mass = 0.1;

	// The moment of inertia is like mass for rotation
	// Use the cpMomentFor*() functions to help you approximate it.
	cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

	// The cpSpaceAdd*() functions return the thing that you are adding.
	// It's convenient to create and add an object in one line.
	obj->ballBody = cpSpaceAddBody(obj->_space, cpBodyNew(mass, moment));
	cpBodySetPos(obj->ballBody, cpv(NX_SCREEN_WIDTH/2, NX_SCREEN_HEIGHT/2));
	cpBodySetAngle(obj->ballBody, 0.2f);
	cpBodySetTorque(obj->ballBody, 0.2f);

	// Now we create the collision shape for the ball.
	// You can create multiple collision shapes that point to the same body.
	// They will all be attached to the body and move around to follow it.
	obj->ballShape = cpSpaceAddShape(obj->_space, cpCircleShapeNew(obj->ballBody, radius, cpvzero));
	cpShapeSetFriction(obj->ballShape, 0.7);
    */

    for(int i = 0 ; i < NX_MAX_ENTITIES ; i++)
    {
        obj->_physicsEntities[i].valid = 0;
    }

	return 0;
}

void nxPhysics_update(nxPhysics* obj, nxFloat timestep)
{
    /*
    cpVect pos = cpBodyGetPos(obj->ballBody);
    cpVect vel = cpBodyGetVel(obj->ballBody);
    printf(
      "ballBody is at (%5.2f, %5.2f). It's velocity is (%5.2f, %5.2f)\n",
      pos.x, pos.y, vel.x, vel.y
    );
	//TODO: update event fired here?

    cpSpaceStep(obj->_space, timestep);
  */

    for(int i = 0 ; i < NX_MAX_ENTITIES ; i++)
    {
        if(obj->_physicsEntities[i].valid)
        {
            cpVect pos = cpBodyGetPos(obj->_physicsEntities[i].body);
            nxFloat rot = cpBodyGetAngle(obj->_physicsEntities[i].body);
//            cpVect vel = cpBodyGetVel(obj->_physicsEntities[i]);
            //Update entity
            nxVector2 vpos = {pos.x, pos.y};
            nxGameLogic_updateEntityState(obj->_gameLogic, obj->_physicsEntities[i].entityId, vpos, rot);
        }
    }

    cpSpaceStep(obj->_space, timestep);
}

void nxPhysics_addEntity(nxPhysics* obj, nxEntity* entity)
{
    switch(entity->type)
    {
        case NX_ENT_PLAYER: 
            {
            int id = obj->_nextEntityId++;
            obj->_physicsEntities[id].entityId = id;
            obj->_physicsEntities[id].valid = 1;

            cpFloat moment = cpMomentForBox(NX_PLAYER_MASS, NX_PLAYER_HALFWIDTH, NX_PLAYER_HALFHEIGHT);
            // The cpSpaceAdd*() functions return the thing that you are adding.
            // It's convenient to create and add an object in one line.
    //        cpBody* body = cpSpaceAddBody(obj->_space, cpBodyNew(NX_PLAYER_MASS, moment));
            cpBody* body = cpSpaceAddBody(obj->_space, cpBodyNew(NX_PLAYER_MASS, NX_INFINITY));
            cpBodySetPos(body, cpv(NX_SCREEN_WIDTH/2, NX_SCREEN_HEIGHT/2));
            cpBodySetAngle(body, 0.0f);
            cpBodySetTorque(body, 0.0f);
//            body->velocity_func = playerUpdateVelocity;

            // Now we create the collision shape for the ball.
            // You can create multiple collision shapes that point to the same body.
            // They will all be attached to the body and move around to follow it.
            cpShape* shape = cpSpaceAddShape(obj->_space, cpBoxShapeNew(body, NX_PLAYER_HALFWIDTH, NX_PLAYER_HALFHEIGHT));
            cpShapeSetCollisionType(shape, NX_PLAYER_COLLISION_TYPE);
            cpShapeSetFriction(shape, 0.7f);
            obj->_physicsEntities[id].shape = shape;
            obj->_physicsEntities[id].body = body;
            break;
            }
        case NX_ENT_PLATFORM:
            {
            nxUInt platformId = obj->_currentPlatformId++;
            printf("%f, %f, %f, %f \n", entity->pos.x,entity->pos.y, NX_SCREEN_HEIGHT-entity->height, entity->width);
            // Add our one way segment
            cpShape* shape = cpSpaceAddShape(obj->_space, 
                    cpSegmentShapeNew(obj->_space->staticBody, cpv(entity->pos.x,entity->pos.y), cpv(entity->pos.x+entity->width, entity->pos.y+entity->height), 10.0f));
            cpShapeSetElasticity(shape, 1.0f);
            cpShapeSetFriction(shape, 0.0f);
            cpShapeSetCollisionType(shape, NX_PLATFORM_COLLISION_TYPE);

            obj->_oneWayPlatforms[platformId].valid = 1;
            obj->_oneWayPlatforms[platformId].n = cpv(0, 1); // let objects pass upwards
            cpShapeSetUserData(shape, &obj->_oneWayPlatforms[platformId]);

            cpSpaceAddCollisionHandler(obj->_space, NX_PLATFORM_COLLISION_TYPE, NX_PLAYER_COLLISION_TYPE, 
                    NULL, platformPreSolve, NULL, NULL, NULL);
            break;
            }
    }
}

void nxPhysics_setLinearVel(nxPhysics* obj, nxUInt entityId, nxVector2 vel)
{
    cpVect velDelta;
    velDelta.x = vel.x;
    velDelta.y = vel.y;
    cpBodySetVel(obj->_physicsEntities[entityId].body, velDelta);
//    cpBodySetForce(obj->_physicsEntities[entityId].body, velDelta); //XXX
}

void nxPhysics_addLinearVel(nxPhysics* obj, nxUInt entityId, nxVector2 vel)
{
    cpVect velDelta;
    velDelta.x = vel.x;
    velDelta.y = vel.y;
    cpVect velBefore = cpBodyGetVel(obj->_physicsEntities[entityId].body);
    cpVect newVel = cpvadd(velDelta,velBefore);
    cpBodySetVel(obj->_physicsEntities[entityId].body, newVel);
}

void nxPhysics_getLinearVel(nxPhysics* obj, nxUInt entityId, nxVector2* res)
{
    cpVect vel = cpBodyGetVel(obj->_physicsEntities[entityId].body);
    nxVector2_fromCpVect(&vel, res);
}

/*
//--------------------------------------------------------------------------------------------
//Non ADT functions below
//--------------------------------------------------------------------------------------------
//HERE
void playerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt)
{
    nxEntity *entity = (nxEntity *)cpBodyGetUserData(body);
//	int jumpState = (ChipmunkDemoKeyboard.y > 0.0f);
	int jumpState = entity->jumping;
	
	// Grab the grounding normal from last frame
	cpVect groundNormal = cpvzero;
	cpBodyEachArbiter(playerBody, (cpBodyArbiterIteratorFunc)SelectPlayerGroundNormal, &groundNormal);
	
	grounded = (groundNormal.y > 0.0);
	if(groundNormal.y < 0.0f) remainingBoost = 0.0f;
	
	// Do a normal-ish update
	cpBool boost = (jumpState && remainingBoost > 0.0f);
	cpVect g = (boost ? cpvzero : gravity);
	cpBodyUpdateVelocity(body, g, damping, dt);
	
	// Target horizontal speed for air/ground control
	cpFloat target_vx = PLAYER_VELOCITY*ChipmunkDemoKeyboard.x;
	
	// Update the surface velocity and friction
	cpVect surface_v = cpv(target_vx, 0.0);
	playerShape->surface_v = surface_v;
	playerShape->u = (grounded ? PLAYER_GROUND_ACCEL/GRAVITY : 0.0);
	
	// Apply air control if not grounded
	if(!grounded){
		// Smoothly accelerate the velocity
		playerBody->v.x = cpflerpconst(playerBody->v.x, target_vx, PLAYER_AIR_ACCEL*dt);
	}
	
	body->v.y = cpfclamp(body->v.y, -FALL_VELOCITY, INFINITY);
}
*/
cpBool platformPreSolve(cpArbiter *arb, cpSpace *space, void *ignore)
{
    CP_ARBITER_GET_SHAPES(arb, a, b); 
    nxOneWayPlatform *platform = (nxOneWayPlatform *)cpShapeGetUserData(a);
     
    if(cpvdot(cpArbiterGetNormal(arb, 0), platform->n) < 0){ 
        cpArbiterIgnore(arb);
        return cpFalse;
    }   
    
    return cpTrue;
}
