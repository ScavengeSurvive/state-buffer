// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_ini
	#undef _inc_ini
#endif
// custom include-guard to ensure we don't duplicate
#if defined _ini_included
	#endinput
#endif
#define _ini_included


#include <a_samp>
#include <YSI\y_iterate>
#include <YSI\y_hooks>


// MAX_STATE_BUFFER_SIZE is the maximum amount of frames that are stored
#if !defined MAX_STATE_BUFFER_SIZE
    #define MAX_STATE_BUFFER_SIZE (256)
#endif

// STATE_BUFFER_GRANULARITY represents the time in milliseconds between each frame being captured
#if !defined STATE_BUFFER_GRANULARITY
    #define STATE_BUFFER_GRANULARITY (100)
#endif

// STATE_BUFFER_TIME is a macro for calculating the length of history
#define STATE_BUFFER_TIME (MAX_STATE_BUFFER_SIZE * STATE_BUFFER_GRANULARITY / 1000)

forward _stateBuffer_captureFrame();


static enum E_BUFFERED_STATE {
    timestamp, // frame timestamp
    tickstamp, // GetTickCount result - this is stored for higher granularity
    gameState, // the result of GetPlayerState

    // this is the on-foot position, from GetPlayerPos and GetPlayerFacingAngle
    Float:posPedX,
    Float:posPedY,
    Float:posPedZ,
    Float:rotPedZ,

    // velocity is stored while on foot and in a vehicle
    Float:velocityX,
    Float:velocityY,
    Float:velocityZ,

    // vehicle-specific position data captured only when IsPlayerInAnyVehicle
    Float:posVehX,
    Float:posVehY,
    Float:posVehZ,
    Float:rotVehW,
    Float:rotVehX,
    Float:rotVehY,
    Float:rotVehZ,
}

static
    stateBufferFrames[MAX_PLAYERS][MAX_STATE_BUFFER_SIZE][E_BUFFERED_STATE], // all the frames for all players
    stateBufferFrame[MAX_PLAYERS];                                           // the current frame, wraps to zero at MAX_STATE_BUFFER_SIZE


// -
// API
// -


// GetPlayerLatestStateFrame stores into `frame` the latest frame in the state buffer.
stock GetPlayerLatestStateFrame(playerid, frame[E_BUFFERED_STATE]) {
    if(!IsPlayerConnected(playerid)) {
        return 1;
    }

    frame = stateBufferFrames[playerid][stateBufferFrame[playerid]];

    return 0;
}

// GetPlayerStateFrame stores into `frame` the latest frame minus `index`. `index` must be positive
// and within the bounds 0..MAX_STATE_BUFFER_SIZE.
stock GetPlayerStateFrame(playerid, index, frame[E_BUFFERED_STATE]) {
    if(!IsPlayerConnected(playerid)) {
        return 1;
    }

    frame = stateBufferFrames[playerid][stateBufferFrame[playerid] - index];

    return 0;
}


// -
// Internal
// -


/*
# Note regarding y_timers:

It's common, and good practice, to use a y_timers `ptask` for per-player timers. However, in this
situation that would not be ideal because y_timers internally balances timers by adjusting their
start/fire times to spread the load on the server. This library is designed to capture individual
points in time for all players, so a single repeating timer with a player loop is used instead.
Because this library is merely calling some natives and storing data into an array, it really won't
have any affect on performance, even on servers with hundreds of players.
*/
hook OnScriptInit() {
    SetTimer("_stateBuffer_captureFrame", STATE_BUFFER_GRANULARITY, true);
}

public _stateBuffer_captureFrame() {
    new vehicleid;

    foreach(new playerid : Player) {
        // a temporary local frame, allocated each player for proper zeroing
        new frameData[E_BUFFERED_STATE];

        frameData[timestamp] = gettime();
        frameData[tickstamp] = GetTickCount();
        frameData[gameState] = GetPlayerState(playerid);

        GetPlayerPos(playerid, frameData[posPedX], frameData[posPedY], frameData[posPedZ]);
        GetPlayerFacingAngle(playerid, frameData[rotPedZ]);

        if(IsPlayerInAnyVehicle(playerid)) {
            vehicleid = GetPlayerVehicleID(playerid);
            GetVehicleVelocity(vehicleid, frameData[velocityX], frameData[velocityY], frameData[velocityZ]);
            GetVehiclePos(vehicleid, frameData[posVehX], frameData[posVehY], frameData[posVehZ]);
            GetVehicleRotationQuat(vehicleid, frameData[rotVehW], frameData[rotVehX], frameData[rotVehY], frameData[rotVehZ]);
        } else {
            GetPlayerVelocity(playerid, frameData[velocityX], frameData[velocityY], frameData[velocityZ]);
        }

        stateBufferFrames[playerid][stateBufferFrame[playerid]] = frameData;

        // Increment the frame number, if it hits the limit then wrap around to zero.
        if(stateBufferFrame[playerid] == MAX_STATE_BUFFER_SIZE - 1) {
            stateBufferFrame[playerid] = 0;
        } else {
            stateBufferFrame[playerid]++;
        }
    }
}
