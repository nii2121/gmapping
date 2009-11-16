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


#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <utils/point.h>

#define MAXLINELENGHT (10240)
#define MAXREADINGS   (10240)

using namespace std;

int  main (int argc, const char** argv){
	if (argc<3){
		cout << "usage scanstudio2carmen scanfilename carmenfilename" << endl;
		exit(1);
	}
	ifstream is(argv[1]);
	if (!is){
		cout << "cannopt open file" << argv[1] << endl;
		exit(1);
	}
	
	ofstream os(argv[2]);
	
	double readings[MAXREADINGS];
	OrientedPoint pose;
	int nbeams;
	while (is){
		char buf[MAXLINELENGHT];
		is.getline(buf,MAXLINELENGHT);
		istringstream st(buf);
		string token;
		st>>token;
		if (token=="RobotPos:"){
			st >> pose.x >> pose.y >> pose.theta;
			pose.x/=1000;
			pose.y/=1000;
		} else
		if (token=="NumPoints:"){
			st >> nbeams;
			assert(nbeams<MAXREADINGS);
		} else
		if (token=="DATA"){
			int c=0;
			while (c<nbeams && is){
				double angle;
				is >> angle;
				is >> readings[c];
				readings[c]/=1000;
				c++;
			}
			if (c==nbeams)
				os << "FLASER " << nbeams << " ";
			c=0;
			while (c<nbeams){
				os << readings[c] << " ";
				c++;
			}	
			os << pose.x << " " << pose.y << " " << pose.theta << "0 0 0 0 pippo 0" << endl;
		}
		
	}
	os.close();
}
