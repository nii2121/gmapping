/*****************************************************************
 *
 * This file is part of the GMAPPING project
 *
 * GMAPPING Copyright (c) 2004 Giorgio Grisetti, 
 * Cyrill Stachniss, and Wolfram Burgard
 *
 * This software is licensed under the "Creative Commons 
 * License (Attribution-NonCommercial-ShareAlike 2.0)" 
 * and is copyrighted by Giorgio Grisetti, Cyrill Stachniss, 
 * and Wolfram Burgard.
 * 
 * Further information on this license can be found at:
 * http://creativecommons.org/licenses/by-nc-sa/2.0/
 * 
 * GMAPPING is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  
 *
 *****************************************************************/


#ifndef ODOMETRYREADING_H
#define ODOMETRYREADING_H

#include <string.h>
#include <sensor/sensor_base/sensorreading.h>
#include <utils/point.h>
#include "odometrysensor.h"

class OdometryReading: public SensorReading{
	public:
		OdometryReading(const OdometrySensor* odo, double time=0);
		inline const OrientedPoint& getPose() const {return m_pose;}
		inline const OrientedPoint& getSpeed() const {return m_speed;}
		inline const OrientedPoint& getAcceleration() const {return m_acceleration;}
		inline void setPose(const OrientedPoint& pose) {m_pose=pose;}
		inline void setSpeed(const OrientedPoint& speed) {m_speed=speed;}
		inline void setAcceleration(const OrientedPoint& acceleration) {m_acceleration=acceleration;}
		
	protected:
		OrientedPoint m_pose;
		OrientedPoint m_speed;
		OrientedPoint m_acceleration;
};

#endif

