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
#include <unistd.h>
#include "scanmatcherprocessor.h"

using namespace std;

#define DEBUG cout << __PRETTY_FUNCTION__
#define MAX_STRING_LENGTH 1024

int main(unsigned int argc, const char * const * argv){
	const char * filename=0;
	double xmin=-100.;
	double ymin=-100.;
	double xmax=100.;
	double ymax=100.;
	double delta=1.;
	double patchDelta=0.1;
	double sigma=0.02;
	double maxrange=81.9;
	double maxUrange=81.9;
	double regscore=1e4;
	double lstep=.05;
	double astep=.05;
	int kernelSize=0;
	int iterations=4;
	double critscore=0.;
	double maxMove=1.;
	bool computeCovariance=false;

	if (argc<1){
		cout << "usage main {arglist}" << endl;
		cout << "where the arguments are: " << endl;
		cout << "\t -xmin     <value>" << endl;
		cout << "\t -xmax     <value>" << endl;
		cout << "\t -ymin     <value>" << endl;
		cout << "\t -ymax     <value>" << endl;
		cout << "\t -maxrange <value>   : maxmimum preception range" << endl;
		cout << "\t -delta    <value>   : patch size" << endl;
		cout << "\t -patchDelta <value> : patch cell size" << endl;
		cout << "\t -lstep    <value> : linear serach step" << endl;
		cout << "\t -astep    <value> : ìangular search step" << endl;
		cout << "\t -regscore <value> : registration scan score" << endl;
		cout << "\t -filename <value> : log filename in carmen format" << endl;
		cout << "\t -sigma    <value> : convolution kernel size" << endl;
		cout << "Look the code for discovering another thousand of unuseful parameters" << endl;
		return -1;
	}


	#define parseFlag(name,value)\
	if (!strcmp(argv[c],name)){\
		value=true;\
		cout << name << " on"<< endl;\
	}\

	#define parseString(name,value)\
	if (!strcmp(argv[c],name) && c<argc-1){\
		c++;\
		value=argv[c];\
		cout << name << "=" << value << endl;\
	}\


	#define parseDouble(name,value)\
	if (!strcmp(argv[c],name) && c<argc-1){\
		c++;\
		value=atof(argv[c]);\
		cout << name << "=" << value << endl;\
	}\
	
	#define parseInt(name,value)\
	if (!strcmp(argv[c],name) && c<argc-1){\
		c++;\
		value=atoi(argv[c]);\
		cout << name << "=" << value << endl;\
	}\

	unsigned int c=1;
	while (c<argc){
		parseString("-filename",filename);
		parseDouble("-xmin",xmin);
		parseDouble("-xmax",xmax);
		parseDouble("-ymin",ymin);
		parseDouble("-ymax",ymax);
		parseDouble("-delta",delta);
		parseDouble("-patchDelta",patchDelta);
		parseDouble("-maxrange",maxrange);
		parseDouble("-maxUrange",maxUrange);
		parseDouble("-regscore",regscore);
		parseDouble("-critscore",critscore);
		parseInt("-kernelSize",kernelSize);
		parseDouble("-sigma",sigma);
		parseInt("-iterations",iterations);
		parseDouble("-lstep",lstep);
		parseDouble("-astep",astep);
		parseDouble("-maxMove",maxMove);
		parseFlag("-computeCovariance",computeCovariance);
		c++;
	}

	#undef parseDouble
	#undef parseString
	#undef parseInt
	#undef parseFlag

	if (! filename){
		cout << "no filename specified" << endl;
		return -1;
	}

	ifstream is;
	is.open(filename);
	if (! is){
		cout << "no file found" << endl;
		return -1;
	}

	DEBUG << "scanmatcher processor construction" << endl;
	ScanMatcherProcessor scanmatcher(xmin, ymin, xmax, ymax, delta, patchDelta);
	
	//double range, double sigma, int kernsize, double lopt, double aopt, int iterations
	scanmatcher.setMatchingParameters(maxUrange, maxrange, sigma, kernelSize, lstep, astep, iterations, computeCovariance);
	scanmatcher.setRegistrationParameters(regscore, critscore);
	
	CarmenConfiguration conf;
	conf.load(is);
	is.close();

	SensorMap sensorMap=conf.computeSensorMap();
	scanmatcher.setSensorMap(sensorMap);
	SensorLog log(sensorMap);
	ifstream logstream(filename);
	log.load(logstream);
	logstream.close();
	cout << "Log loaded " << log.size() << " records" << endl; 

	char ofilename[MAX_STRING_LENGTH];
	ofilename[0]=0;
	strcat(ofilename, "scanmatched-");
	strcat(ofilename,filename); 
	ofstream poseStream(ofilename);
	scanmatcher.init();
	for (SensorLog::const_iterator it=log.begin(); it!=log.end(); it++){
		const RangeReading* rr=dynamic_cast<const RangeReading*>(*it);
		if (rr){
			const RangeSensor* rs=dynamic_cast<const RangeSensor*>(rr->getSensor());
			assert (rs && rs->beams().size()==rr->size());
			scanmatcher.processScan(*rr);
			OrientedPoint p=scanmatcher.getPose();
			poseStream << "FLASER "<< rr->size() << " ";
			for (RangeReading::const_iterator b=rr->begin(); b!=rr->end(); b++){
				poseStream << *b << " ";
			}
			poseStream << p.x << " " << p.y << " " << p.theta << " ";
			p=rr->getPose();
			poseStream << p.x << " " << p.y << " " << p.theta << " ";
			poseStream << "0 pippo 0" << endl;
		}
	}
	poseStream.close();
}
