/***************************************************************************
                          glider.h  -  description
                             -------------------
    begin                : 23.12.2003
    copyright            : (C) 2003 by Eckhard Völlm
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

#ifndef GLIDER_H_
#define GLIDER_H_

#include "vector.h"
#include "gpgga.h"
#include "gprmc.h"
#include "pgrmz.h"
#include "gpgsa.h"
#include "xcvxc.h"
#include "pflaa.h"
#include "pflau.h"

class glider
{

public:

  glider( double &lat, double &lon, float &speed, float &heading, float wind, float winddir, float &altitude, float climb, float jitter, float deviation );  // jitter in %
  void setFd( int fd );
  void Straight( bool sim_heading=false );
  void setCircle( float radius, std::string direction );
  void Circle( bool left=false );
  void FixedPos();
  void FixedPosGround();
  float random();

private:

  int myFd;
  GPGGA myGPGGA;
  GPRMC myGPRMC;
  PGRMZ myPGRMZ;
  GPGSA myGPGSA;
  XCVXC myXCVXC;
  PFLAA myPFLAA;
  PFLAU myPFLAU;
  double &lat;
  double &lon;
  float  &speed;
  double speedKnots;
  float  &heading;
  float  wind;
  double windKnots;
  float  winddir;
  float  &altitude;
  float  climb;
  double circle;
  double ctime;
  double courseChg;
  int tick;
  float myjitter;
  float mydeviation;
};

#endif
