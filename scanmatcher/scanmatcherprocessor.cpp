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
#include "scanmatcherprocessor.h"

using namespace std;

ScanMatcherProcessor::ScanMatcherProcessor(const ScanMatcherMap& m)
  : m_map(m.getCenter(), m.getWorldSizeX(), m.getWorldSizeY(), m.getResolution()), 
    m_pose(0,0,0){
  m_regScore=300;
  m_critScore=.5*m_regScore;
  m_maxMove=1;
  m_beams=0;
  m_computeCovariance=false;
  m_eigenspace=gsl_eigen_symmv_alloc(3);
}


ScanMatcherProcessor::ScanMatcherProcessor 
  (double xmin, double ymin, double xmax, double ymax, double delta, double patchdelta):
  m_map(Point((xmax+xmin)*.5, (ymax+ymin)*.5), xmax-xmin, ymax-ymin, patchdelta), m_pose(0,0,0){
	m_regScore=300;
	m_critScore=.5*m_regScore;
	m_maxMove=1;
	m_beams=0;
	m_computeCovariance=false;
	m_eigenspace=gsl_eigen_symmv_alloc(3);
}

ScanMatcherProcessor::~ScanMatcherProcessor (){
	gsl_eigen_symmv_free(m_eigenspace);
}


void ScanMatcherProcessor::setSensorMap(const SensorMap& smap){
	m_sensorMap=smap;
	
	/*
	 Construct the angle table for the sensor
	 
	 FIXME has to be extended to more than one laser... 
	*/
	
	SensorMap::const_iterator laser_it=m_sensorMap.find(string("FLASER"));
	assert(laser_it!=m_sensorMap.end());
	const RangeSensor* rangeSensor=dynamic_cast<const RangeSensor*>((laser_it->second));
	assert(rangeSensor && rangeSensor->beams().size());
	
	m_beams=rangeSensor->beams().size();
	double* angles=new double[rangeSensor->beams().size()];
	for (unsigned int i=0; i<m_beams; i++){
		angles[i]=rangeSensor->beams()[i].pose.theta;
	}
	m_matcher.setLaserParameters(m_beams, angles, rangeSensor->getPose());
	delete [] angles;
	
	
}

void ScanMatcherProcessor::init(){
	m_first=true;
	m_pose=OrientedPoint(0,0,0);
	m_count=0;
}

void ScanMatcherProcessor::processScan(const RangeReading & reading){
	/**retireve the position from the reading, and compute the odometry*/
	OrientedPoint relPose=reading.getPose();
	if (!m_count){
		m_odoPose=relPose;
	}

	//compute the move in the scan m_matcher
	//reference frame

	OrientedPoint move=relPose-m_odoPose;

	double dth=m_odoPose.theta-m_pose.theta;
	// cout << "rel-move x="<< move.x <<  " y=" << move.y << " theta=" << move.theta << endl;

	double lin_move=move*move;
	if (lin_move>m_maxMove){
		cout << "Too big jump in the log file: " << lin_move << endl;
		cout << "relPose=" << relPose.x << " " <<relPose.y << endl;
		cout << "aborting" << endl;
		assert(0);
		dth=0;
		move.x=move.y=move.theta=0;
	}
	
	double s=sin(dth), c=cos(dth);
	OrientedPoint dPose;
	dPose.x=c*move.x-s*move.y;
	dPose.y=s*move.x+c*move.y;
	dPose.theta=move.theta;

	cout << "abs-move x="<< dPose.x <<  " y=" << dPose.y << " theta=" << dPose.theta << endl;

	m_pose=m_pose+dPose;
	m_pose.theta=atan2(sin(m_pose.theta), cos(m_pose.theta));

	cout << "StartPose: x="
	<< m_pose.x << " y=" << m_pose.y << " theta=" << m_pose.theta << endl;

	m_odoPose=relPose; //update the past pose for the next iteration


	//FIXME here I assume that everithing is referred to the center of the robot,
	//while the offset of the laser has to be taken into account
	
	assert(reading.size()==m_beams);
	double * plainReading = new double[m_beams];
	for(unsigned int i=0; i<m_beams; i++)
		plainReading[i]=reading[i];
	//the final stuff: scan match the pose
	double score=0;
	OrientedPoint newPose=m_pose;
	if (m_count){
		if(m_computeCovariance){
			ScanMatcher::CovarianceMatrix cov;
			score=m_matcher.optimize(newPose, cov, m_map, m_pose, plainReading);
			gsl_matrix* m=gsl_matrix_alloc(3,3);
			gsl_matrix_set(m,0,0,cov.xx); gsl_matrix_set(m,0,1,cov.xy); gsl_matrix_set(m,0,2,cov.xt);
			gsl_matrix_set(m,1,0,cov.xy); gsl_matrix_set(m,1,1,cov.yy); gsl_matrix_set(m,1,2,cov.yt);
			gsl_matrix_set(m,2,0,cov.xt); gsl_matrix_set(m,2,1,cov.yt); gsl_matrix_set(m,2,2,cov.tt);
			gsl_matrix* evec=gsl_matrix_alloc(3,3);
			gsl_vector* eval=gsl_vector_alloc(3);
			gsl_eigen_symmv (m, eval,  evec, m_eigenspace);
			cout << "evals=" << gsl_vector_get(eval, 0) <<  " " << gsl_vector_get(eval, 1)<< " " << gsl_vector_get(eval, 2)<<endl;
			gsl_matrix_free(m);
			gsl_matrix_free(evec);
			gsl_vector_free(eval);
		} else {
			score=m_matcher.optimize(newPose, m_map, m_pose, plainReading);
		}
		
		
	}
	//...and register the scan
	if (!m_count || score<m_regScore){
		cout << "Registering" << endl;
		m_matcher.invalidateActiveArea();
		if (score<m_critScore){
			cout << "New Scan added, using odo pose" << endl;
			m_matcher.registerScan(m_map, m_pose, plainReading);
		} else {
			m_matcher.registerScan(m_map, newPose, plainReading);;
			cout << "New Scan added, using matched pose" << endl;
		}	
	}

	cout << " FinalPose: x="
		<< newPose.x << " y=" << newPose.y << " theta=" << newPose.theta << endl;
	cout << "score=" << score << endl;
	m_pose=newPose;
	delete [] plainReading;
	m_count++;
}

void ScanMatcherProcessor::setMatchingParameters
	(double urange, double range, double sigma, int kernsize, double lopt, double aopt, int iterations, bool computeCovariance){
	m_matcher.setMatchingParameters(urange, range,  sigma, kernsize, lopt, aopt, iterations);
	m_computeCovariance=computeCovariance;
}

void ScanMatcherProcessor::setRegistrationParameters(double regScore, double critScore){
	m_regScore=regScore;
	m_critScore=critScore;
}
		

OrientedPoint ScanMatcherProcessor::getPose() const{
	return m_pose;
}

