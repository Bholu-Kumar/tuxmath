#include "tts_toggle.h"
#include "options.h"
#include "fileops.h"
#include "tuxmath.h"
#include "globals.h"
#include "setup.h"       /* for local_game */
#include "t4k_common.h"  /* for T4K_Tts_say */

/* These functions are defined in comets.c but have no shared header */
extern void stop_tts_announcer_thread(void);
extern void start_tts_announcer_thread(void);

void ToggleTTS(void)
{
    if (Opts_GetGlobalOpt(USE_TTS) == 1)
    {
        /* Stop the in-game TTS announcer thread if it is running */
        extern SDL_Thread* tts_announcer_thread;
        if (tts_announcer_thread)
            stop_tts_announcer_thread();

        /* Announce AFTER turning off so the user can hear the confirmation */
        T4K_Tts_say(DEFAULT_VALUE, DEFAULT_VALUE, INTERRUPT,
                     "Text to speech is disabled");
        Opts_SetGlobalOpt(USE_TTS, 0);   /* also sets text_to_speech_status = 0 */
    }
    else
    {
        Opts_SetGlobalOpt(USE_TTS, 1);   /* also sets text_to_speech_status = 1 */

        /* We rely on text_to_speech_status flag logic in t4k_common to
           automatically re-enable speech. We don't manually touch spd_connection. */

        T4K_Tts_say(DEFAULT_VALUE, DEFAULT_VALUE, INTERRUPT,
                     "Text to speech is enabled");
    }

    /* Persist the new preference to the user config file */
    write_user_config_file(local_game);
}

int Tux_pollEvent(SDL_Event *event)
{
    int ret = SDL_PollEvent(event);

    if (ret && event->type == SDL_EVENT_KEY_DOWN)
    {
        if (event->key.key == SDLK_F5)
            ToggleTTS();
    }
    return ret;
}
