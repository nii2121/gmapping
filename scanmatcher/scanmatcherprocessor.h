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


#ifndef SCANMATCHERPROCESSOR_H
#define SCANMATCHERPROCESSOR_H

#include <log/sensorlog.h>
#include <sensor/sensor_range/rangesensor.h>
#include <sensor/sensor_range/rangereading.h>
#include <gsl/gsl_eigen.h>
#include "scanmatcher.h"


class ScanMatcherProcessor{
	public:
  ScanMatcherProcessor(const ScanMatcherMap& m);
  ScanMatcherProcessor (double xmin, double ymin, double xmax, double ymax, double delta, double patchdelta);
		~ScanMatcherProcessor ();
		void processScan(const RangeReading & reading);
		void setSensorMap(const SensorMap& smap);
		void init();
		void setMatchingParameters
			(double urange, double range, double sigma, int kernsize, double lopt, double aopt, int iterations, bool computeCovariance=false);
		void setRegistrationParameters(double regScore, double critScore);
		OrientedPoint getPose() const;
		inline const ScanMatcherMap& getMap() const {return m_map;}
	protected:
		ScanMatcher m_matcher;
		bool m_computeCovariance;
		bool m_first;
		SensorMap m_sensorMap;
		double m_regScore, m_critScore;
		unsigned int m_beams;
		double m_maxMove;
		//state
		ScanMatcherMap m_map;
		OrientedPoint m_pose;
		OrientedPoint m_odoPose;
		int  m_count;
		gsl_eigen_symmv_workspace * m_eigenspace;
};


#endif


