/***************************************************************************
                          gprmc.cpp  -  description
                             -------------------
    begin                : 23.12.2003
    copyright            : (C) 2003 by Eckhard Völlm, by 2009-2013 Axel Pauli
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

#include "gprmc.h"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cmath>
#include <Router.h>
#include "Protocols.h"

using namespace std;

GPRMC::GPRMC()
{
}

char* GPRMC::dmshh_format_lat(double degrees)
{
  static char buf[16];
  int deg_part;
  double min_part;

  if( degrees < 0 )
    {
      degrees = -degrees;
    }

  deg_part = (int) rint(degrees);
  min_part = 60.0 * (degrees - deg_part);

  sprintf( buf, "%02d%07.4f", deg_part, min_part );

  return buf;
}

char* GPRMC::dmshh_format_lon(double degrees)
{
  static char buf[16];
  int deg_part;
  double min_part;

  if( degrees < 0 )
    {
      degrees = -degrees;
    }

  deg_part = (int) rint(degrees);
  min_part = 60.0 * (degrees - deg_part);

  sprintf( buf, "%03d%07.4f", deg_part, min_part );

  return buf;
}


// Example of GPRMC:
// $GPRMC,223030.803,A,5228.1139,N,01334.0933,E,0.00,329.74,251009,,,A*6A
int GPRMC::send( double lat, double lon, float speed, float course, int fd )
{
	char sentence[80];
	uint64_t rts = esp_timer_get_time();
	float time = rts/1000000.0;   // in seconds

	sprintf( sentence,"$GPRMC%6.3f,A,%s,%c,%s,%c,%0.3f,%0.2f,310521,,,A*",
			time, dmshh_format_lat( lat ), lat > 0 ? 'N' : 'S', dmshh_format_lon( lon ), lon > 0 ? 'E' : 'W', speed, course );

	uint sum = Protocols::getCheckSum( sentence );
	int i = strlen(sentence);
	sprintf( &sentence[i], "*%02X\r\n", sum );
	Router::sendNMEA( sentence );

	return strlen( sentence );
}

