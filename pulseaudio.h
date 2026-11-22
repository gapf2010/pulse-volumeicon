#ifndef PULSEAUDIO_H
#define PULSEAUDIO_H

void pulseaudio_init(void);
void pulseaudio_refresh(void);

void pulseaudio_mute(void);
void pulseaudio_volume(int amount);

// Defined in pulseaudio.c
extern int volumeLevel;
extern int volumeMuted;

#endif
