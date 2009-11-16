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


#ifndef CARMENWRAPPER_H
#define CARMENWRAPPER_H

#include <iostream>
#include <deque>
#include <pthread.h>
#include <carmen/carmen.h>
#include <carmen/global.h>
#include <sensor/sensor_base/sensor.h>
#include <log/carmenconfiguration.h>
#include <log/sensorstream.h>
#include <log/sensorlog.h>
#include <sensor/sensor_range/rangesensor.h>
#include <sensor/sensor_range/rangereading.h>

class CarmenWrapper {
public:
  static void initializeIPC(const char* name);
  static bool start(const char* name);
  static bool isRunning();
  static void lock();
  static void unlock();
  
  static int queueLength();
  static OrientedPoint getTruePos();
  static bool getReading(RangeReading& reading);
  static void addReading(RangeReading& reading);
  static const SensorMap& sensorMap();
  static bool sensorMapComputed();
  
// conversion function  
  static carmen_robot_laser_message reading2carmen(const RangeReading& reading);
  static RangeReading carmen2reading(const carmen_robot_laser_message& msg);
  static carmen_point_t point2carmen (const OrientedPoint& p);
  static OrientedPoint carmen2point (const carmen_point_t& p);
  
    
// carmen interaction
  static void robot_frontlaser_handler(carmen_robot_laser_message* frontlaser);
  static void simulator_truepos_handler(carmen_simulator_truepos_message* truepos);
  static void shutdown_module(int sig);
  
  private:
  static std::deque<RangeReading> m_rangeDeque;
  static pthread_mutex_t m_mutex, m_lock;  
  static pthread_t m_readingThread;
  static void * m_reading_function(void*);
  static bool m_threadRunning;
  static SensorMap m_sensorMap;
  static RangeSensor* m_rangeSensor;
  static OrientedPoint m_truepos;
};





#endif
/*
int main (int argc, char** argv) {

	CarmenWrapper::init_carmen(argc, argv);
	while (true) {
		IPC_listenWait(100);
	}    
	return 1;
}
*/
