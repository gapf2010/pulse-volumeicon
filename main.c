#include <gtk/gtk.h>
#include <libnotify/notify.h>

#include "pulseaudio.h"
#include "trayicon.h"
#include "volnotify.h"

// How often to re-read the volume, so the icon follows changes made
// elsewhere (media keys, pavucontrol...). Interim measure until the
// libpulse event subscription is written.
#define POLL_SECONDS 3

static gboolean poll_volume(gpointer data){
	int oldLevel = volumeLevel, oldMuted = volumeMuted;

	pulseaudio_refresh();
	if(oldLevel != volumeLevel || oldMuted != volumeMuted)
		trayicon_update(volumeLevel, volumeMuted);

	return G_SOURCE_CONTINUE;
}

int main(int argc, char *argv[]){
	gtk_init(&argc, &argv);

	volnotify_init();
	trayicon_init();
	pulseaudio_init();

	g_timeout_add_seconds(POLL_SECONDS, poll_volume, NULL);

	gtk_main();

	notify_uninit();
	return 0;
}
