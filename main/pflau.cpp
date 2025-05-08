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

#include "pflau.h"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cmath>
#include <Router.h>
#include "Protocols.h"

using namespace std;

PFLAU::PFLAU()
{
}

/*
PFLAU – Heartbeat, status, and basic alarms
Syntax:
PFLAU,<RX>,<TX>,<GPS>,<Power>,<AlarmLevel>,<RelativeBearing>,
<AlarmType>,<RelativeVertical>,<RelativeDistance>[,<ID>]
Description:
Heartbeat message; output once per second. Consumers should use this message
to detect the presence (and absence) of a compatible data stream.
The sentence summarizes the most relevant status information from the last one-
second interval: RF status (RX, TX), power state, and the most important current
threat, either traffic, an obstacle, or an alert zone. Consumers with limited
resources (e.g. with respect to display capabilities or computational resources) can
thus use PFLAU to display basic safety information. Other consumers shall also use
PFLAA for extended information.
On devices with SSR/ADS-B Module, non-directional targets are output if enabled
(PCASPFLAU configuration setting).
For data port version >=7, Alert Zone alarms are available; see the <AlarmLevel>
and <AlarmType> fields.
For data port version >= 4, traffic advisory notifications (INFO alarms) are
available, see the <AlarmType> field.

Input / Output:
Only sent by FLARM
Availability:
Depending on configuration: PFLAC,,NMEAOUT
Periodicity:
Sent once every second (1.8 s at maximum)
Parameters:

<RX> Decimal integer value. Range: from 0 to 99.
Number of devices with unique IDs currently received
regardless of the horizontal or vertical separation.
Because the processing might be based on extrapolated
historical data, <Rx> might be lower than the number of
aircraft in range, i.e. there might be other traffic around
(even if the number is zero).
Do not expect to receive <Rx> PFLAA sentences, because
the number of aircraft being processed might be higher
or lower.

<TX> Decimal integer value. Range: from 0 to 1.
Transmission status: 1 for OK and 0 for no transmission.

<GPS> Decimal integer value. Range: from 0 to 2.
GPS status:
0 = no GPS reception
1 = 3d-fix on ground, i.e. not airborne
2 = 3d-fix when airborne
If <GPS> goes to 0, FLARM will not work. Nevertheless,
wait for some seconds to issue any warnings.

<Power> Decimal integer value. Range: from 0 to 1.
Power status: 1 for OK and 0 for under- or over-voltage.

<AlarmLevel> Decimal integer value. Range: from 0 to 3.
Alarm level as assessed by FLARM:
0 = no alarm (also used for no-alarm traffic
information)
1 = aircraft or obstacle alarm, 15-20 seconds to
impact, Alert Zone alarm, or traffic advisory
(<AlarmType> = 4)
2 = aircraft or obstacle alarm, 10-15 seconds to
impact
3 = aircraft or obstacle alarm, 0-10 seconds to impact
Note: Alert Zone: If inside the zone, alarm level is 1 for
4 seconds, then 0 for 12 seconds, then repeats.
<RelativeBearing> Decimal integer value. Range: -180 to 180.
Relative bearing in degrees from true ground track to the
intruder’s position. Positive values are clockwise. 0°
indicates that the object is exactly ahead. The field is
empty for non-directional targets or when no aircraft are
within range. For obstacle alarm and Alert Zone alarm,
this field is 0.

<AlarmType> Hexadecimal value. Range: from 0 to FF.
Type of alarm as assessed by FLARM
0 = no aircraft within range or no-alarm traffic
information
2 = aircraft alarm
3 = obstacle/Alert Zone alarm (if data port version
< 7, otherwise only obstacle alarms are indicated by

<AlarmType> = 3)
4 = traffic advisory (sent once each time an aircraft
enters within distance 1.5 km and vertical distance
300 m from own ship; when data port version >=4)
xx = Alert Zone alarm (see comment below)
When data port version >=7, the type of Alert Zone is
sent as <AlarmType> in the range 10..FF. Refer to the

<ZoneType> parameter in the PFLAO sentence for a
description.

<RelativeVertical> Decimal integer value. Range: from -32768 to 32767.
Relative vertical separation in meters above own
position. Negative values indicate that the other aircraft
or obstacle is lower. The field is empty when no aircraft
are within range.
For Alert Zone and obstacle warnings, this field is 0.

<RelativeDistance> Decimal integer value. Range: from 0 to 2147483647.
Relative horizontal distance in meters to the target or
obstacle. For non-directional targets, this value is
estimated based on signal strength.
The field is empty when no aircraft are within range and
no alarms are generated.
For Alert Zone, this field is 0.

<ID> The field is omitted for protocol version < 4.
6-digit hexadecimal value (e.g. “5A77B1”) as configured
in the target’s PFLAC,,ID.
The interpretation is only delivered in <ID-Type> in the
PFLAA sentence (if received for the same aircraft).
The <ID> field is the ICAO 24-bit address for Mode-S
targets and a FLARM-generated ID for Mode-C targets.
The ID for Mode-C targets may change at any time.
The field is empty when no aircraft are within range and
no alarms are generated.
For obstacles, in PowerFLARM-based devices with data
port version >= 8, this field is set to the obstacle ID
unique for the obstacle and the specific database version
installed. In other cases, it is set to FFFFFF.
In case of Alert Zone warning, the FLARM ID of the Alert
Zone station is output.

Example:
$PFLAU,3,1,2,1,2,-30,2,-32,755*
FLARM is working properly and currently receives 3 other aircraft. The most
dangerous of these aircraft is at 11 o’clock, position 32m below and 755m away.
It is a level 2 alarm.

Example:
$PFLAU,2,1,1,1,0,,0,,,*
FLARM is working properly and receives two other aircraft. They are both out of
range.
Example:
$PFLAU,2,1,2,1,1,-45,2,50,75,1A304C*
FLARM is working properly and receives two other aircraft. The most dangerous
of these aircraft has the ID “1A304C” and it is at 9 o’clock, position 50m below and
75m away. Level 1 alarm.


 */

// PFLAU,<RX>,<TX>,<GPS>,<Power>,<AlarmLevel>,<RelativeBearing>,<AlarmType>,<RelativeVertical>,<RelativeDistance>[,<ID>]

int PFLAU::send( int GPS )
{
	char sentence[160];

	sprintf( sentence,"$PFLAU,0,1,%d,1,0,0,0,0,0", GPS );

	uint sum = Protocols::getCheckSum( sentence );
	int i = strlen(sentence);
	sprintf( &sentence[i], "*%02X\r\n", sum );
	Router::sendNMEA( sentence );

	return strlen( sentence );
}

