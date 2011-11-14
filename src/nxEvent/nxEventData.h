#ifndef  NXEVENTDATA_H
#define  NXEVENTDATA_H

#include <nxEntity.h>
#include <nxAlgebra/nxVector2.h>
#include <nxCore/nxTypes.h>

typedef struct nxCreateEntityEventData
{
	nxEntity entity;
	nxUInt isPlayer;
} nxCreateEntityEventData;

typedef struct nxMovementEventData
{
	nxUInt entityId;
} nxMovementEventData;

typedef struct nxUpdateEntityEventData
{
	nxEntity entity;
} nxUpdateEntityEventData;

typedef struct nxFireEventData
{
	nxUInt entityId;
} nxFireEventData;

typedef struct nxPhysicsUpdateEntityEventData
{
	nxUInt entityId;
    nxVector2 pos;
    nxFloat rot;
} nxPhysicsUpdateEntityEventData;

typedef struct nxCrouchEventData
{
	nxUInt entityId;
} nxCrouchEventData;

typedef struct nxUnCrouchEventData
{
	nxUInt entityId;
} nxUnCrouchEventData;

#endif   // NXEVENTDATA_H
