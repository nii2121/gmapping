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


#ifndef GRIDSLAMPROCESSOR_H
#define GRIDSLAMPROCESSOR_H

#include <gsl/gsl_eigen.h>
#include <fstream>
#include <vector>
#include <utils/dmatrix.h>
#include <deque>
#include <particlefilter/particlefilter.h>
#include <utils/point.h>
#include <utils/macro_params.h>
#include <log/sensorlog.h>
#include <sensor/sensor_range/rangesensor.h>
#include <sensor/sensor_range/rangereading.h>
#include <scanmatcher/scanmatcher.h>
#include "motionmodel.h"


class GridSlamProcessor{
 public:

  struct TNode{
    TNode(const OrientedPoint& pose, double weight, TNode* parent=0, unsigned int childs=0);
    ~TNode();
    OrientedPoint pose;
    double weight;
    double accWeight;
    TNode* parent;
    const RangeReading* reading;
    unsigned int childs;
    mutable unsigned int visitCounter;
    mutable bool flag;
  };
		
  struct Particle{
    Particle(const ScanMatcherMap& map);
    inline operator double() const {return weight;}
    inline operator OrientedPoint() const {return pose;}
    inline void setWeight(double w) {weight=w;}
    ScanMatcherMap map;
    OrientedPoint pose;
    double weight;
    double weightSum;
    int previousIndex;
    TNode* node; 
  };	

  typedef std::vector<GridSlamProcessor::TNode*> TNodeVector;
  typedef std::deque<GridSlamProcessor::TNode*> TNodeDeque;		
  typedef std::vector<Particle> ParticleVector;


  // public constructors
  GridSlamProcessor();
  GridSlamProcessor(std::ostream& infoStr);
  GridSlamProcessor* clone() const;
  virtual ~GridSlamProcessor();
		
  //methods for accessing the parameters
  void setSensorMap(const SensorMap& smap);
  void init(unsigned int size, double xmin, double ymin, double xmax, double ymax,
	    double delta, OrientedPoint initialPose=OrientedPoint(0,0,0), bool mode=false);
  void setMatchingParameters(double urange, double range, double sigma, 
			     int kernsize, double lopt, double aopt, int iterations, 
			     double likelihoodSigma=1, double likelihoodGain=1, 
			     unsigned int likelihoodSkip=0);
  void setMotionModelParameters(double srr, double srt, double str, double stt);
  void setUpdateDistances(double linear, double angular, double resampleThreshold);
  
  //the "core" algorithm
  void processTruePos(const OdometryReading& odometry);
  bool processScan(const RangeReading & reading, int adaptParticles=0);
				
  // for saving the filter state in a tree!
  TNodeVector getTrajectories() const;
  void integrateScanSequence(TNode* node);
		
  inline const ParticleVector& getParticles() const {return m_particles; }
  inline const std::vector<unsigned int>& getIndexes() const{return m_indexes; }
  int getBestParticleIndex() const;

  //callbacks
  virtual void onOdometryUpdate();
  virtual void onResampleUpdate();
  virtual void onScanmatchUpdate();
	
  //accessor methods
  MEMBER_PARAM_SET_GET(m_matcher, double, laserMaxRange, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher, double, usableRange, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher,double, gaussianSigma, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher,double, likelihoodSigma, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher, int,    kernelSize, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher, double, optAngularDelta, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher, double, optLinearDelta, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher, unsigned int, optRecursiveIterations, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher, unsigned int, likelihoodSkip, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher, double, llsamplerange, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher, double, lasamplerange, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher, double, llsamplestep, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher, double, lasamplestep, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher, bool, generateMap, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher, bool, enlargeStep, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher, double, linearOdometryReliability, protected, public, public)
  MEMBER_PARAM_SET_GET(m_matcher, double, angularOdometryReliability, protected, public, public)
  STRUCT_PARAM_SET_GET(m_motionModel, double, srr, protected, public, public)
  STRUCT_PARAM_SET_GET(m_motionModel, double, srt, protected, public, public)
  STRUCT_PARAM_SET_GET(m_motionModel, double, str, protected, public, public)
  STRUCT_PARAM_SET_GET(m_motionModel, double, stt, protected, public, public)
  PARAM_SET_GET(bool, covScanmatch, protected, public, public)
  PARAM_SET_GET(bool, considerOdometryCovariance, protected, public, public)
  PARAM_SET_GET(bool, performScanmatching, protected, public, public)
  PARAM_SET_GET(double, minimumScore, protected, public, public)

  std::ofstream& outputStream();
  std::ostream& infoStream();

 protected:
  GridSlamProcessor(const GridSlamProcessor& gsp);

  //processing parameters
  PARAM_SET_GET(double, resampleThreshold, protected, public, public)
  PARAM_GET(double, neff, protected, public)
  PARAM_GET(double, xmin, protected, public)
  PARAM_GET(double, ymin, protected, public)
  PARAM_GET(double, xmax, protected, public)
  PARAM_GET(double, ymax, protected, public)
  PARAM_GET(double, delta, protected, public)
  PARAM_SET_GET(double, linearThresholdDistance, protected, public, public)
  PARAM_SET_GET(double, angularThresholdDistance, protected, public, public)
  PARAM_SET_GET(double, obsSigmaGain, protected, public, public)



/*   PARAM_SET_GET(double, regScore, protected, public, public) */
/*   PARAM_SET_GET(double, critScore, protected, public, public) */
/*   PARAM_SET_GET(double, maxMove, protected, public, public) */
/*   PARAM_SET_GET(OrientedPoint, entropyDelta, protected, public, public) */

 protected:  
  ScanMatcher m_matcher;

  Covariance3 m_odometryCovariance;
  unsigned int m_beams;
  
  ParticleVector m_particles;
  std::vector<unsigned int> m_indexes;
  std::vector<double> m_weights;
  
  MotionModel m_motionModel;
  MotionModel m_perfectModel;
	
  int  m_count, m_readingCount;
  OrientedPoint m_lastPartPose;
  OrientedPoint m_odoPose;
  OrientedPoint m_pose;
  double m_linearDistance, m_angularDistance;

  std::ofstream m_outputStream;
  std::ostream& m_infoStream;
	
  gsl_eigen_symmv_workspace * m_eigenspace;
  gsl_matrix * m_cmat;
  gsl_matrix * m_evec;
  gsl_vector * m_eval;
  gsl_vector * m_noise;
  gsl_vector * m_pnoise;

  // the functions below performs side effect on the internal structure,
  //should be called only inside the processScan method
		
  OrientedPoint drawFromObservation(const OrientedPoint& pose, double lmax=0);
  void updateEigenSystem(const ScanMatcher::CovarianceMatrix& cov);
 private:
  void scanMatch(const double *plainReading);
  void normalize();
  
  // return if a resampling occured or not
  bool resample(const double* plainReading, int adaptParticles, 
		const RangeReading* rr=0);
  
  //tree utilities  
  void updateTreeWeights(bool weightsAlreadyNormalized = false);
  void resetTree();
  double propagateWeights();
};

typedef std::multimap<const GridSlamProcessor::TNode*, GridSlamProcessor::TNode*> TNodeMultimap;


#endif
