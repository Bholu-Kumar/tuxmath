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
#endif

char wrapped_lines[MAX_LINES][MAX_LINEWIDTH];

int main(int argc, char* argv[])
{
#ifdef WIN32
    /* On Windows (MSIX / Start-Menu launch) the working directory is NOT
       set to the folder that contains TuxMath.exe, so DATA_PREFIX="data"
       would resolve to the wrong place.  Use SDL_GetBasePath() to find the
       directory that actually contains the exe and switch to it before
       anything else touches the filesystem. Do NOT use freopen here because
       MSIX install directories under Program Files\WindowsApps are read-only. */
    {
        const char *base = SDL_GetBasePath();
        if (base)
        {
            _chdir(base);
            SetCurrentDirectoryA(base);
        }
    }
#endif
    setup(argc, argv);
    TitleScreen();  /* Run the game! */
    cleanup();
    return 0;
}

