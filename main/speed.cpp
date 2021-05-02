/***************************************************************************
                          speed.cpp  -  description
                             -------------------
    begin                : Sat Jul 20 2002
    copyright            : 2002      by Andre Somers
                         : 2007-2012 by Axel Pauli

    email                : kflog.cumulus@gmail.com

    This file is part of Cumulus

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

#include <cmath>

#include "speed.h"

// initialize static values
Speed::speedUnit Speed::_horizontalUnit = kilometersPerHour;
Speed::speedUnit Speed::_verticalUnit   = metersPerSecond;
Speed::speedUnit Speed::_windUnit       = kilometersPerHour;

const double Speed::toKph  = 3.6;
const double Speed::toMph  = 2.2369;
const double Speed::toKnot = 1.9438;
const double Speed::toFpm  = 196.8504;

Speed::Speed() :
  _speed(0),
  _isValid(false)
{
}

Speed::Speed(double Mps) :
  _speed(Mps),
  _isValid(true)
{
}

Speed::~Speed()
{
}

/** + operator for speed. */
Speed
Speed::operator +(const Speed& x) const
{
  return Speed(x._speed + _speed);
}

/** - operator for speed. */
Speed
Speed::operator -(const Speed& x) const
{
  return Speed(_speed - x._speed);
}

/** - operator for speed. */
Speed
operator -(double left, const Speed& right)
{
  return Speed(left - right.getMps());
}

/** + operator for speed. */
Speed
operator +(double left, const Speed& right)
{
  return Speed(left + right.getMps());
}

/** * operator for speed. */
Speed
operator *(double left, const Speed& right)
{
  return Speed(left * right.getMps());
}

/** / operator for speed. */
double
Speed::operator /(const Speed& x) const
{
  return _speed / x._speed;
}

/** * operator for speed. */
double
Speed::operator *(const Speed& x) const
{
  return _speed * x._speed;
}

/** == operator for Speed */
bool
Speed::operator ==(const Speed& x) const
{
  return (x._speed == _speed);
}

/** != operator for Speed */
bool
Speed::operator !=(const Speed& x) const
{
  return (x._speed != _speed);
}

/** - prefix operator for speed */
Speed
Speed::operator -() const
{
  return Speed(-_speed);
}



void Speed::setValueInUnit(double speed, speedUnit unit)
{
  switch (unit)
    {
      case knots:
        setKnot(speed);
        break;
      case milesPerHour:
        setMph(speed);
        break;
      case metersPerSecond:
        setMps(speed);
        break;
      case kilometersPerHour:
        setKph(speed);
        break;
      case feetPerMinute:
        setFpm(speed);
        break;
      default:
        setMps(speed);
        break;
    }
}

std::string Speed::getUnitText(speedUnit unit)
{
  switch (unit)
    {
      case knots:
        return "Kt"; // can also be abbreviated as Kn
        break;
      case milesPerHour:
        return "Mph";
        break;
      case metersPerSecond:
        return "m/s";
        break;
      case kilometersPerHour:
        return "Km/h";
        break;
      case feetPerMinute:
        return "Fpm";
        break;
      default:
        return "m/s";
    }
}

double Speed::getValueInUnit(speedUnit unit) const
{
  switch (unit)
    {
      case knots:
        return getKnots();
        break;
      case milesPerHour:
        return getMph();
        break;
      case metersPerSecond:
        return getMps();
        break;
      case kilometersPerHour:
        return getKph();
        break;
      case feetPerMinute:
        return getFpm();
        break;
      default:
        return getMps();
    }
}
