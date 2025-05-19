#include "ParticleModules_Collision.h"

#include "HAL/PlatformType.h"


struct FParticleEventInstancePayload
{
    uint32 bSpawnEventsPresent:1;
    uint32 bDeathEventsPresent:1;
    uint32 bCollisionEventsPresent:1;
    uint32 bBurstEventsPresent:1;

    int32 SpawnTrackingCount;
    int32 DeathTrackingCount;
    int32 CollisionTrackingCount;
    int32 BurstTrackingCount;
};
