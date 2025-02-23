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

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "glider.h"
#include <string>
#include <HardwareSerial.h>  // delay()

static    double lat=48.50;
static    double lon=9.4510;
static    float  speed=100.0;

static    std::string direction="right";

static    float  heading=230.0;
static    float  radius=130.0;    // default circle radius
static    float  altitude=1000.0; // default altitude
static    float  climb=0.0;       // default climb rate zero
static    int    delaytime=1000;      // default pause is 1000ms


//
void nmeasim( std::string mode, int time, float wind, float winddir, float aHeading, float jitter, float deviation )
{
  winddir += 180;
  if( winddir > 360 )
	  winddir -= 360;
  // aHeading += 180;
  if( aHeading > 360 )
	  aHeading -= 360;
  glider myGl( lat, lon, speed, aHeading, wind, winddir, altitude, climb, jitter, deviation );
  myGl.setCircle( radius, direction );
  int loops = 0;
  while( 1 )
    {
      if( mode == "str" )
        myGl.Straight();
      if( mode == "str_head" )
         myGl.Straight( true );
      if( mode == "cir" )
        myGl.Circle();
      if( mode == "cirR" )
             myGl.Circle(false);
      if( mode == "cirL" )
             myGl.Circle(true);
      if( mode == "pos" )
        myGl.FixedPos();
      if( mode == "gpos" )
        myGl.FixedPosGround();
      delay( delaytime );
      loops++;
      if( loops > time )
    	  break;
    }
}

