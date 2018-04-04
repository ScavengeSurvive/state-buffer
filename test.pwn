#include "state-buffer.inc"

main() {
	new playerid;
	new frame[E_BUFFERED_STATE];
	GetPlayerLatestStateFrame(playerid, frame);
	GetPlayerStateFrame(playerid, 0, frame);
	new frames[MAX_STATE_BUFFER_SIZE][E_BUFFERED_STATE];
	GetPlayerStateFrames(playerid, frames);
}
