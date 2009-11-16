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


#include "carmenwrapper.h"
using namespace std;

//static vars for the carmenwrapper
SensorMap CarmenWrapper::m_sensorMap;
deque<RangeReading> CarmenWrapper::m_rangeDeque;
pthread_mutex_t CarmenWrapper::m_mutex;  
pthread_mutex_t CarmenWrapper::m_lock;  
pthread_t CarmenWrapper::m_readingThread;
RangeSensor* CarmenWrapper::m_rangeSensor=0;
bool CarmenWrapper::m_threadRunning=false;
OrientedPoint CarmenWrapper::m_truepos;



void CarmenWrapper::initializeIPC(const char* name) {
  carmen_initialize_ipc((char *)name);
}

bool CarmenWrapper::start(const char* name){
  if (m_threadRunning)
    return false;
	carmen_robot_subscribe_frontlaser_message(NULL, (carmen_handler_t)robot_frontlaser_handler, CARMEN_SUBSCRIBE_LATEST);
	carmen_simulator_subscribe_truepos_message(NULL,(carmen_handler_t) simulator_truepos_handler, CARMEN_SUBSCRIBE_LATEST);
	signal(SIGINT, shutdown_module);
	pthread_mutex_init(&m_mutex, 0);
	pthread_mutex_init(&m_lock, 0);
	m_threadRunning=true;
	pthread_create (&m_readingThread,0,m_reading_function,0);
	return true; 
}

void CarmenWrapper::lock(){
	pthread_mutex_lock(&m_lock);
}

void CarmenWrapper::unlock(){
	pthread_mutex_unlock(&m_lock);
}


bool CarmenWrapper::sensorMapComputed(){
	pthread_mutex_lock(&m_mutex);
	bool smok=m_rangeSensor;
	pthread_mutex_unlock(&m_mutex);
	//return 1;
	return smok;
}

const SensorMap& CarmenWrapper::sensorMap(){
	return m_sensorMap;
}
 
bool CarmenWrapper::isRunning(){
	return m_threadRunning;
}

int CarmenWrapper::queueLength(){
	int ql=0;
	pthread_mutex_lock(&m_mutex);
	ql=m_rangeDeque.size();
	pthread_mutex_unlock(&m_mutex);
	return ql;
}

OrientedPoint CarmenWrapper::getTruePos(){
	return m_truepos;
}

bool CarmenWrapper::getReading(RangeReading& reading){
	bool present=false;
	pthread_mutex_lock(&m_mutex);
	if (!m_rangeDeque.empty()){
//		cerr << __PRETTY_FUNCTION__ << ": queue size=" <<m_rangeDeque.size() << endl;
		reading=m_rangeDeque.front();
		m_rangeDeque.pop_front();
		present=true;
	}
	pthread_mutex_unlock(&m_mutex);
	return present;
}

void CarmenWrapper::addReading(RangeReading& reading){
	pthread_mutex_lock(&m_mutex);
	m_rangeDeque.push_back(reading);
	pthread_mutex_unlock(&m_mutex);
}


//RangeSensor::RangeSensor(std::string name, unsigned int beams_num, unsigned int res, const OrientedPoint& position, double span, double maxrange): 

void CarmenWrapper::robot_frontlaser_handler(carmen_robot_laser_message* frontlaser) {
/*	if (! m_rangeSensor){
		double res=0;
		if (frontlaser->num_readings==180 || frontlaser->num_readings==181)
			res=M_PI/180;
		if (frontlaser->num_readings==360 || frontlaser->num_readings==361)
			res=M_PI/360;
		assert(res>0);
		m_rangeSensor=new RangeSensor("FLASER",frontlaser->num_readings, res, OrientedPoint(0,0,0), 0, 89.9);
		m_sensorMap.insert(make_pair(string("FLASER"), m_rangeSensor));
		
		cout << __PRETTY_FUNCTION__ 
		     << ": FrontLaser configured." 
		     << " Readings " << m_rangeSensor->beams().size() 
		     << " Resolution " << res << endl;
	}
	
	RangeReading reading(m_rangeSensor, frontlaser->timestamp);
	reading.resize(m_rangeSensor->beams().size());
	for (unsigned int i=0; i< (unsigned int)frontlaser->num_readings; i++){
		reading[i]=(double)frontlaser->range[i];
	}
	reading.setPose(OrientedPoint(frontlaser->x, frontlaser->y, frontlaser->theta));
*/	
	RangeReading reading=carmen2reading(*frontlaser);
	addReading(reading);
}

void CarmenWrapper::simulator_truepos_handler(carmen_simulator_truepos_message* truepos){
	m_truepos.x=truepos->truepose.x;
	m_truepos.y=truepos->truepose.y;
	m_truepos.theta=truepos->truepose.theta;
}

RangeReading CarmenWrapper::carmen2reading(const carmen_robot_laser_message& msg){
	if (! m_rangeSensor){
		double res=0;
		if (msg.num_readings==180 || msg.num_readings==181)
			res=M_PI/180;
		if (msg.num_readings==360 || msg.num_readings==361)
			res=M_PI/360;
		assert(res>0);
		m_rangeSensor=new RangeSensor("FLASER",msg.num_readings, res, OrientedPoint(0,0,0), 0, 89.9);
		m_sensorMap.insert(make_pair(string("FLASER"), m_rangeSensor));
		
		cout << __PRETTY_FUNCTION__ 
		     << ": FrontLaser configured." 
		     << " Readings " << m_rangeSensor->beams().size() 
		     << " Resolution " << res << endl;
	}
	
	RangeReading reading(m_rangeSensor, msg.timestamp);
	reading.resize(m_rangeSensor->beams().size());
	for (unsigned int i=0; i< (unsigned int)msg.num_readings; i++){
		reading[i]=(double)msg.range[i];
	}
	reading.setPose(OrientedPoint(msg.x, msg.y, msg.theta));
	return reading;
}

void * CarmenWrapper::m_reading_function(void*){
	while (true) {
		lock();
		IPC_listenClear(100);
		unlock();
		usleep(100000);
	}    
	return 0;
}

void CarmenWrapper::shutdown_module(int sig){
  if(sig == SIGINT) {
    close_ipc();
    fprintf(stderr, "\nDisconnecting (shutdown_module(%d) called).\n",sig);
    exit(0);
  }
}
/*
typedef struct {
  int num_readings;
  float *range;
  char *tooclose;
  double x, y, theta;//position of the laser on the robot
  double odom_x, odom_y, odom_theta; //position of the center of the robot
  double tv, rv;
  double forward_safety_dist, side_safety_dist;
  double turn_axis;
  double timestamp;
  char host[10];
} carmen_robot_laser_message;
*/

carmen_robot_laser_message CarmenWrapper::reading2carmen(const RangeReading& reading){
	carmen_robot_laser_message frontlaser;
	frontlaser.num_readings=reading.size();
	frontlaser.range = new float[frontlaser.num_readings];
	frontlaser.tooclose=0;
	frontlaser.x=frontlaser.odom_x=reading.getPose().x;
	frontlaser.y=frontlaser.odom_y=reading.getPose().y;
	frontlaser.theta=frontlaser.odom_theta=reading.getPose().theta;
	frontlaser.tv=frontlaser.rv=0;
	frontlaser.forward_safety_dist=frontlaser.side_safety_dist=0;
	frontlaser.turn_axis=0;
	frontlaser.timestamp=reading.getTime();
	for (unsigned int i=0; i< reading.size(); i++){
		frontlaser.range[i]=(float)reading[i];
	}
	return frontlaser;
}

carmen_point_t CarmenWrapper::point2carmen (const OrientedPoint& p){
	return (carmen_point_t){p.x,p.y,p.theta};
}

OrientedPoint CarmenWrapper::carmen2point (const carmen_point_t& p){
	return OrientedPoint(p.x, p.y, p.theta);
}


/*
int main (int argc, char** argv) {
	CarmenWrapper::start(argc, argv);
	while(1){
		sleep(2);
		RangeReading reading(0,0);
		while(CarmenWrapper::getReading(reading)){
			cout << "FLASER " <<  reading.size();
			for (int i=0; i<reading.size(); i++)
				cout << " " << reading[i];
			cout << reading.getPose().x << " "
			     << reading.getPose().y << " "
			     << reading.getPose().theta << " 0 cazzo 0" << endl;
		}
		cout << endl;
	}
	return 1;
}
*/

