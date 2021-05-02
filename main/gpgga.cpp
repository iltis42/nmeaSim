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

char * GPGGA::dmshh_format_lat(double degrees)
{
  static char buf[16];
  int deg_part;
  double min_part;

  if( degrees < 0 )
    {
      degrees = -degrees;
    }

  deg_part = (int) degrees;
  min_part = 60.0 * (degrees - deg_part);

  sprintf( buf, "%02d%07.4f", deg_part, min_part );

  return buf;
}

char * GPGGA::dmshh_format_lon(double degrees)
{
  static char buf[16];
  int deg_part;
  double min_part;

  if( degrees < 0 )
    {
      degrees = -degrees;
    }

  deg_part = (int) degrees;
  min_part = 60.0 * (degrees - deg_part);

  sprintf( buf, "%03d%07.4f", deg_part, min_part );

  return buf;
}


// Example of GPGGA:
// $GPGGA,223031.803,5228.1139,N,01334.0933,E,1,10,00.8,35.3,M,39.8,M,,*53
int GPGGA::send( double lat, double lon, float altitude, int fd )
{
  char sentence[80];
  uint64_t rts = esp_timer_get_time();
  float time = rts/1000000.0;   // in seconds
   //  std::string utcTime = dateTimeUtc.time().toString("hhmmss.zzz");

  sprintf( sentence, "$GPGGA,%6.3f,%s,%c,%s,%c,1,08,2.1,%.1f,M,,,,0000",
		  time,  dmshh_format_lat( lat ), lat > 0 ? 'N' : 'S', dmshh_format_lon( lon ), lon > 0 ? 'E' : 'W', altitude );

  uint sum = Protocols::getCheckSum( sentence );
  int i = strlen(sentence);
  sprintf( &sentence[i], "*%02X\r\n", sum );
  Router::sendNMEA( sentence );
  return strlen( sentence );
}




