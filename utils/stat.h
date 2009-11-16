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


#ifndef STAT_H
#define STAT_H
#include "point.h"

/**stupid utility function for drawing particles form a zero mean, sigma variance normal distribution
probably it should not go there*/
double sampleGaussian(double sigma,unsigned long int S=0);

double evalGaussian(double sigmaSquare, double delta);
double evalLogGaussian(double sigmaSquare, double delta);

struct Covariance3{
	Covariance3 operator + (const Covariance3 & cov) const;
	static Covariance3 zero;
	double xx, yy, tt, xy, xt, yt;
};

struct EigenCovariance3{
	EigenCovariance3();
	EigenCovariance3(const Covariance3& c);
	EigenCovariance3 rotate(double angle) const;
	OrientedPoint sample() const;
	double eval[3];
	double evec[3][3];
};

struct Gaussian3{
	OrientedPoint mean;
	EigenCovariance3 covariance;
	double eval(const OrientedPoint& p) const;
};



#endif

