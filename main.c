#include <stdio.h>
#include <gtk/gtk.h>
#include <libnotify/notify.h>

#include "pulseaudio.h"
#include "trayicon.h"
#include "volnotify.h"

// How often to re-read the volume, so the icon follows changes made
// elsewhere (media keys, pavucontrol...). Interim measure until the
// libpulse event subscription is written.
#define POLL_SECONDS 3

// Name the running copy owns on the D-Bus session bus
#define APP_ID "io.github.gapf2010.PulseVolumeIcon"

// Returns TRUE if another copy is already running. Uses GApplication's
// single-instance support: the first copy claims APP_ID on the session
// bus, and later copies find it taken. Without a session bus the check
// can't be done, so the program just starts.
static gboolean already_running(GApplication *app){
	GError *error = NULL;

	if(!g_application_register(app, NULL, &error)){
		fprintf(stderr, "pulse-volumeicon: can't check for a running copy: %s\n",
			error->message);
		g_clear_error(&error);
		return FALSE;
	}

	return g_application_get_is_remote(app);
}

static gboolean poll_volume(gpointer data){
	int oldLevel = volumeLevel, oldMuted = volumeMuted;

	pulseaudio_refresh();
	if(oldLevel != volumeLevel || oldMuted != volumeMuted)
		trayicon_update(volumeLevel, volumeMuted);

	return G_SOURCE_CONTINUE;
}

int main(int argc, char *argv[]){
	GApplication *app;

	gtk_init(&argc, &argv);

	// Flags 0 instead of G_APPLICATION_DEFAULT_FLAGS, which needs
	// GLib 2.74 (newer than Slackware 15.0's)
	app = g_application_new(APP_ID, (GApplicationFlags)0);
	if(already_running(app)){
		fprintf(stderr, "pulse-volumeicon is already running\n");
		g_object_unref(app);
		return 0;
	}

	volnotify_init();
	trayicon_init();
	pulseaudio_init();

	g_timeout_add_seconds(POLL_SECONDS, poll_volume, NULL);

	gtk_main();

	notify_uninit();
	g_object_unref(app);
	return 0;
}
