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

void dms_format(double lat, char *buf)
{

  int degree = (int)lat;
  int minutes = (int) ( (lat - (double)degree) * 60.0);
  int seconds = (int) ( (lat - (double)degree - (double)minutes / 60.0) * 60.0 * 60.0 );

  sprintf( buf, "%02d%02d.%02d", degree, minutes, seconds );
}


GPRMC::GPRMC()
{
}

/*

eg4. $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh

1    = UTC of position fix
2    = Data status (V=navigation receiver warning)
3    = Latitude of fix
4    = N or S
5    = Longitude of fix
6    = E or W
7    = Speed over ground in knots
8    = Track made good in degrees True
9    = UT date
10   = Magnetic variation degrees (Easterly var. subtracts from true course)
11   = E or W
12   = Checksum


Example of GPRMC:
$GPRMC,223030.803,A,5228.1139,N,01334.0933,E,0.00,329.74,251009,,,A*6A
 *
 *
 *
 */
int GPRMC::send( double lat, double lon, float speed, float course, int fd )
{
	char sentence[160];
	uint64_t rts = esp_timer_get_time();
	int time = (int)(rts/1000000.0);   // in seconds
	int time_h = (time/3600)%24;
	int time_m = (time/60)%60;
	int time_s = time%60;

	char dms_lat[10];
	char dms_lon[10];
	dms_format( lat, dms_lat );
	dms_format( lon, dms_lon );

	sprintf( sentence,"$GPRMC,%02d%02d%02d,A,%s,%c,%s,%c,%0.3f,%0.2f,030521,0,W",
			time_h, time_m, time_s, dms_lat, lat > 0 ? 'N' : 'S', dms_lon, lon > 0 ? 'E' : 'W', speed, course );

	uint sum = Protocols::getCheckSum( sentence );
	int i = strlen(sentence);
	sprintf( &sentence[i], "*%02X\r\n", sum );
	Router::sendNMEA( sentence );

	return strlen( sentence );
}

