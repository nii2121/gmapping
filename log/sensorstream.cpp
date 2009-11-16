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


#include <assert.h>
#include <sstream>
#include "sensorstream.h"
#define LINEBUFFER_SIZE 1000000 //for not Cyrill to unbless me, it is better to exagerate :-))

using namespace std;

//SensorStream
SensorStream::SensorStream(const SensorMap& sensorMap) :m_sensorMap(sensorMap){}

SensorStream::~SensorStream(){}

SensorReading* SensorStream::parseReading(std::istream& is, const SensorMap& smap){
	SensorReading* reading=0;
	if (is){
		char buf[LINEBUFFER_SIZE];
		is.getline(buf, LINEBUFFER_SIZE);
		istringstream lis(buf);
		
		string sensorname;
		
		if (lis){
			lis >>sensorname; 
		} else 
			return 0;
		
		SensorMap::const_iterator it=smap.find(sensorname);
		if (it==smap.end()){
			return 0;
		}
		
		Sensor* sensor=it->second;
		
		OdometrySensor* odometry=dynamic_cast<OdometrySensor*>(sensor);
		if (odometry)
			reading=parseOdometry(lis, odometry);
		
		RangeSensor* range=dynamic_cast<RangeSensor*>(sensor);
		if (range)
			reading=parseRange(lis, range);
	}
	return reading;
}

OdometryReading* SensorStream::parseOdometry(std::istream& is, const OdometrySensor* osen ){
	OdometryReading* reading=new OdometryReading(osen);
	OrientedPoint pose;
	OrientedPoint speed;
	OrientedPoint accel;
	is >> pose.x >> pose.y >> pose.theta;
	is >> speed.x >>speed.theta;
	speed.y=0;
	is >> accel.x;
	accel.y=accel.theta=0;
	reading->setPose(pose); reading->setSpeed(speed); reading->setAcceleration(accel);
	double a;
	string s;
	is >> a >>s >>a;
	reading->setTime(a);
	return reading;
}

RangeReading* SensorStream::parseRange(std::istream& is, const RangeSensor* rs){
	unsigned int size;
	is >> size;
	assert(size=rs->beams().size());
	
	RangeReading* reading=new RangeReading(rs);
	for (unsigned int i=0; i<size; i++){
		double r;
		is >> r;
		reading->push_back(r);
	}
	OrientedPoint pose;
	is >> pose.x >> pose.y >> pose.theta;
	reading->setPose(pose);
	double a,b,c;
	is >> a >> b >> c;
	string s;
	is >> a >> s;
	is >> a;
	reading->setTime(a);
	return reading;
}

//LogSensorStream
LogSensorStream::LogSensorStream(const SensorMap& sensorMap, const SensorLog* log):
	SensorStream(sensorMap){
	m_log=log;
	assert(m_log);
	m_cursor=log->begin();
}

LogSensorStream::operator bool() const{
	return m_cursor==m_log->end();
}

bool LogSensorStream::rewind(){
	m_cursor=m_log->begin();
	return true;
}

SensorStream& LogSensorStream::operator >>(const SensorReading*& rd){
	rd=*m_cursor;
	m_cursor++;
	return *this;
}

//InputSensorStream
InputSensorStream::InputSensorStream(const SensorMap& sensorMap, std::istream& is):
	SensorStream(sensorMap), m_inputStream(is){
}

InputSensorStream::operator bool() const{
	return (bool) m_inputStream;
}

bool InputSensorStream::rewind(){
	//m_inputStream.rewind();
	return false;
}

SensorStream& InputSensorStream::operator >>(const SensorReading*& reading){
	reading=parseReading(m_inputStream, m_sensorMap);
	return *this;
}

