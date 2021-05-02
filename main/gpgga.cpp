/***************************************************************************
                          gpgga.cpp  -  description
                             -------------------
    begin                : 23.12.2003
    copyright            : (C) 2003 by Eckhard Völlm, 2009-2013 by Axel Pauli
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

#include "gpgga.h"
#include "gprmc.h"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
// #include <atomic>
#include "Protocols.h"
#include "Router.h"

using namespace std;

GPGGA::GPGGA()
{
}




// Example of GPGGA:
// $GPGGA,223031.803,5228.1139,N,01334.0933,E,1,10,00.8,35.3,M,39.8,M,,*53
/*
llll.ll = latitude of position
a = N or S
yyyyy.yy = Longitude of position
a = E or W

*/

int GPGGA::send( double lat, double lon, float altitude, int fd )
{
  char sentence[160];
  uint64_t rts = esp_timer_get_time();
  int time = (int)(rts/1000000.0);   // in seconds
  int time_h = (time/3600)%24;
  int time_m = (time/60)%60;
  int time_s = time%60;
   //  std::string utcTime = dateTimeUtc.time().toString("hhmmss.zzz");
  char dms_lat[10];
  char dms_lon[10];
  dms_format( lat, dms_lat );
  dms_format( lon, dms_lon );

  sprintf( sentence, "$GPGGA,%02d%02d%02d,%s,%c,%s,%c,1,08,1.1,%.1f,M,,,,0000",
		  time_h, time_m, time_s,  dms_lat, lat > 0 ? 'N' : 'S', dms_lon, lon > 0 ? 'E' : 'W', altitude );

  uint sum = Protocols::getCheckSum( sentence );
  int i = strlen(sentence);
  sprintf( &sentence[i], "*%02X\r\n", sum );
  Router::sendNMEA( sentence );
  return strlen( sentence );
}




