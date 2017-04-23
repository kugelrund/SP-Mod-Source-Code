#ifndef __G_PUBLIC_H__
#define __G_PUBLIC_H__
// g_public.h -- game module information visible to server

#define	GAME_API_VERSION	6

// entity->svFlags
// the server does not know how to interpret most of the values
// in entityStates (level eType), so the game must explicitly flag
// special server behaviors
#define	SVF_NOCLIENT			0x00000001	// don't send entity to clients, even if it has effects
#define SVF_INACTIVE			0x00000002	// Can't be used when this flag is on
#define SVF_NPC					0x00000004
#define SVF_BOT					0x00000008
#define SVF_PLAYER_USABLE		0x00000010	// player can use this with the use button
#define	SVF_BROADCAST			0x00000020	// send to all connected clients
#define	SVF_PORTAL				0x00000040	// merge a second pvs at origin2 into snapshots
#define	SVF_USE_CURRENT_ORIGIN	0x00000080	// entity->currentOrigin instead of entity->s.origin
											// for link position (missiles and movers)
#define SVF_TRIMODEL			0x00000100	// Use a three piece model make up like a player does
#define	SVF_OBJECTIVE			0x00000200	// Draw it's name if crosshair comes across it
#define	SVF_ANIMATING			0x00000400	// Currently animating from startFrame to endFrame
#define	SVF_NPC_PRECACHE		0x00000800	// Tell cgame to precache this spawner's NPC stuff
#define	SVF_HEALING				0x00001000	// Being healed - Hack for medics
#define	SVF_NAVGOAL				0x00002000	// Navgoal
#define	SVF_NOPUSH				0x00004000	// Can't be pushed around
#define	SVF_MUST_HEAL			0x00008000	// Medic has to heal this guy
#define SVF_PLAT				0x00010000	// A func_plat or door acting like one
#define	SVF_BBRUSH				0x00020000	// breakable brush
#define	SVF_LOCKEDENEMY			0x00040000	// keep current enemy until dead
#define SVF_IGNORE_ENEMIES		0x00080000	// Ignore all enemies
#define SVF_BEAMING				0x00100000	// Being transported
#define SVF_PLAYING_SOUND		0x00200000	// In the middle of playing a sound
#define SVF_CUSTOM_GRAVITY		0x00400000	// Use personal gravity
#define SVF_BROKEN				0x00800000	// A broken misc_model_breakable
#define SVF_NO_TELEPORT			0x01000000	// Will not be teleported
#define SVF_NONNPC_ENEMY		0x02000000	// Not a client/NPC, but can still be considered a hostile lifeform
#define SVF_SELF_ANIMATING		0x04000000	// Ent will control it's animation itself in it's think func
#define SVF_STASIS_DOOR			0x08000000	// Ent is a stasis door
#define SVF_NO_BASIC_SOUNDS		0x10000000	// Don't load basic custom sound set
#define SVF_NO_COMBAT_SOUNDS	0x20000000	// Don't load combat custom sound set
#define SVF_NO_EXTRA_SOUNDS		0x40000000	// Don't load extra custom sound set
#define SVF_NO_SCAV_SOUNDS		0x80000000	// Don't load scavenger custom sound set
//===============================================================

typedef struct gentity_s gentity_t;
typedef struct gclient_s gclient_t;

typedef enum
{
	eNO = 0,
	eFULL,
	eAUTO,
} SavedGameJustLoaded_e;



#ifndef GAME_INCLUDE

// the server needs to know enough information to handle collision and snapshot generation

struct gentity_s {
	entityState_t	s;				// communicated by server to clients
	struct playerState_s	*client;
	qboolean	inuse;
	qboolean	linked;				// qfalse if not in any good cluster

	int			svFlags;			// SVF_NOCLIENT, SVF_BROADCAST, etc

	qboolean	bmodel;				// if false, assume an explicit mins / maxs bounding box
									// only set by gi.SetBrushModel
	vec3_t		mins, maxs;
	int			contents;			// CONTENTS_TRIGGER, CONTENTS_SOLID, CONTENTS_BODY, etc
									// a non-solid entity should set to 0

	vec3_t		absmin, absmax;		// derived from mins/maxs and origin + rotation

	// currentOrigin will be used for all collision detection and world linking.
	// it will not necessarily be the same as the trajectory evaluation for the current
	// time, because each entity must be moved one at a time after time is advanced
	// to avoid simultanious collision issues
	vec3_t		currentOrigin;
	vec3_t		currentAngles;

	gentity_t	*owner;				// objects never interact with their owners, to
									// prevent player missiles from immediately
									// colliding with their owner
	// the game dll can add anything it wants after
	// this point in the structure
};

#endif		// GAME_INCLUDE

//===============================================================

//
// functions provided by the main engine
//
typedef struct {
	//============== general Quake services ==================

	// print message on the local console
	void	(*Printf)( const char *fmt, ... );

	// Write a camera ref_tag to cameras.map
	void	(*WriteCam)( const char *text );

	// abort the game
	void	(*Error)( int, const char *fmt, ... );

	// get current time for profiling reasons
	// this should NOT be used for any game related tasks,
	// because it is not journaled
	int		(*Milliseconds)( void );

	// console variable interaction
	cvar_t	*(*cvar)( const char *var_name, const char *value, int flags );
	void	(*cvar_set)( const char *var_name, const char *value );
	int		(*Cvar_VariableIntegerValue)( const char *var_name );
	void	(*Cvar_VariableStringBuffer)( const char *var_name, char *buffer, int bufsize );

	// ClientCommand and ServerCommand parameter access
	int		(*argc)( void );
	char	*(*argv)( int n );

	int		(*FS_FOpenFile)( const char *qpath, fileHandle_t *file, fsMode_t mode );
	int		(*FS_Read)( void *buffer, int len, fileHandle_t f );
	int		(*FS_Write)( const void *buffer, int len, fileHandle_t f );
	void	(*FS_FCloseFile)( fileHandle_t f );
	int		(*FS_ReadFile)( const char *name, void **buf );
	void	(*FS_FreeFile)( void *buf );
	int		(*FS_GetFileList)(  const char *path, const char *extension, char *listbuf, int bufsize );

	// Savegame handling
	//
	qboolean	(*AppendToSaveGame)(unsigned long chid, void *data, int length);
	int			(*ReadFromSaveGame)(unsigned long chid, void *pvAddress, int iLength, void **ppvAddressPtr);
	int			(*ReadFromSaveGameOptional)(unsigned long chid, void *pvAddress, int iLength, void **ppvAddressPtr);

	// add commands to the console as if they were typed in
	// for map changing, etc
	void	(*SendConsoleCommand)( const char *text );


	//=========== server specific functionality =============

	// kick a client off the server with a message
	void	(*DropClient)( int clientNum, const char *reason );

	// reliably sends a command string to be interpreted by the given
	// client.  If clientNum is -1, it will be sent to all clients
	void	(*SendServerCommand)( int clientNum, const char *fmt, ... );

	// config strings hold all the index strings, and various other information
	// that is reliably communicated to all clients
	// All of the current configstrings are sent to clients when
	// they connect, and changes are sent to all connected clients.
	// All confgstrings are cleared at each level start.
	void	(*SetConfigstring)( int num, const char *string );
	void	(*GetConfigstring)( int num, char *buffer, int bufferSize );

	// userinfo strings are maintained by the server system, so they
	// are persistant across level loads, while all other game visible
	// data is completely reset
	void	(*GetUserinfo)( int num, char *buffer, int bufferSize );
	void	(*SetUserinfo)( int num, const char *buffer );

	// the serverinfo info string has all the cvars visible to server browsers
	void	(*GetServerinfo)( char *buffer, int bufferSize );

	// sets mins and maxs based on the brushmodel name
	void	(*SetBrushModel)( gentity_t *ent, const char *name );

	// collision detection against all linked entities
	void	(*trace)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );

	// point contents against all linked entities
	int		(*pointcontents)( const vec3_t point, int passEntityNum );

	qboolean	(*inPVS)( const vec3_t p1, const vec3_t p2 );
	qboolean	(*inPVSIgnorePortals)( const vec3_t p1, const vec3_t p2 );
	void		(*AdjustAreaPortalState)( gentity_t *ent, qboolean open );
	qboolean	(*AreasConnected)( int area1, int area2 );

	// an entity will never be sent to a client or used for collision
	// if it is not passed to linkentity.  If the size, position, or
	// solidity changes, it must be relinked.
	void	(*linkentity)( gentity_t *ent );
	void	(*unlinkentity)( gentity_t *ent );		// call before removing an interactive entity

	// EntitiesInBox will return brush models based on their bounding box,
	// so exact determination must still be done with EntityContact
	int		(*EntitiesInBox)( const vec3_t mins, const vec3_t maxs, gentity_t **list, int maxcount );

	// perform an exact check against inline brush models of non-square shape
	qboolean	(*EntityContact)( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );

	// sound ovveride values
	int		*S_Override;

	// dynamic memory allocator for things that need to be freed
	void	*(*Malloc)( int bytes );
	void	(*Free)( void *buf );
} game_import_t;

//
// functions exported by the game subsystem
//
typedef struct {
	int			apiversion;

	// init and shutdown will be called every single level
	// levelTime will be near zero, while globalTime will be a large number
	// that can be used to track spectator entry times across restarts
	void		(*Init)( const char *mapname, const char *spawntarget, int checkSum, const char *entstring, int levelTime, int randomSeed, int globalTime, SavedGameJustLoaded_e eSavedGameJustLoaded, qboolean qbLoadTransition );
	void		(*Shutdown) (void);

	// ReadLevel is called after the default map information has been
	// loaded with SpawnEntities
	void		(*WriteLevel) (qboolean qbAutosave);
	void		(*ReadLevel)  (qboolean qbAutosave, qboolean qbLoadTransition);
	qboolean	(*GameAllowedToSaveHere)(void);

	// return NULL if the client is allowed to connect, otherwise return
	// a text string with the reason for denial
	char		*(*ClientConnect)( int clientNum, qboolean firstTime, SavedGameJustLoaded_e eSavedGameJustLoaded );

	void		(*ClientBegin)( int clientNum, usercmd_t *cmd, SavedGameJustLoaded_e eSavedGameJustLoaded);
	void		(*ClientUserinfoChanged)( int clientNum );
	void		(*ClientDisconnect)( int clientNum );
	void		(*ClientCommand)( int clientNum );
	void		(*ClientThink)( int clientNum, usercmd_t *cmd );

	void		(*RunFrame)( int levelTime );

	// ConsoleCommand will be called when a command has been issued
	// that is not recognized as a builtin function.
	// The game can issue gi.argc() / gi.argv() commands to get the command
	// and parameters.  Return qfalse if the game doesn't recognize it as a command.
	qboolean	(*ConsoleCommand)( void );

	//
	// global variables shared between game and server
	//

	// The gentities array is allocated in the game dll so it
	// can vary in size from one game to another.
	// 
	// The size will be fixed when ge->Init() is called
	// the server can't just use pointer arithmetic on gentities, because the
	// server's sizeof(struct gentity_s) doesn't equal gentitySize
	struct gentity_s	*gentities;
	int			gentitySize;
	int			num_entities;		// current number, <= MAX_GENTITIES
} game_export_t;

game_export_t *GetGameApi (game_import_t *import);

#endif//#ifndef __G_PUBLIC_H__