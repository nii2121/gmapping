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
#include "autoptr.h"

using namespace std;
typedef autoptr<double> DoubleAutoPtr;

int main(int argc, const char * const * argv){
	double* d1=new double(10.);
	double* d2=new double(20.);
	cout << "Construction test" << endl;
	DoubleAutoPtr pd1(d1);
	DoubleAutoPtr pd2(d2);
	cout << *pd1 << " " << *pd2 << endl;
	cout << "Copy Construction" << endl;
	DoubleAutoPtr pd3(pd1);
	cout << *pd3 << endl;
	cout << "assignment" << endl;
	pd3=pd2;
	pd1=pd2;
	cout << *pd1 << " " << *pd2 << " " << *pd3 << " " << endl;
	cout << "conversion operator" << endl;
	DoubleAutoPtr nullPtr;
	cout << "conversion operator " << !(nullPtr) << endl;
	cout << "neg conversion operator " << nullPtr << endl;
	cout << "conversion operator " << (int)pd1 << endl;
	cout << "neg conversion operator " << !(pd1) << endl;
}
