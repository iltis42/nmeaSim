/***************************************************************************
                          gprmc.h  -  description
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

#ifndef GPRMC_H_
#define GPRMC_H_

#include <string>

void dms_format(double degrees, char *buf );


class GPRMC
{
public:

  GPRMC();
  int send( double lat, double lon, float speed, float course, int fd );

private:

  std::string sentence;


};

#endif
