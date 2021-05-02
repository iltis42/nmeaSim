/***************************************************************************
                          gpgsa.cpp  -  description
                             -------------------
    begin                : 24.10.2009
    copyright            : (C) 2009-2013 by Axel Pauli
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

#include <unistd.h>
#include <iostream>

#include "gpgsa.h"
#include "Router.h"
#include "Protocols.h"

using namespace std;


GPGSA::GPGSA()
{
}

/**
GSA - GPS DOP and active satellites

        1 2 3                    14 15  16  17  18
        | | |                    |  |   |   |   |
 $--GSA,a,a,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x.x,x.x*hh<CR><LF>

 Field Number:
  1) Selection mode
  2) Mode
  3) ID of 1st satellite used for fix
  4) ID of 2nd satellite used for fix
  ...
  14) ID of 12th satellite used for fix
  15) PDOP in meters
  16) HDOP in meters
  17) VDOP in meters
  18) checksum

1    = Mode:
       M=Manual, forced to operate in 2D or 3D
       A=Automatic, 3D/2D
2    = Mode:
       1=Fix not available
       2=2D
       3=3D

15   = PDOP
16   = HDOP
17   = VDOP

*/

// $GPGSA,A,3,14,32,17,20,11,23,28,,,,,,1.7,1.1,1.2*3C

int GPGSA::send()
{
  char sentence[80];
  sprintf( sentence, "%s", "$GPGSA,A,3,14,32,17,20,11,23,28,33,,,,,1.3,1.1,1.2" );

  uint sum = Protocols::getCheckSum( sentence );
  int i = strlen(sentence);
  sprintf( &sentence[i], "*%02X\r\n", sum );
  Router::sendNMEA( sentence );
  return strlen( sentence );

}


