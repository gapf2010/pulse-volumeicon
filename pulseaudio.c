#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pulseaudio.h"
#include "trayicon.h"
#include "volnotify.h"

// Still a pactl-based hack until the libpulse code is written, but it now
// talks to the default sink (sink index 0 often doesn't exist, especially
// on PipeWire) and reads the real state instead of assuming it.
//
// LC_ALL=C keeps pactl's output in English ("Mute: yes") so it can be
// parsed regardless of the user's locale.
#define PACTL     "LC_ALL=C pactl "
#define SINK      "@DEFAULT_SINK@"
#define VOLUME_MAX 100

int volumeLevel = 0;
int volumeMuted = 0;

// Run a command and capture its output. Returns 0 on success.
static int run_read(const char *cmd, char *buf, size_t len){
	FILE *p = popen(cmd, "r");
	size_t n;

	if(p == NULL)
		return -1;

	n = fread(buf, 1, len - 1, p);
	buf[n] = '\0';

	return pclose(p) == 0 ? 0 : -1;
}

// Read the current volume and mute state of the default sink.
void pulseaudio_refresh(void){
	char buf[512];
	char *slash;
	int level;

	// Output looks like:
	// Volume: front-left: 26214 /  40% / -23.88 dB,   front-right: ...
	if(run_read(PACTL "get-sink-volume " SINK " 2>/dev/null", buf, sizeof(buf)) == 0
			&& (slash = strchr(buf, '/')) != NULL
			&& sscanf(slash + 1, " %d%%", &level) == 1)
		volumeLevel = level;

	// Output looks like: Mute: yes
	if(run_read(PACTL "get-sink-mute " SINK " 2>/dev/null", buf, sizeof(buf)) == 0)
		volumeMuted = (strstr(buf, "yes") != NULL);
}

void pulseaudio_init(void){
	pulseaudio_refresh();
	// Only update the icon here: no notification at startup and,
	// importantly, don't touch the user's volume.
	trayicon_update(volumeLevel, volumeMuted);
}

void pulseaudio_mute(void){
	if(system(PACTL "set-sink-mute " SINK " toggle") != 0){
		fprintf(stderr, "pulse-volumeicon: failed to toggle mute\n");
		return;
	}

	pulseaudio_refresh();
	volnotify_show(volumeLevel, volumeMuted);
	trayicon_update(volumeLevel, volumeMuted);
}

void pulseaudio_volume(int amount){
	char cmd[96];
	int level;

	// Start from the real level, in case something else changed it.
	pulseaudio_refresh();

	level = volumeLevel + amount;
	if(level < 0)
		level = 0;
	// Don't push past 100% by scrolling, but don't yank the volume
	// down either if something else already set it above 100%.
	if(amount > 0 && level > VOLUME_MAX)
		level = volumeLevel > VOLUME_MAX ? volumeLevel : VOLUME_MAX;

	snprintf(cmd, sizeof(cmd), PACTL "set-sink-volume " SINK " %d%%", level);
	if(system(cmd) != 0){
		fprintf(stderr, "pulse-volumeicon: failed to set volume\n");
		return;
	}

	volumeLevel = level;
	volnotify_show(volumeLevel, volumeMuted);
	trayicon_update(volumeLevel, volumeMuted);
}
