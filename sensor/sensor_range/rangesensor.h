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


#ifndef RANGESENSOR_H
#define RANGESENSOR_H

#include <vector>
#include <sensor/sensor_base/sensor.h>
#include <utils/point.h>

class RangeSensor: public Sensor{
	friend class Configuration;
	friend class CarmenConfiguration;
	friend class CarmenWrapper;
	public:
		struct Beam{
			OrientedPoint pose;	//pose relative to the center of the sensor
			double span;	//spam=0 indicates a line-like beam
			double maxRange;	//maximum range of the sensor
		};	
		RangeSensor(std::string name);
		RangeSensor(std::string name, unsigned int beams, double res, const OrientedPoint& position=OrientedPoint(0,0,0), double span=0, double maxrange=89.0);
		inline const std::vector<Beam>& beams() const {return m_beams;}
		inline OrientedPoint getPose() const {return m_pose;}
	protected:
		OrientedPoint m_pose;
		std::vector<Beam> m_beams;
};


#endif
