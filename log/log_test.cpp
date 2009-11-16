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


#include <fstream>
#include <iostream>
#include <log/carmenconfiguration.h>
#include <log/sensorlog.h>


using namespace std;

int main(char argc, char ** argv){
	if (argc<2){
		cout << "usage log_test <filename>" << endl;
		exit (-1);
	}
	ifstream is(argv[1]);
	if (! is){
		cout << "no file " << argv[1] << " found" << endl;
		exit (-1);
	}
	CarmenConfiguration conf;
	conf.load(is);
	
	SensorMap m=conf.computeSensorMap();
	
	//for (SensorMap::const_iterator it=m.begin(); it!=m.end(); it++)
	//	cout << it->first << " " << it->second->getName() << endl;
	
	SensorLog log(m);
	is.close();
	
	ifstream ls(argv[1]);
	log.load(ls);
	ls.close();
	cout << "log size" << log.size() << endl;
	for (SensorLog::iterator it=log.begin(); it!=log.end(); it++){
		RangeReading* rr=dynamic_cast<RangeReading*>(*it);
		if (rr){
			cout << rr->getSensor()->getName() << " ";
			cout << rr->size()<< " ";
			for (RangeReading::const_iterator it=rr->begin(); it!=rr->end(); it++){
				cout << *it << " ";
			}
			cout<< rr->getPose().x << " " << rr->getPose().y << " " << rr->getPose().theta << endl;
		}
	}
}
