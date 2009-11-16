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


#ifndef SENSORSTREAM_H
#define SENSORSTREAM_H

#include <istream>
#include "sensorlog.h"
class SensorStream{
	public:
		SensorStream(const SensorMap& sensorMap);
		virtual ~SensorStream();
		virtual operator bool() const=0;
		virtual bool rewind() = 0 ;
		virtual SensorStream& operator >>(const SensorReading*&) = 0;
		inline const SensorMap& getSensorMap() const {return m_sensorMap; }
	protected:
		const SensorMap& m_sensorMap;
		static SensorReading* parseReading(std::istream& is, const SensorMap& smap);
		static OdometryReading* parseOdometry(std::istream& is, const OdometrySensor* );
		static RangeReading* parseRange(std::istream& is, const RangeSensor* );
};

class InputSensorStream: public SensorStream{
	public:
		InputSensorStream(const SensorMap& sensorMap, std::istream& is);
		virtual operator bool() const;
		virtual bool rewind();
		virtual SensorStream& operator >>(const SensorReading*&);
		//virtual SensorStream& operator >>(SensorLog*& log);
	protected:
		std::istream& m_inputStream;
};

class LogSensorStream: public SensorStream{
	public:
		LogSensorStream(const SensorMap& sensorMap, const SensorLog* log);
		virtual operator bool() const;
		virtual bool rewind();
		virtual SensorStream& operator >>(const SensorReading*&);
	protected:
		const SensorLog* m_log;
		SensorLog::const_iterator m_cursor;
};


#endif
