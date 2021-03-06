#ifndef  NXPHYSICS_H
#define  NXPHYSICS_H

#include <chipmunk/chipmunk.h>

#include <nxAlgebra/nxVector2.h>
#include <nxCore/nxTypes.h>
#include <nxCore/nxConstants.h>
#include <nxCore/nxMM.h>
#include <nxCore/nxMath.h>
#include <nxCore/nxLog.h>
#include <nxLogic/nxEntity.h>
#include <nxEvent/nxEvent.h>
#include <nxEvent/nxEventData.h>
#include <nxEvent/nxEventManager.h>

//#include <nxLogic/nxGameLogic.h>

#define NX_INFINITY __builtin_inf()

//#define NX_PLAYER_MASS 1.0f
#define NX_PLAYER_MASS 10000.0f
#define NX_BULLET_MASS 0.3f
#define NX_FIKE_MASS 1000.0f

//#define NX_GRAVITY 20.0f
#define NX_GRAVITY 9.8f

#define NX_PLATFORM_COLLISION_TYPE 1
#define NX_PLAYER_COLLISION_TYPE 2
#define NX_BULLET_COLLISION_TYPE 3

//#define NX_PLAYER_MASS NX_INFINITY

//#define NX_JUMP_HEIGHT 2050.0
#define NX_JUMP_HEIGHT 8000.0f

//#define NX_JUMP_BOOST_HEIGHT 155.0
#define NX_JUMP_BOOST_HEIGHT 1000.0
#define NX_FALL_SPEED 400.0f

//#define NX_PLAYER_VELOCITY 200.0
#define NX_PLAYER_SPEED 2000.0f

#define NX_PLAYER_JUMP_SPEED 500.0f

//#define NX_PLAYER_GROUND_ACCEL_TIME 0.2
#define NX_PLAYER_GROUND_ACCEL_TIME 10000.2f
#define NX_PLAYER_GROUND_ACCEL (NX_PLAYER_SPEED/NX_PLAYER_GROUND_ACCEL_TIME)

//#define NX_PLAYER_AIR_ACCEL_TIME 0.25
#define NX_PLAYER_AIR_ACCEL_TIME 19.25f
#define NX_PLAYER_AIR_ACCEL (NX_PLAYER_SPEED/NX_PLAYER_AIR_ACCEL_TIME)

struct nxGameLogic;
//typedef struct nxGameLogic nxGameLogic;

/**
 * Physical attributes of an entity.
 **/
typedef struct nxPhysicsEntity
{
    nxUInt entityId;
    nxUInt valid;
	cpShape* shape;
	cpBody* body;
    nxEntity* entity;
} nxPhysicsEntity;

typedef struct nxOneWayPlatform
{
    cpVect n; // direction objects may pass through
    nxUInt valid;
} nxOneWayPlatform;

/**
 * Encapsulates the physics for the game.
 **/
typedef struct nxPhysics
{
    /*
	cpShape* ballShape;
	cpBody* ballBody;
    */
    struct nxGameLogic* _gameLogic;
	cpSpace* _space;
	cpShape* _ground;
	cpShape* _leftWall;
	cpShape* _rightWall;

    nxUInt _nextEntityId;
    nxUInt _currentPlatformId;

    nxPhysicsEntity _physicsEntities[NX_MAX_ENTITIES];
    nxOneWayPlatform _oneWayPlatforms[NX_MAX_ONEWAYPLATFORMS];
} nxPhysics;

nxPhysics* nxPhysics_alloc(struct nxGameLogic* gameLogic);
void nxPhysics_shutdown(nxPhysics* obj);
nxInt nxPhysics_init0(nxPhysics* obj);
void nxPhysics_update(nxPhysics* obj, nxFloat timestep);
void nxPhysics_addEntity(nxPhysics* obj, nxEntity* entity);
void nxPhysics_setLinearVel(nxPhysics* obj, nxUInt entityId, const nxVector2* vel);
void nxPhysics_addLinearVel(nxPhysics* obj, nxUInt entityId, nxVector2 vel);
void nxPhysics_getLinearVel(nxPhysics* obj, nxUInt entityId, nxVector2* res);
void nxPhysics_applyImpulseToEntity(nxPhysics* obj, nxUInt entityId, const nxVector2* vel);

void playerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt);
cpBool platformPreSolve(cpArbiter *arb, cpSpace *space, void *ignore);
cpBool bulletPreSolve(cpArbiter *arb, cpSpace *space, void *ignore);

#endif   // NXPHYSICS_H
