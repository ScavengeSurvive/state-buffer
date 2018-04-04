#include "state-buffer.inc"

#include <string>
#include <action-text>
#include <YSI\y_timers>

main() {
	new playerid;
	new frame[E_BUFFERED_STATE];
	GetPlayerLatestStateFrame(playerid, frame);
	GetPlayerStateFrame(playerid, 0, frame);
}

new fullString[1024];
ptask demo[100](playerid) {
	fullString[0] = EOS;
	for(new i = 1; i <= 5; i++) {
		new
			str[120],
			frame[E_BUFFERED_STATE];

		GetPlayerStateFrame(playerid, i, frame);

		format(str, sizeof(str),
			"%d:%d %d~n~p(%.1f,%.1f,%.1f) r(%.1f) v(%.2f,%.2f,%.2f)~n~",
			frame[stbuf_timestamp],
			frame[stbuf_tickstamp],
			frame[stbuf_gameState],
			frame[stbuf_posPedX],
			frame[stbuf_posPedY],
			frame[stbuf_posPedZ],
			frame[stbuf_rotPedZ],
			frame[stbuf_velocityX],
			frame[stbuf_velocityY],
			frame[stbuf_velocityZ]
		);
		strcat(fullString, str);
	}
	ShowActionText(playerid, fullString, 0, 1000);
}
