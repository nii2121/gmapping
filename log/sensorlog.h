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


#ifndef SENSORLOG_H
#define SENSORLOG_H

#include <list>
#include <istream>
#include <sensor/sensor_base/sensorreading.h>
#include <sensor/sensor_odometry/odometrysensor.h>
#include <sensor/sensor_range/rangesensor.h>
#include <sensor/sensor_odometry/odometryreading.h>
#include <sensor/sensor_range/rangereading.h>
#include "configuration.h"

class SensorLog : public std::list<SensorReading*>{
	public:
		SensorLog(const SensorMap&);
		~SensorLog();
		std::istream& load(std::istream& is);
		OrientedPoint boundingBox(double& xmin, double& ymin, double& xmax, double& ymax) const;
	protected:
		const SensorMap& m_sensorMap;
		OdometryReading* parseOdometry(std::istream& is, const OdometrySensor* ) const;
		RangeReading* parseRange(std::istream& is, const RangeSensor* ) const;
};

#endif
