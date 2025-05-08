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

#ifndef PFLAU_H_
#define PFLAU_H_

#include <string>

class PFLAU
{
public:

  PFLAU();
  int send( int GPS=3 );

private:
  std::string sentence;

};

#endif
