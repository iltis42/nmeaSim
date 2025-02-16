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

#include "pflaa.h"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cmath>
#include <Router.h>
#include "Protocols.h"

using namespace std;

PFLAA::PFLAA()
{
}

/*
Data on other proximate aircraft:

PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,
<RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,
<ClimbRate>,<AcftType>[,<NoTrack>[,<Source>,<RSSI>]]


<AlarmLevel> Decimal integer value. Range: from 0 to 3.
Alarm level as assessed by FLARM:
0 = no alarm (also used for no-alarm traffic information)
1 = alarm, 15-20 seconds to impact
2 = alarm, 10-15 seconds to impact
3 = alarm, 0-10 seconds to impact

<RelativeNorth> Decimal integer value. Range: from -20000000 to 20000000.
Relative position in meters true north from own position. If
<RelativeEast> is empty, <RelativeNorth> represents the
estimated distance to a target with unknown bearing
(transponder Mode-C/S).

<RelativeEast> Decimal integer value. Range: from -20000000 to 20000000.
Relative position in meters true east from own position. The
field is empty for non-directional targets.

<RelativeVertical> Decimal integer value. Range: from -32768 to 32767.
Relative vertical separation in meters above own position.
Negative values indicate that the other aircraft is lower.
Some distance-dependent random noise is applied to altitude
data if stealth mode is activated either on the target or own
aircraft and no alarm is present at this time.

<IDType> Decimal integer value. Range: from 0 to 2.
Defines the interpretation of the following <ID> field.
0 = random ID, configured or if stealth mode is activated
either on the target or own aircraft
1 = official ICAO 24-bit aircraft address
2 = fixed FLARM ID (chosen by FLARM)
The field is empty if no identification is known (e.g.
transponder Mode-C).

<ID> 6-digit hexadecimal value (e.g. “5A77B1”) as configured in
the target’s PFLAC,,ID sentence. The interpretation is
delivered in <ID-Type>.
The field is empty if no identification is known (e.g.
Transponder Mode-C). Random ID will be sent if stealth mode
is activated either on the target or own aircraft and no alarm
is present at this time.

<Track> Decimal integer value. Range: from 0 to 359.
The target’s true ground track in degrees. The value 0
indicates a true north track. This field is empty if stealth
mode is activated either on the target or own aircraft and for
non-directional targets.

<TurnRate> Currently this field is empty.

<GroundSpeed> Decimal integer value. Range: from 0 to 32767.
When the aircraft is considered moving, the target’s ground
speed in m/s, forced to > 0.
When the aircraft is considered on the ground, the field is
forced to 0.
This field is empty if stealth mode is activated either on the
target or own aircraft and for non-directional targets

<ClimbRate>
Decimal fixed-point number with one digit after the radix
point (dot). Range: from -32.7 to 32.7.
The target’s climb rate in m/s. Positive values indicate a
climbing aircraft. This field is empty if stealth mode is
activated either on the target or own aircraft and for non-
directional targets.

<AcftType> Hexadecimal value. Range: from 0 to F.
Aircraft types:
0 = (reserved)
1 = glider/motor glider (turbo, self-launch, jet) / TMG
2 = tow plane/tug plane
3 = helicopter/gyrocopter/rotorcraft
4 = skydiver, parachute (do not use for drop plane!)
5 = drop plane for skydivers
6 = hang glider (hard)
7 = paraglider (soft)
8 = aircraft with reciprocating engine(s)
9 = aircraft with jet/turboprop engine(s)
A = unknown
B = balloon (hot, gas, weather, static)
C = airship, blimp, zeppelin
D = unmanned aerial vehicle (UAV, RPAS, drone)
E = (reserved)
F = static obstacle

<NoTrack> Field is omitted if data port version <8.
The target’s configured no track setting.
Decimal integer value. Range: from 0 to 1.
0 = no track option not set
1 = no track option set

<Source> The field is omitted if data port version <9.
Data source of the $PFLAA sentence:
0 = FLARM
1 = ADS-B
3 = ADS-R (rebroadcasting of UAT ADS-B to 1090 MHz)
4 = TIS-B (broadcast of location of non-ADS-B equipped
aircraft)
6 = Mode-S (non-directional targets)
If the same target is received from multiple sources, the
following precedence applies: FLARM > ADS-B > ADS-R >
TIS-B > Mode-S.

<RSSI> The field is omitted if data port version <9.
Signal level of the received target in dBm (example: “-71.2”).
Empty if unknown.
This field can be used to help assess the quality of the radio
link. It depends on the installation of the sending station, the
installation of the receiving station and the distance.


Example of PFLAA:

$PFLAA,0,-1234,1234,220,2,DD8F12,180,,30,-1.4,1*

There is a glider in the south-east direction, 1.7km away (1.2km south, 1.2km
east), 220m higher flying on south track with a ground speed of 30m/s in a slight
left turn with 4.5°/s turning rate, sinking with 1.4m/s. Its ID is a static FLARM-ID
“DD8F12”. There is no danger.

 *
 *
 *
 *
 PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,
<RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,
<ClimbRate>,<AcftType>[,<NoTrack>[,<Source>,<RSSI>]]

 *
 */
int PFLAA::send( int alarm, int alt,  char * ID, int speed, double climb )
{
	char sentence[160];

	sprintf( sentence,"$PFLAA,%d,-1234,1234,%d,2,%s,180,,%d,%0.2f,1,0,1,-71.2",
			alarm, alt, ID, speed, climb  );

	uint sum = Protocols::getCheckSum( sentence );
	int i = strlen(sentence);
	sprintf( &sentence[i], "*%02X\r\n", sum );
	Router::sendNMEA( sentence );

	return strlen( sentence );
}

