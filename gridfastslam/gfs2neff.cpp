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

using namespace std;

int main(unsigned int argc, const char* const *argv){
	if (argc<3){
		cout << "usage gfs2neff <infilename> <nefffilename>" << endl;
		return -1;
	}
	ifstream is(argv[1]);
	if (!is){
		cout << "could read file "<< endl;
		return -1;
	}
	ofstream os(argv[2]);
	if (! os){
		cout << "could write file "<< endl;
		return -1;
	}
	unsigned int frame=0;
	double neff=0;
	while(is){
			char buf[8192];
			is.getline(buf, 8192);
			istringstream lineStream(buf);
			string recordType;
			lineStream >> recordType;
			if (recordType=="FRAME"){
				lineStream>> frame;
			}
			if (recordType=="NEFF"){
				lineStream>> neff;
				os << frame << " " << neff << endl;
			}
	}
	os.close();
}
