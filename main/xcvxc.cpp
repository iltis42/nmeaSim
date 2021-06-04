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

#include "xcvxc.h"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cmath>
#include <Router.h>
#include "Protocols.h"

using namespace std;

XCVXC::XCVXC()
{
}

/*

eg4. $XCVXC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh

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


Example of XCVXC:
$XCVXC,223030.803,A,5228.1139,N,01334.0933,E,0.00,329.74,251009,,,A*6A
 *
 *
 *
 */
int XCVXC::send( float course )
{
	char sentence[80];
	sprintf( sentence,"!xc,%3.2f", course );

	uint sum = Protocols::getCheckSum( sentence );
	int i = strlen(sentence);
	sprintf( &sentence[i], "*%02X\r\n", sum );
	Router::sendNMEA( sentence );

	return strlen( sentence );
}

