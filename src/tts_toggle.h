#ifndef TTS_TOGGLE_H
#define TTS_TOGGLE_H

#include <SDL3/SDL.h>

/* Drop-in replacement for SDL_PollEvent that also intercepts F5 */
int Tux_pollEvent(SDL_Event *event);

/* Toggle TTS on/off, persist to config, and give audio feedback */
void ToggleTTS(void);

/* Toggle Braille output (stub) */
void ToggleBraille(void);

#endif
