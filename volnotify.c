#include <stdio.h>
#include <libnotify/notify.h>

#include "volnotify.h"

static NotifyNotification *notification;

void volnotify_init(void){
	notify_init("pulse-volumeicon");
	notification = notify_notification_new("Volume", NULL, "audio-volume-muted");

	notify_notification_set_timeout(notification, NOTIFY_EXPIRES_DEFAULT);
	// Ask the notification daemon to replace the previous volume popup
	// instead of stacking a new one per scroll step. Different daemons
	// look at different hints, so set both common ones.
	notify_notification_set_hint_string(notification, "synchronous", "volume");
	notify_notification_set_hint_string(notification,
		"x-canonical-private-synchronous", "volume");
}

void volnotify_show(int volumeLevel, int volumeMuted){
	const char *icon;
	char body[32];

	if(volumeMuted || volumeLevel == 0)
		icon = "audio-volume-muted";
	else if(volumeLevel <= 33)
		icon = "audio-volume-low";
	else if(volumeLevel <= 66)
		icon = "audio-volume-medium";
	else
		icon = "audio-volume-high";

	if(volumeMuted)
		snprintf(body, sizeof(body), "Muted");
	else
		snprintf(body, sizeof(body), "%d%%", volumeLevel);

	notify_notification_update(notification, "Volume", body, icon);
	// Progress bar, for daemons that support it
	notify_notification_set_hint_int32(notification, "value", volumeLevel);
	notify_notification_show(notification, NULL);
}
