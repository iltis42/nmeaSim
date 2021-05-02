/***************************************************************************
                          pgrmz.cpp - description
                             -------------------
    begin                : 02.08.2010
    copyright            : (C) 2010-2013 by Axel Pauli
    email                : kflog.cumulus@gmail.com

    $Id$

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>
#include <cstdio>
#include <unistd.h>

#include "pgrmz.h"
#include "Protocols.h"
#include "Router.h"

using namespace std;

PGRMZ::PGRMZ()
{
}

/**
  Used by Garmin and Flarm devices
  $PGRMZ,93,f,3*21
         93,f         Altitude in feet
         3            Position fix dimensions 2 = FLARM barometric altitude
                                              3 = GPS altitude

  Flarm example: $PGRMZ,2963,F,2*04

  Input parameter altitude is expected as meters.
*/
int PGRMZ::send( float altitude, int fd )
{
  char sentence[80];
  sprintf(sentence,"$PGRMZ,%d,F,2*", (int)(altitude*3.28095 + 0.5) );
  uint sum = Protocols::getCheckSum( sentence );
  int i = strlen(sentence);
  sprintf( &sentence[i], "*%02X\r\n", sum );
  Router::sendNMEA( sentence );
  return strlen( sentence );
}
