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


#ifndef CARMENCONFIGURATION_H
#define CARMENCONFIGURATION_H

#include <string>
#include <map>
#include <vector>
#include <istream>
#include <sensor/sensor_base/sensor.h>
#include "configuration.h"

class CarmenConfiguration: public Configuration, public std::map<std::string, std::vector<std::string> >{
	public:
		virtual std::istream& load(std::istream& is);
		virtual SensorMap computeSensorMap() const;
};

#endif

