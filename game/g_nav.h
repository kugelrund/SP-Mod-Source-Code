#ifndef __G_NAV_H__
#define __G_NAV_H__

#define	WAYPOINT_NONE	-1

#define MAX_WAYPOINT_REACHED_DIST_SQUARED	1024	//32 squared
#define	MAX_COLL_AVOID_DIST					128
#define	NAVGOAL_USE_RADIUS					16384	//Used to force the waypoint_navgoals with a manually set radius to actually do a DistanceSquared check, not just bounds overlap

extern	bool navCalculatePaths;

extern	bool NAVDEBUG_showNodes;
extern	bool NAVDEBUG_showEdges;
extern	bool NAVDEBUG_showTestPath;
extern	bool NAVDEBUG_showEnemyPath;
extern	bool NAVDEBUG_showCombatPoints;
extern	bool NAVDEBUG_showNavGoals;
extern	bool NAVDEBUG_showCollision;

extern	int	 NAVDEBUG_curGoal;

void NAV_Shutdown( void );
void NAV_CalculatePaths( const char *filename, int checksum );
void NAV_CalculateSquadPaths( const char *filename, int checksum );

void NAV_ShowDebugInfo( void );

int NAV_TestBestNode( vec3_t position, int startID, int endID );

qboolean NPC_GetMoveDirection( vec3_t out, float *distance );
void NPC_MoveToGoalExt( vec3_t point );
void NAV_FindPlayerWaypoint( void );
qboolean NAV_CheckAhead( gentity_t *self, vec3_t end, trace_t &trace, int clipmask );

void CG_DrawNode( vec3_t origin, int type );
void CG_DrawEdge( vec3_t start, vec3_t end, int type );
void CG_DrawRadius( vec3_t origin, unsigned int radius, int type );
void CG_DrawCombatPoint( vec3_t origin, int type );

#endif //#ifndef __G_NAV_H__
