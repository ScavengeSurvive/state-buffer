# state-buffer

[![sampctl](https://shields.southcla.ws/badge/sampctl-state--buffer-2f2f2f.svg?style=for-the-badge)](https://github.com/Southclaws/state-buffer)

Captures a running buffer of player states for use in anti-cheat or statistical
libraries.

No this is not an NPC-related library, it simply records a players actions into
Pawn memory and provides an interface to access that data.

## Installation

Simply install to your project:

```bash
sampctl package install Southclaws/state-buffer
```

Include in your code and begin using the library:

```pawn
#include <state-buffer>
```

## Usage

By including this library, `OnScriptInit` is hooked to start the frame capture
timer. This timer runs every `STATE_BUFFER_GRANULARITY` milliseconds with a
default value of 100. This means every 100ms, every connected player's state
(see the `E_BUFFERED_STATE` enumerator for what is stored in a frame) is stored.

The amount of history stored is controlled with `MAX_STATE_BUFFER_SIZE` with a
default value of 256. Meaning `256 * 100 / 1000` = 25.6 seconds of player state
history is stored with default settings.

You can use the function `GetPlayerStateFrame` to get the player's state from a
particular point in history, as long as it's within the buffer history.

This can be used to "re-play" a player's actions over the previous 25 seconds,
very useful for administrative work.

### Example - Positional Data

To get historical position data from a player:

```pawn
new frames[MAX_STATE_BUFFER_SIZE][E_BUFFERED_STATE];
GetPlayerStateFrames(playerid, frames);

new next;
new Float:distance;
for(new i; i < MAX_STATE_BUFFER_SIZE - 1; ++i) {
    next = i + 1; // the next frame

    // add up distance of the past 25 seconds
    distance += GetDistance3D(
        frames[next][posPedX], frames[next][posPedY], frames[next][posPedZ],
        frames[i][posPedX], frames[i][posPedY], frames[i][posPedZ]
    );
}

printf("Over the last 25.6 seconds, player moved %fm", distance);
```

## Development

The library is currently in an early stage and more states can be added easily
to record a more accurate representation of a player's history. An
administrative tool may also be released that's built on top of this library
providing rewind/replay features for administrators to investigate potential
rule breakers.

## Testing

To test, simply run the package:

```bash
sampctl package run
```

And connect to `localhost:7777` to see a demo.
