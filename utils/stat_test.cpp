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
#include <math.h>
#include "stat.h"

using namespace std;

// struct Covariance3{
// 	double xx, yy, tt, xy, xt, yt;
// };

#define SAMPLES_NUMBER 1000

int main(int argc, char**  argv){
	Covariance3 cov={1.,0.01,0.01,0,0,0};
	EigenCovariance3 ecov(cov);
	cout << "EigenValues: " << ecov.eval[0] << " "<< ecov.eval[1] << " "  << ecov.eval[2] << endl;
	
	cout << "EigenVectors:" <<endl;
	cout<< ecov.evec[0][0] << " "<< ecov.evec[0][1] << " "  << ecov.evec[0][2] << endl;
	cout<< ecov.evec[1][0] << " "<< ecov.evec[1][1] << " "  << ecov.evec[1][2] << endl;
	cout<< ecov.evec[2][0] << " "<< ecov.evec[2][1] << " "  << ecov.evec[2][2] << endl;
	
	EigenCovariance3 rcov(ecov.rotate(M_PI/4));
	cout << "*************** Rotated ***************" << endl;
	cout << "EigenValues: " << rcov.eval[0] << " "<< rcov.eval[1] << " "  << rcov.eval[2] << endl;
	
	cout << "EigenVectors:" <<endl;
	cout<< rcov.evec[0][0] << " "<< rcov.evec[0][1] << " "  << rcov.evec[0][2] << endl;
	cout<< rcov.evec[1][0] << " "<< rcov.evec[1][1] << " "  << rcov.evec[1][2] << endl;
	cout<< rcov.evec[2][0] << " "<< rcov.evec[2][1] << " "  << rcov.evec[2][2] << endl;
	
	cout << "sampling:" << endl;
	ofstream fs("stat_test.dat");
	for (unsigned int i=0; i<SAMPLES_NUMBER; i++){
		OrientedPoint op=rcov.sample();
		fs << op.x << " " <<  op.y << " " << op.theta << endl;
	}
	fs.close();
}

