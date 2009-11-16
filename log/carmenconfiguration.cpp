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


#include "carmenconfiguration.h"
#include <iostream>
#include <sstream>
#include <assert.h>
#include <sensor_odometry/odometrysensor.h>
#include <sensor_range/rangesensor.h>


#define LINEBUFFER_SIZE 2048

using namespace std;

istream& CarmenConfiguration::load(istream& is){
	clear();
	char buf[LINEBUFFER_SIZE];
	bool laseron=false;
	string beams;
	while (is){
		is.getline(buf, LINEBUFFER_SIZE);
		istringstream lis(buf);
		
		string qualifier;
		string name;
		
		if (lis) 
			lis >> qualifier; 
		else 
			continue;
		//this is a workaround for carmen log files
		//the number lf laser beams should be specofoed in the config
		//part of the log
		if (qualifier=="FLASER"){
			laseron=true;
			lis >> beams;
		}
		if (qualifier!="PARAM") 
			continue;
		if (lis) 
			lis >> name; 
		else continue;
		

		vector<string> v;
		while (lis){
			string cparm;
			lis >> cparm;
			if (lis)
				v.push_back(cparm);
		}
		insert(make_pair(name, v));
	}
	if (laseron){
		vector<string> v;
		v.push_back(beams);
		insert(make_pair("laser_beams", v));
		cout << "LASER BEAMS FROM LOG: " << beams << endl;
		v.clear();
		v.push_back("on");
		insert(make_pair("robot_use_laser", v));
	}
	return is;
}

SensorMap CarmenConfiguration::computeSensorMap() const{
	//this boring stuff is for retrieving the parameters from the loaded tokens
	
	SensorMap smap;
	//odometry
	OdometrySensor* odometry=new OdometrySensor("ODOM");
	OdometrySensor* truepos=new OdometrySensor("TRUEPOS", true);
	
	smap.insert(make_pair(odometry->getName(), odometry));
	smap.insert(make_pair(truepos->getName(), truepos));
	//sonars
	const_iterator key=find("robot_use_sonar");
	if (key!=end() && key->second.front()=="on"){
		RangeSensor* sonar=new RangeSensor("SONAR");

		//the center of the sonar is the center of the base
		sonar->m_pose.x=sonar->m_pose.y=sonar->m_pose.theta=0;

		double maxrange=10.;
		key=find("robot_max_sonar");
		if (key!=end()){
			maxrange=atof(key->second.front().c_str());
			cout << "max sonar:" << maxrange << endl;
		}

		unsigned int sonar_num=0;
		key=find("robot_num_sonars");
		if (key!=end()){
			sonar_num=atoi(key->second.front().c_str());
			cout << "robot_num_sonars" << sonar_num << endl;
		}

		key=find("robot_sonar_offsets");
		if (key!=end()){
			const vector<string> & soff=key->second;

			if( (soff.size()/3<sonar_num)){
				cout << __PRETTY_FUNCTION__ << ": Error " << soff.size()
				<< " parameters for defining the sonar offsets"
				<< " while the specified number of sonars requires "
				<< sonar_num*3 << " parameters at least" << endl;
			} else {
				cout << __PRETTY_FUNCTION__ << ": Ok " << soff.size() << " parameters for defining the sonar offsets of " << sonar_num << " devices" << endl;
			}


			RangeSensor::Beam beam;

			for (unsigned int i=0; i<sonar_num*3; i+=3){
				beam.span=M_PI/180.*7.5;
				beam.pose.x=atof(soff[i].c_str());
				beam.pose.y=atof(soff[i+1].c_str());
				beam.pose.theta=atof(soff[i+2].c_str());
				beam.maxRange=maxrange;
				sonar->m_beams.push_back(beam);
				cout << "beam_x" << beam.pose.x;
				cout << " beam_y" << beam.pose.y;
				cout << " beam_theta" << beam.pose.theta << endl;;
			}
		}
		smap.insert(make_pair(sonar->getName(), sonar));
	}
	
	//laser
	key=find("robot_use_laser");

	if (key!=end() && key->second.front()=="on"){
		RangeSensor* laser=new RangeSensor("FLASER");
		
		//by default the center of the robot is the center of the laser
		laser->m_pose.x=laser->m_pose.y=laser->m_pose.theta=0;
		key=find("laser_front_laser_offset");
		if (key!=end()){
			// laser->m_pose.x==atof(key->second.front().c_str());
			cout << "OFFSET not used" << laser->m_pose.x << endl;
		}
		
		
		
		RangeSensor::Beam beam;
			
		double angle=-.5*M_PI;
		unsigned int beam_no=180;
	
		key=find("laser_beams");
		if (key!=end()){
			beam_no=atoi(key->second.front().c_str());
			cout << "BEAMS="<< beam_no << endl;
		}
		
		double maxrange=89;
		double resolution=1.;
		
		if (beam_no==180 || beam_no==181)
			resolution =1.;
		else if (beam_no==360 || beam_no==361)
			resolution =.5;
		else {
			key=find("laser_front_laser_resolution");
			if (key!=end()){
				resolution=atof(key->second.front().c_str());
				cout << "RES" << resolution << endl;
			}
		}
		
		double step=resolution*M_PI/180.;
		for (unsigned int i=0; i<beam_no; i++, angle+=step){
			beam.span=0;
			beam.pose.x=0;
			beam.pose.y=0;
			beam.pose.theta=angle;
			beam.maxRange=maxrange;
			laser->m_beams.push_back(beam);
		}
		smap.insert(make_pair(laser->getName(), laser));
		cout << "beams" << beam_no << endl;
	}
	
	return smap;
};

