/***************************************************************************
                          nmeasim.cpp  -  description
                             -------------------
    begin                :
    copyright            : (C) 2021 by Eckhard Völlm

    NMEA simulator for EPS32

    The NMEA simulator can generate NMEA sentences from the passed options.
    Furthermore it is able to play the content of recorded NMEA or IGC files.

***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <string>

void nmeasim(std::string mode, int time, float wind, float winddir, float heading, float jitter=0, float deviation=0 );

