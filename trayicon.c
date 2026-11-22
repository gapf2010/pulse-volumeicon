#include <stdio.h>
#include <gtk/gtk.h>
#include <libayatana-appindicator/app-indicator.h>

#include "trayicon.h"
#include "pulseaudio.h"

// Percent per scroll notch
#define VOLUME_STEP 5

static AppIndicator *indicator;
static GtkWidget *menuLevel;
static GtkWidget *menuMute;

//
// Handlers

static void trayicon_on_mute(GtkMenuItem *item, gpointer data){
	pulseaudio_mute();
}

static void trayicon_on_quit(GtkMenuItem *item, gpointer data){
	gtk_main_quit();
}

static void trayicon_on_scroll(AppIndicator *ind, gint delta,
		GdkScrollDirection direction, gpointer data){
	if(direction == GDK_SCROLL_UP)
		pulseaudio_volume(VOLUME_STEP);
	else if(direction == GDK_SCROLL_DOWN)
		pulseaudio_volume(-VOLUME_STEP);
}

//
// Public functions

void trayicon_init(void){
	GtkWidget *menu, *separator, *menuQuit;

	// StatusNotifierItem icon: works with KDE Plasma, Xfce, Waybar,
	// and GNOME with the AppIndicator extension.
	indicator = app_indicator_new("pulse-volumeicon", "audio-volume-muted",
		APP_INDICATOR_CATEGORY_HARDWARE);
	app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
	app_indicator_set_title(indicator, "Volume");

	// AppIndicators always need a menu; left click opens it.
	menu = gtk_menu_new();

	menuLevel = gtk_menu_item_new_with_label("Volume");
	gtk_widget_set_sensitive(menuLevel, FALSE);

	menuMute = gtk_menu_item_new_with_label("Mute");
	g_signal_connect(menuMute, "activate", G_CALLBACK(trayicon_on_mute), NULL);

	separator = gtk_separator_menu_item_new();

	menuQuit = gtk_menu_item_new_with_label("Quit");
	g_signal_connect(menuQuit, "activate", G_CALLBACK(trayicon_on_quit), NULL);

	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuLevel);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuMute);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), separator);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuQuit);
	gtk_widget_show_all(menu);

	app_indicator_set_menu(indicator, GTK_MENU(menu));

	// Middle click toggles mute
	app_indicator_set_secondary_activate_target(indicator, menuMute);

	// Scroll changes volume
	g_signal_connect(indicator, "scroll-event",
		G_CALLBACK(trayicon_on_scroll), NULL);
}

void trayicon_update(int volumeLevel, int volumeMuted){
	const char *icon;
	char text[32];

	if(volumeMuted || volumeLevel == 0)
		icon = "audio-volume-muted";
	else if(volumeLevel <= 33)
		icon = "audio-volume-low";
	else if(volumeLevel <= 66)
		icon = "audio-volume-medium";
	else
		icon = "audio-volume-high";

	if(volumeMuted)
		snprintf(text, sizeof(text), "Volume: %d%% (muted)", volumeLevel);
	else
		snprintf(text, sizeof(text), "Volume: %d%%", volumeLevel);

	app_indicator_set_icon_full(indicator, icon, text);
	gtk_menu_item_set_label(GTK_MENU_ITEM(menuLevel), text);
	gtk_menu_item_set_label(GTK_MENU_ITEM(menuMute),
		volumeMuted ? "Unmute" : "Mute");
}
