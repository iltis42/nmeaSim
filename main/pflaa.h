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

#ifndef PFLAA_H_
#define PFLAA_H_

#include <string>

class PFLAA
{
public:

  PFLAA();
  int send( int alarm = 0, int alt = 0.0,  char * ID = 0, int speed = 30, double climb=0.0 );

private:
  std::string sentence;

};

#endif
