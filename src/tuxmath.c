/*
   tuxmath.c:

   Main function for TuxMath

   Copyright 2001, 2006, 2007, 2008, 2009, 2010, 2011.
Authors: Bill Kendrick, David Bruce, Tim Holy, Karl Ove Hufthammer.
Project email: <tuxmath-devel@lists.sourceforge.net>
Project website: http://tux4kids.alioth.debian.org

tuxmath.c is part of "Tux, of Math Command", a.k.a. "tuxmath".

Tuxmath is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Tuxmath is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



/* (tuxmath.h brings in "gettext.h" and <locale.h> */
#include "tuxmath.h"
#include "setup.h"
#include "titlescreen.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#  include <SDL3/SDL.h>
#  include <direct.h>
#  include <windows.h>
#elif defined(__APPLE__)
#  include <SDL3/SDL.h>
#  include <unistd.h>
#endif

char wrapped_lines[MAX_LINES][MAX_LINEWIDTH];

#ifdef _WIN32
static LONG WINAPI TuxMathCrashHandler(PEXCEPTION_POINTERS pExc)
{
    DWORD code = pExc->ExceptionRecord->ExceptionCode;
    /* Ignore normal C++ exceptions (0xE06D7363) and thread naming / RPC exceptions */
    if (code == 0xE06D7363 || code == 0x406D1388 || code == 0x000006BA || code == 0x40010006)
        return EXCEPTION_CONTINUE_SEARCH;

    fprintf(stderr, "\n========================================\n");
    fprintf(stderr, "FATAL CRASH in TuxMath: Code 0x%08lX at Address %p\n",
            (unsigned long)code,
            pExc->ExceptionRecord->ExceptionAddress);
    fprintf(stderr, "========================================\n");
    fflush(stderr);
    return EXCEPTION_CONTINUE_SEARCH;
}
#endif

int main(int argc, char* argv[])
{
#ifdef _WIN32
    AddVectoredExceptionHandler(1, TuxMathCrashHandler);
#endif
#if defined(WIN32) || defined(__APPLE__)
    /* Set working directory to executable directory so relative paths work when launched from any CWD (e.g. MSIX or macOS bundle) */
    {
        const char *base = SDL_GetBasePath();
        if (base)
        {
#ifdef WIN32
            _chdir(base);
            SetCurrentDirectoryA(base);
#else
            chdir(base);
#endif
        }
    }
#endif
    setup(argc, argv);
    TitleScreen();  /* Run the game! */
    cleanup();
    return 0;
}

