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


#ifndef SMMAP_H
#define SMMAP_H
#include <grid/map.h>
#include <grid/harray2d.h>
#include <utils/point.h>
#define SIGHT_INC 1

struct PointAccumulator{
	typedef point<float> FloatPoint;
	PointAccumulator(int i=-1): acc(0,0), n(0), visits(0){assert(i==-1);}
        inline void update(bool value, const Point& p=Point(0,0));
	inline Point mean() const {return 1./n*Point(acc.x, acc.y);}
	inline operator double() const { return visits?(double)n*SIGHT_INC/(double)visits:-1; }
	inline void add(const PointAccumulator& p) {acc=acc+p.acc; n+=p.n; visits+=p.visits; }
	static const PointAccumulator& Unknown();
	static PointAccumulator* unknown_ptr;
	FloatPoint acc;
	int n, visits;
	inline double entropy() const;
};

void PointAccumulator::update(bool value, const Point& p){
	if (value) {
		acc.x+=p.x;
		acc.y+=p.y; 
		n++; 
		visits+=SIGHT_INC;
	} else
		visits++;
}

double PointAccumulator::entropy() const{
	if (!visits)
		return -log(.5);
	if (n==visits || n==0)
		return 0;
	double x=(double)n*SIGHT_INC/(double)visits;
	return -( x*log(x)+ (1-x)*log(1-x) );
}


typedef Map<PointAccumulator,HierarchicalArray2D<PointAccumulator> > ScanMatcherMap;

#endif 
