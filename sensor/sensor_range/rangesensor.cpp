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


#include "rangesensor.h"

RangeSensor::RangeSensor(std::string name): Sensor(name){}

RangeSensor::RangeSensor(std::string name, unsigned int beams_num, double res, const OrientedPoint& position, double span, double maxrange): 
	m_pose(position), m_beams(beams_num){
	double angle=-.5*res*beams_num;
	for (unsigned int i=0; i<beams_num; i++, angle+=res){
		RangeSensor::Beam& beam(m_beams[i]);
		beam.span=span;
		beam.pose.x=0;
		beam.pose.y=0;
		beam.pose.theta=angle;
		beam.maxRange=maxrange;
	}
}
