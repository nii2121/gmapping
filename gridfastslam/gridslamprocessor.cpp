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


#include <values.h>
#include <string>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <fstream>
#include <gsl/gsl_blas.h>

#include <utils/stat.h>
#include "gridslamprocessor.h"

//#define MAP_CONSISTENCY_CHECK
//#define GENERATE_TRAJECTORIES

const double m_distanceThresholdCheck = 20;
 
using namespace std;


GridSlamProcessor::GridSlamProcessor(): m_infoStream(cout){
 
  m_eigenspace=gsl_eigen_symmv_alloc(3);
  m_cmat=gsl_matrix_alloc(3,3);
  m_evec=gsl_matrix_alloc(3,3);
  m_eval=gsl_vector_alloc(3);
  m_noise=gsl_vector_alloc(3);
  m_pnoise=gsl_vector_alloc(3);
	
  m_perfectModel.srr=0;
  m_perfectModel.srt=0;
  m_perfectModel.str=0;
  m_perfectModel.stt=0;
  m_obsSigmaGain=1;
  m_covScanmatch=false;
  m_resampleThreshold=0.5;
  m_performScanmatching=true;
  m_minimumScore=0.;

  m_considerOdometryCovariance=false;
  m_odometryCovariance=Covariance3::zero;
}

GridSlamProcessor::GridSlamProcessor(const GridSlamProcessor& gsp) 
  :m_particles(gsp.m_particles), m_infoStream(cout){
  m_eigenspace=gsl_eigen_symmv_alloc(3);
  m_cmat=gsl_matrix_alloc(3,3);
  m_evec=gsl_matrix_alloc(3,3);
  m_eval=gsl_vector_alloc(3);
  m_noise=gsl_vector_alloc(3);
  m_pnoise=gsl_vector_alloc(3);

  m_obsSigmaGain=gsp.m_obsSigmaGain;
  m_covScanmatch=gsp.m_covScanmatch;
  m_resampleThreshold=gsp.m_resampleThreshold;
  m_performScanmatching=gsp.m_performScanmatching;
  m_minimumScore=gsp.m_minimumScore;

  m_beams=gsp.m_beams;
  m_indexes=gsp.m_indexes;
  m_motionModel=gsp.m_motionModel;
  m_perfectModel=gsp.m_perfectModel;
  m_resampleThreshold=gsp.m_resampleThreshold;
  m_matcher=gsp.m_matcher;
		
  m_count=gsp.m_count;
  m_readingCount=gsp.m_readingCount;
  m_lastPartPose=gsp.m_lastPartPose;
  m_pose=gsp.m_pose;
  m_odoPose=gsp.m_odoPose;
  m_linearDistance=gsp.m_linearDistance;
  m_angularDistance=gsp.m_angularDistance;
  m_neff=gsp.m_neff;
	

  if (m_infoStream) {
    m_infoStream << "FILTER COPY CONSTRUCTOR" << endl;
    m_infoStream << "m_odoPose=" << m_odoPose.x << " " <<m_odoPose.y << " " << m_odoPose.theta << endl;
    m_infoStream << "m_lastPartPose=" << m_lastPartPose.x << " " <<m_lastPartPose.y << " " << m_lastPartPose.theta << endl;
    m_infoStream << "m_linearDistance=" << m_linearDistance << endl;
    m_infoStream << "m_angularDistance=" << m_linearDistance << endl;
  }
  m_xmin=gsp.m_xmin;
  m_ymin=gsp.m_ymin;
  m_xmax=gsp.m_xmax;
  m_ymax=gsp.m_ymax;
  m_delta=gsp.m_delta;
	
//   m_regScore=gsp.m_regScore;
//   m_critScore=gsp.m_critScore;
//   m_maxMove=gsp.m_maxMove;
	
  m_linearThresholdDistance=gsp.m_linearThresholdDistance;
  m_angularThresholdDistance=gsp.m_angularThresholdDistance;
  m_obsSigmaGain=gsp.m_obsSigmaGain;

  m_considerOdometryCovariance=gsp.m_considerOdometryCovariance;
  m_odometryCovariance=gsp.m_odometryCovariance;
#ifdef MAP_CONSISTENCY_CHECK
  if (m_infoStream)
    m_infoStream << __PRETTY_FUNCTION__ <<  ": trajectories copy.... ";
#endif
  TNodeVector v=gsp.getTrajectories();
  for (unsigned int i=0; i<v.size(); i++){
    m_particles[i].node=v[i];
  }
#ifdef MAP_CONSISTENCY_CHECK
  if (m_infoStream)
    m_infoStream <<  "end" << endl;
#endif


  if (m_infoStream)
    m_infoStream  << "Tree: normalizing, resetting and propagating weights within copy construction/cloneing ..." ;
  updateTreeWeights(false);

  if (m_infoStream)
    m_infoStream  << ".done!" <<endl;
}

GridSlamProcessor::GridSlamProcessor(std::ostream& infoS): m_infoStream(infoS){
  m_eigenspace=gsl_eigen_symmv_alloc(3);
  m_cmat=gsl_matrix_alloc(3,3);
  m_evec=gsl_matrix_alloc(3,3);
  m_eval=gsl_vector_alloc(3);
  m_noise=gsl_vector_alloc(3);
  m_pnoise=gsl_vector_alloc(3);
	
  m_perfectModel.srr=0;
  m_perfectModel.srt=0;
  m_perfectModel.str=0;
  m_perfectModel.stt=0;
  m_obsSigmaGain=1;
  m_covScanmatch=false;
  m_resampleThreshold=0.5;
  m_performScanmatching=true;
  m_minimumScore=0.;
	
  m_considerOdometryCovariance=false;
  m_odometryCovariance=Covariance3::zero;
}

GridSlamProcessor* GridSlamProcessor::clone() const {
# ifdef MAP_CONSISTENCY_CHECK
  if (m_infoStream)
    m_infoStream << __PRETTY_FUNCTION__ << ": performing preclone_fit_test" << endl;
  typedef std::map<autoptr< Array2D<PointAccumulator> >::reference* const, int> PointerMap;
  PointerMap pmap;
  for (ParticleVector::const_iterator it=m_particles.begin(); it!=m_particles.end(); it++){
    const ScanMatcherMap& m1(it->map);
    const HierarchicalArray2D<PointAccumulator>& h1(m1.storage());
    for (int x=0; x<h1.getXSize(); x++){
      for (int y=0; y<h1.getYSize(); y++){
	const autoptr< Array2D<PointAccumulator> >& a1(h1.m_cells[x][y]);
	if (a1.m_reference){
	  PointerMap::iterator f=pmap.find(a1.m_reference);
	  if (f==pmap.end())
	    pmap.insert(make_pair(a1.m_reference, 1));
	  else
	    f->second++;
	}
      }
    }
  }
  if (m_infoStream) 
    m_infoStream << __PRETTY_FUNCTION__ <<  ": Number of allocated chunks" << pmap.size() << endl;
  for(PointerMap::const_iterator it=pmap.begin(); it!=pmap.end(); it++)
    assert(it->first->shares==(unsigned int)it->second);
  if (m_infoStream)
    m_infoStream << __PRETTY_FUNCTION__ <<  ": SUCCESS, the error is somewhere else" << endl;
# endif
  GridSlamProcessor* cloned=new GridSlamProcessor(*this);

# ifdef MAP_CONSISTENCY_CHECK
  if (m_infoStream)
    m_infoStream << __PRETTY_FUNCTION__ <<  ": trajectories end" << endl;
  if (m_infoStream)
    m_infoStream << __PRETTY_FUNCTION__ << ": performing afterclone_fit_test" << endl;
  ParticleVector::const_iterator jt=cloned->m_particles.begin();
  for (ParticleVector::const_iterator it=m_particles.begin(); it!=m_particles.end(); it++){
    const ScanMatcherMap& m1(it->map);
    const ScanMatcherMap& m2(jt->map);
    const HierarchicalArray2D<PointAccumulator>& h1(m1.storage());
    const HierarchicalArray2D<PointAccumulator>& h2(m2.storage());
    jt++;
    for (int x=0; x<h1.getXSize(); x++){
      for (int y=0; y<h1.getYSize(); y++){
	const autoptr< Array2D<PointAccumulator> >& a1(h1.m_cells[x][y]);
	const autoptr< Array2D<PointAccumulator> >& a2(h2.m_cells[x][y]);
	assert(a1.m_reference==a2.m_reference);
	assert((!a1.m_reference) || !(a1.m_reference->shares%2));
      }
    }
  }
  if (m_infoStream)
    m_infoStream << __PRETTY_FUNCTION__ <<  ": SUCCESS." << endl;
# endif
  return cloned;
}

GridSlamProcessor::~GridSlamProcessor(){
  if (m_infoStream)
    m_infoStream << __PRETTY_FUNCTION__ << ": Start" << endl;
  gsl_eigen_symmv_free(m_eigenspace);
  gsl_matrix_free(m_cmat);
  gsl_matrix_free(m_evec);
  gsl_vector_free(m_eval);
  gsl_vector_free(m_noise);
  gsl_vector_free(m_pnoise);
  if (m_infoStream)
    m_infoStream << __PRETTY_FUNCTION__ << ": Deeting tree" << endl;
  for (std::vector<Particle>::iterator it=m_particles.begin(); it!=m_particles.end(); it++){
#ifdef TREE_CONSISTENCY_CHECK		
    TNode* node=it->node;
    while(node)
      node=node->parent;
    if (m_infoStream)
      m_infoStream << "@" << endl;
#endif
    if (it->node)
      delete it->node;
  }
	
# ifdef MAP_CONSISTENCY_CHECK
  if (m_infoStream)
    m_infoStream << __PRETTY_FUNCTION__ << ": performing predestruction_fit_test" << endl;
  typedef std::map<autoptr< Array2D<PointAccumulator> >::reference* const, int> PointerMap;
  PointerMap pmap;
  for (ParticleVector::const_iterator it=m_particles.begin(); it!=m_particles.end(); it++){
    const ScanMatcherMap& m1(it->map);
    const HierarchicalArray2D<PointAccumulator>& h1(m1.storage());
    for (int x=0; x<h1.getXSize(); x++){
      for (int y=0; y<h1.getYSize(); y++){
	const autoptr< Array2D<PointAccumulator> >& a1(h1.m_cells[x][y]);
	if (a1.m_reference){
	  PointerMap::iterator f=pmap.find(a1.m_reference);
	  if (f==pmap.end())
	    pmap.insert(make_pair(a1.m_reference, 1));
	  else
	    f->second++;
	}
      }
    }
  }
  if (m_infoStream)
    m_infoStream << __PRETTY_FUNCTION__ << ": Number of allocated chunks" << pmap.size() << endl;
  for(PointerMap::const_iterator it=pmap.begin(); it!=pmap.end(); it++)
    assert(it->first->shares>=(unsigned int)it->second);
  if (m_infoStream)
    m_infoStream << __PRETTY_FUNCTION__ << ": SUCCESS, the error is somewhere else" << endl;
# endif
}


		
void GridSlamProcessor::setMatchingParameters(double urange, double range, double sigma, int kernsize, double lopt, double aopt, int iterations, double likelihoodSigma, double likelihoodGain, unsigned int likelihoodSkip){
  m_obsSigmaGain=likelihoodGain;
  m_matcher.setMatchingParameters(urange, range, sigma, kernsize, lopt, aopt, iterations, likelihoodSigma, likelihoodSkip);
  if (m_infoStream)
    m_infoStream << " -maxUrange "<< urange
		 << " -maxUrange "<< range
		 << " -sigma     "<< sigma
		 << " -kernelSize "<< kernsize
		 << " -lstep "    << lopt
		 << " -lobsGain " << m_obsSigmaGain
		 << " -astep "    << aopt << endl;
	     
	
}

void GridSlamProcessor::setMotionModelParameters(double srr, double srt, double str, double stt){
  m_motionModel.srr=srr;
  m_motionModel.srt=srt;
  m_motionModel.str=str;
  m_motionModel.stt=stt;	

  if (m_infoStream)
    m_infoStream << " -srr "<< srr 	<< " -srt "<< srt  
		 << " -str "<< str 	<< " -stt "<< stt << endl;

}

void GridSlamProcessor::setUpdateDistances(double linear, double angular, double resampleThreshold){
  m_linearThresholdDistance=linear; 
  m_angularThresholdDistance=angular;
  m_resampleThreshold=resampleThreshold;	
  if (m_infoStream)
    m_infoStream << " -linearUpdate " << linear
		 << " -angularUpdate "<< angular
		 << " -resampleThreshold " << m_resampleThreshold << endl;
}

//HERE STARTS THE BEEF

GridSlamProcessor::Particle::Particle(const ScanMatcherMap& m):
  map(m), pose(0,0,0), weight(0), weightSum(0), previousIndex(0){
  node=0;
}

	
void GridSlamProcessor::setSensorMap(const SensorMap& smap){
	
  /*
    Construct the angle table for the sensor
	 
    FIXME For now detect the readings of only the front laser, and assume its pose is in the center of the robot 
  */
	
  SensorMap::const_iterator laser_it=smap.find(std::string("FLASER"));
  assert(laser_it!=smap.end());
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

void GridSlamProcessor::init(unsigned int size, double xmin, double ymin, double xmax, double ymax, double delta, OrientedPoint initialPose, bool mode){
  m_xmin=xmin;
  m_ymin=ymin;
  m_xmax=xmax;
  m_ymax=ymax;
  m_delta=delta;
  m_covScanmatch=mode;
  if (m_infoStream)
    m_infoStream 
      << " -xmin "<< m_xmin
      << " -xmax "<< m_xmax
      << " -ymin "<< m_ymin
      << " -ymax "<< m_ymax
      << " -delta "<< m_delta
      << " -particles "<< size
      << " -covScanMatch "<< m_covScanmatch << endl;
			

  m_particles.clear();
  TNode* node=new TNode(initialPose, 0, 0, 0);
  ScanMatcherMap lmap(Point(xmin+xmax, ymin+ymax)*.5, xmax-xmin, ymax-ymin, delta);
  for (unsigned int i=0; i<size; i++){
    m_particles.push_back(Particle(lmap));
    m_particles.back().pose=initialPose;
    m_particles.back().setWeight(0);
    m_particles.back().previousIndex=0;

    // this is not needed
    //		m_particles.back().node=new TNode(initialPose, 0, node, 0);

    // we use the root directly
    m_particles.back().node= node;
  }
  m_neff=(double)size;
  m_count=0;
  m_readingCount=0;
  m_linearDistance=m_angularDistance=0;
}

void GridSlamProcessor::processTruePos(const OdometryReading& o){
  const OdometrySensor* os=dynamic_cast<const OdometrySensor*>(o.getSensor());
  if (os && os->isIdeal() && m_outputStream){
    m_outputStream <<  "SIMULATOR_POS " <<  o.getPose().x << " " << o.getPose().y << " " 
		   << o.getPose().theta << " " <<o.getTime() << endl;
  }
}


bool GridSlamProcessor::processScan(const RangeReading & reading, int adaptParticles){

  /**retireve the position from the reading, and compute the odometry*/
  OrientedPoint relPose=reading.getPose();
  if (!m_count){
    m_lastPartPose=m_odoPose=relPose;
  }
	
  //write the state of the reading and the subsequent particle updates
  for (ParticleVector::iterator it=m_particles.begin(); it!=m_particles.end(); it++){
    OrientedPoint& pose(it->pose);
    pose=m_perfectModel.drawFromMotion(pose, relPose, m_odoPose);
    m_odometryCovariance=m_odometryCovariance+m_motionModel.gaussianApproximation(relPose, m_odoPose);
  }
  if (m_outputStream.is_open()){
    m_outputStream << "ODO_UPDATE "<< m_particles.size() << " ";
    for (ParticleVector::iterator it=m_particles.begin(); it!=m_particles.end(); it++){
      OrientedPoint& pose(it->pose);
      m_outputStream << pose.x << " " << pose.y << " " << pose.theta << " " << it-> weight << " ";
    }
    m_outputStream << reading.getTime();
    m_outputStream << endl;
  }
	
  onOdometryUpdate();
	
  OrientedPoint move=relPose-m_odoPose;
  move.theta=atan2(sin(move.theta), cos(move.theta));
  m_linearDistance+=sqrt(move*move);
  m_angularDistance+=fabs(move.theta);

  if (m_linearDistance>m_distanceThresholdCheck){
    if (m_infoStream)
      m_infoStream << "***********************************************************************" << endl;
    cerr << "********** Error: m_distanceThresholdCheck overridden!!!! *************" << endl;
    cerr << "m_distanceThresholdCheck=" << m_distanceThresholdCheck << endl;
    cerr << "Old Odometry Pose= " << m_odoPose.x << " " << m_odoPose.y 
	 << " " <<m_odoPose.theta << endl;
    cerr << "New Odometry Pose (reported from observation)= " << relPose.x << " " << relPose.y 
	 << " " <<relPose.theta << endl;
    cerr << "***********************************************************************" << endl;
    cerr << "** The Odometry has a big jump here. This is probably a bug in the   **" << endl;
    cerr << "** odometry/laser input. We continue now, but the result is probably **" << endl;
    cerr << "** crap or can lead to a core dump since the map doesn't fit.... C&G **" << endl;
    cerr << "***********************************************************************" << endl;
  }
	
  m_odoPose=relPose;
	
  bool processed=false;
  if (! m_count 
      || m_linearDistance>m_linearThresholdDistance 
      || m_angularDistance>m_angularThresholdDistance){

    if (m_outputStream.is_open()){
      m_outputStream << "FRAME " <<  m_readingCount;
      m_outputStream << " " << m_linearDistance;
      m_outputStream << " " << m_angularDistance << endl;
    }
		
    if (m_infoStream)
      m_infoStream << "update frame " <<  m_readingCount << endl
		   << "update ld=" << m_linearDistance << " ad=" << m_angularDistance << endl;


    if (m_infoStream)
      m_infoStream << "Laser Pose= " << reading.getPose().x << " " << reading.getPose().y 
		   << " " << reading.getPose().theta << endl;

		
    //this is for converting the reading in a scan-matcher feedable form
    assert(reading.size()==m_beams);
    double * plainReading = new double[m_beams];
    for(unsigned int i=0; i<m_beams; i++){
      plainReading[i]=reading[i];
    }
    m_infoStream << "m_count " << m_count << endl;
    if (m_count>0){
      scanMatch(plainReading);
      if (m_outputStream.is_open()){
	m_outputStream << "LASER_READING "<< reading.size() << " ";
	for (RangeReading::const_iterator b=reading.begin(); b!=reading.end(); b++){
	  m_outputStream << *b << " ";
	}
	OrientedPoint p=reading.getPose();
	m_outputStream << p.x << " " << p.y << " " << p.theta << " " << reading.getTime()<< endl;
	m_outputStream << "SM_UPDATE "<< m_particles.size() << " ";
	for (ParticleVector::const_iterator it=m_particles.begin(); it!=m_particles.end(); it++){
	  const OrientedPoint& pose=it->pose;
	  m_outputStream <<  pose.x << " " << pose.y << " " << pose.theta << " " << it-> weight << " ";
	}
	m_outputStream << endl;
      }
      onScanmatchUpdate();
      updateTreeWeights(false);

      if (m_outputStream.is_open()){
	m_outputStream << "NEFF " << m_neff << endl;
      }
      resample(plainReading, adaptParticles);
		
    } else {
      m_infoStream << "Registering First Scan"<< endl;
      for (ParticleVector::iterator it=m_particles.begin(); it!=m_particles.end(); it++){	
	m_matcher.invalidateActiveArea();
	m_matcher.computeActiveArea(it->map, it->pose, plainReading);
	m_matcher.registerScan(it->map, it->pose, plainReading);

	// cyr: not needed anymore, particles refer to the root in the beginning!
	TNode* node=new	TNode(it->pose, 0., it->node,  0);
	node->reading=0;
	it->node=node;
				
      }
    }
    //			if (m_infoStream)
    m_infoStream  << "Tree: normalizing, resetting and propagating weights at the end..." ;
    updateTreeWeights(false);
    //			if (m_infoStream)
    m_infoStream  << ".done!" <<endl;
		
    delete [] plainReading;
    m_lastPartPose=m_odoPose; //update the past pose for the next iteration
    m_linearDistance=0;
    m_angularDistance=0;
    m_count++;
    processed=true;

  }
  if (m_outputStream.is_open())
    m_outputStream << flush;
  m_readingCount++;
  return processed;
}

OrientedPoint GridSlamProcessor::drawFromObservation(const OrientedPoint& pose, double lmax){
  double c1=gsl_vector_get(m_eval,0),
    c2=gsl_vector_get(m_eval,1),
    c3=gsl_vector_get(m_eval,2);
	       
  double nf=exp(1.5*lmax);
  c1=sqrt(fabs(c1))*nf, c2=sqrt(fabs(c2))*nf, c3=sqrt(fabs(c3))*nf;
	
  gsl_vector_set(m_pnoise,0, sampleGaussian(c1));
  gsl_vector_set(m_pnoise,1, sampleGaussian(c2));
  gsl_vector_set(m_pnoise,2, sampleGaussian(c3));
	

	
  gsl_blas_dgemv (CblasNoTrans, 1., m_evec, m_pnoise, 0, m_noise);
  OrientedPoint ret(gsl_vector_get(m_noise,0),gsl_vector_get(m_noise,1),gsl_vector_get(m_noise,2));
  ret.theta=atan2(sin(ret.theta), cos(ret.theta));
	
  return ret+pose;
}

void GridSlamProcessor::updateEigenSystem(const ScanMatcher:: CovarianceMatrix& cov){
  /*		if (m_infoStream)
		m_infoStream << " Covariance Stats:" << endl;
		if (m_infoStream)
		m_infoStream << cov.xx <<  " " << cov.xy << " " << cov.xt << endl;
		if (m_infoStream)
		m_infoStream << cov.xy <<  " " << cov.yy << " " << cov.yt << endl;
		if (m_infoStream)
		m_infoStream << cov.xt <<  " " << cov.yt << " " << cov.tt << endl << endl;*/
	
	
  gsl_matrix_set(m_cmat,0,0,cov.xx); 
  gsl_matrix_set(m_cmat,0,1,cov.xy); 
  gsl_matrix_set(m_cmat,0,2,cov.xt);
	
  gsl_matrix_set(m_cmat,1,0,cov.xy); 
  gsl_matrix_set(m_cmat,1,1,cov.yy); 
  gsl_matrix_set(m_cmat,1,2,cov.yt);
	
  gsl_matrix_set(m_cmat,2,0,cov.xt); 
  gsl_matrix_set(m_cmat,2,1,cov.yt); 
  gsl_matrix_set(m_cmat,2,2,cov.tt);

  gsl_eigen_symmv (m_cmat, m_eval,  m_evec, m_eigenspace);
	
  if (isnan(gsl_vector_get(m_eval,0))||isnan(gsl_vector_get(m_eval,1))||isnan(gsl_vector_get(m_eval,2))){
    if (m_infoStream)
      m_infoStream << "DEGENERATED MATRIX" << endl;
    if (m_infoStream)
      m_infoStream << gsl_matrix_get(m_evec,0,0) << " " << gsl_matrix_get(m_evec,0,1) << " "
		   << gsl_matrix_get(m_evec,0,2) << " " << std::endl;
    if (m_infoStream)
      m_infoStream << gsl_matrix_get(m_evec,1,0) << " "
		   << gsl_matrix_get(m_evec,1,1) << " "
		   << gsl_matrix_get(m_evec,1,2) << " " << std::endl;
    if (m_infoStream)
      m_infoStream << gsl_matrix_get(m_evec,2,0) << " "
		   << gsl_matrix_get(m_evec,2,1) << " "
		   << gsl_matrix_get(m_evec,2,2) << " " << std::endl;
    if (m_infoStream)
      m_infoStream << gsl_vector_get(m_eval,0) << " "
		   << gsl_vector_get(m_eval,1) << " "
		   << gsl_vector_get(m_eval,2) << " " << std::endl;
		
    gsl_matrix_set(m_evec,0,0,1); 
    gsl_matrix_set(m_evec,0,1,0); 
    gsl_matrix_set(m_evec,0,2,0);
		
    gsl_matrix_set(m_evec,1,0,0); 
    gsl_matrix_set(m_evec,1,1,1); 
    gsl_matrix_set(m_evec,1,2,0);
		
    gsl_matrix_set(m_evec,2,0,0); 
    gsl_matrix_set(m_evec,2,1,0); 
    gsl_matrix_set(m_evec,2,2,1);
		
    gsl_vector_set(m_eval,0,1e-10); 
    gsl_vector_set(m_eval,1,1e-10); 
    gsl_vector_set(m_eval,2,1e-10);
  }
		
}
		

std::ofstream& GridSlamProcessor::outputStream(){
  return m_outputStream;
}

std::ostream& GridSlamProcessor::infoStream(){
  return m_infoStream;
}


int GridSlamProcessor::getBestParticleIndex() const{
  unsigned int bi=0;
  double bw=-1e1000;
  for (unsigned int i=0; i<m_particles.size(); i++)
    if (bw<m_particles[i].weightSum){
      bw=m_particles[i].weightSum;
      bi=i;
    }
  return (int) bi;
}


void GridSlamProcessor::onOdometryUpdate(){
}

void GridSlamProcessor::onResampleUpdate(){
}

void GridSlamProcessor::onScanmatchUpdate(){
}



void GridSlamProcessor::scanMatch(const double* plainReading){
  // sample a new pose from each scan in the reference
  double sumScore=0;
  ScanMatcher::CovarianceMatrix cov;
  EigenCovariance3 odoCov=EigenCovariance3(m_odometryCovariance).rotate(-m_odoPose.theta);
  std::cerr << "Odometry statistics" << std::endl;
  std::cerr << odoCov.eval[0] << ":" << odoCov.evec[0][0] << " " << odoCov.evec[0][1] << " " << odoCov.evec[0][2] << std::endl;
  std::cerr << odoCov.eval[1] << ":" << odoCov.evec[1][0] << " " << odoCov.evec[1][1] << " " << odoCov.evec[1][2] << std::endl;
  std::cerr << odoCov.eval[2] << ":" << odoCov.evec[2][0] << " " << odoCov.evec[2][1] << " " << odoCov.evec[2][2] << std::endl;
  for (ParticleVector::iterator it=m_particles.begin(); it!=m_particles.end(); it++){
    OrientedPoint corrected;
    double score, l, s;
    Gaussian3 odoStat;
    odoStat.mean=it->pose;
    odoStat.covariance=odoCov.rotate(it->pose.theta);
    if(m_covScanmatch){
      score=m_matcher.optimize(corrected, cov, it->map, it->pose, plainReading);
      sumScore+=score;
      it->pose=corrected;
      if (score>m_minimumScore){
	updateEigenSystem(cov);
	it->pose=drawFromObservation(corrected);
      } else {
	if (m_infoStream)
	  m_infoStream << "Scan Matching Failed, using odometry. Likelihood=" << l <<std::endl;
	it->pose=m_motionModel.drawFromMotion(it->pose, m_odoPose, m_lastPartPose);
      }
      m_matcher.likelihoodAndScore(s, l, it->map, it->pose, plainReading);
    } else {
      score=1.;
      if (m_performScanmatching){
	score=m_matcher.optimize(corrected, it->map, it->pose, plainReading);
	it->pose=corrected;
      } else {	
	corrected=it->pose;
      }
      double lmax=0;
      if (m_considerOdometryCovariance)
	l=m_matcher.likelihood(lmax, it->pose, cov, it->map, corrected, odoStat, plainReading);
      else
	l=m_matcher.likelihood(lmax, it->pose, cov, it->map, corrected, plainReading);
      //cout  << "score " <<score << " log-likelihood " << l << endl;
      if (score>m_minimumScore){
	//cov.xx+=1e-5;
	//cov.yy+=1e-10;
	//cov.tt+=1e-20;
	//cov.xy+=1e-30;
	//cov.yt+=1e-30;
	//cov.xt+=1e-30;
				
	updateEigenSystem(cov);
	it->pose=drawFromObservation(it->pose);
	if(isnan(it->pose.x)||isnan(it->pose.y)||isnan(it->pose.theta)){
	  std::cerr << "degenerated matrix" << std::endl;
	  std::cerr << gsl_matrix_get(m_cmat,0,0) << " "
		    << gsl_matrix_get(m_cmat,0,1) << " "
		    << gsl_matrix_get(m_cmat,0,2) << " " << std::endl;
	  std::cerr << gsl_matrix_get(m_cmat,1,0) << " "
		    << gsl_matrix_get(m_cmat,1,1) << " "
		    << gsl_matrix_get(m_cmat,1,2) << " " << std::endl;
	  std::cerr << gsl_matrix_get(m_cmat,2,0) << " "
		    << gsl_matrix_get(m_cmat,2,1) << " "
		    << gsl_matrix_get(m_cmat,2,2) << " " << std::endl;
	  assert(false);
	}
	assert (!isnan(it->pose.x)&&!isnan(it->pose.y)&&!isnan(it->pose.theta));
      } else {
	if (m_infoStream){
	  m_infoStream << "Scan Matching Failed, using odometry. Likelihood=" << l <<std::endl;
	  m_infoStream << "lp:" << m_lastPartPose.x << " "  << m_lastPartPose.y << " "<< m_lastPartPose.theta <<std::endl;
	  m_infoStream << "op:" << m_odoPose.x << " " << m_odoPose.y << " "<< m_odoPose.theta <<std::endl;
	}
	it->pose=m_motionModel.drawFromMotion(it->pose, m_odoPose, m_lastPartPose);
      }
      m_odometryCovariance=Covariance3::zero;
    }
    sumScore+=score;		
    it->weight+=l;
    it->weightSum+=l;
    //set up the selective copy of the active area
    //by detaching the areas that will be updated
    m_matcher.invalidateActiveArea();
    m_matcher.computeActiveArea(it->map, it->pose, plainReading);
  }
  if (m_infoStream)
    m_infoStream << "Average Scan Matching Score=" << sumScore/m_particles.size() << std::endl;	
}

void GridSlamProcessor::normalize(){
  //normalize the log m_weights
  double gain=1./(m_obsSigmaGain*m_particles.size());
  double lmax=-1e1000;
  for (ParticleVector::iterator it=m_particles.begin(); it!=m_particles.end(); it++){
    lmax=it->weight>lmax?it->weight:lmax;
  }
  //cout << "!!!!!!!!!!! maxwaight= "<< lmax << endl;
	
  m_weights.clear();
  double wcum=0;
  m_neff=0;
  for (std::vector<Particle>::iterator it=m_particles.begin(); it!=m_particles.end(); it++){
    m_weights.push_back(exp(gain*(it->weight-lmax)));
    wcum+=m_weights.back();
    //cout << "l=" << it->weight<< endl;
  }
	
  m_neff=0;
  for (std::vector<double>::iterator it=m_weights.begin(); it!=m_weights.end(); it++){
    *it=*it/wcum;
    double w=*it;
    m_neff+=w*w;
  }
  m_neff=1./m_neff;
	
}

bool GridSlamProcessor::resample(const double* plainReading, int adaptSize, const RangeReading* rr){

  bool hasResampled = false;
  
  TNodeVector oldGeneration;
  for (unsigned int i=0; i<m_particles.size(); i++){
    oldGeneration.push_back(m_particles[i].node);
  }
	
  if (m_neff<m_resampleThreshold*m_particles.size()){		
		
    if (m_infoStream)
      m_infoStream  << "*************RESAMPLE***************" << std::endl;
		
    uniform_resampler<double, double> resampler;
    m_indexes=resampler.resampleIndexes(m_weights, adaptSize);
		
    if (m_outputStream.is_open()){
      m_outputStream << "RESAMPLE "<< m_indexes.size() << " ";
      for (std::vector<unsigned int>::const_iterator it=m_indexes.begin(); it!=m_indexes.end(); it++){
	m_outputStream << *it <<  " ";
      }
      m_outputStream << std::endl;
    }
		
    onResampleUpdate();
    //BEGIN: BUILDING TREE
    ParticleVector temp;
    unsigned int j=0;
    std::vector<unsigned int> deletedParticles;  		//this is for deleteing the particles which have been resampled away.
		
    //		cerr << "Existing Nodes:" ;
    for (unsigned int i=0; i<m_indexes.size(); i++){
      //			cerr << " " << m_indexes[i];
      while(j<m_indexes[i]){
	deletedParticles.push_back(j);
	j++;
      }
      if (j==m_indexes[i])
	j++;
      Particle & p=m_particles[m_indexes[i]];
      TNode* node=0;
      TNode* oldNode=oldGeneration[m_indexes[i]];
      //			cerr << i << "->" << m_indexes[i] << "B("<<oldNode->childs <<") ";
      node=new	TNode(p.pose, 0, oldNode, 0);
      node->reading=0;
      //			cerr << "A("<<node->parent->childs <<") " <<endl;
			
      temp.push_back(p);
      temp.back().node=node;
      temp.back().previousIndex=m_indexes[i];
    }
    while(j<m_indexes.size()){
      deletedParticles.push_back(j);
      j++;
    }
    //		cerr << endl;
    std::cerr <<  "Deleting Nodes:";
    for (unsigned int i=0; i<deletedParticles.size(); i++){
      std::cerr <<" " << deletedParticles[i];
      delete m_particles[deletedParticles[i]].node;
      m_particles[deletedParticles[i]].node=0;
    }
    std::cerr  << " Done" <<std::endl;

    //END: BUILDING TREE
    std::cerr << "Deleting old particles..." ;
    m_particles.clear();
    std::cerr << "Done" << std::endl;
    std::cerr << "Copying Particles and  Registering  scans...";
    for (ParticleVector::iterator it=temp.begin(); it!=temp.end(); it++){
      it->setWeight(0);
      m_matcher.invalidateActiveArea();
      m_matcher.registerScan(it->map, it->pose, plainReading);
      m_particles.push_back(*it);
    }
    std::cerr  << " Done" <<std::endl;
    hasResampled = true;
  } else {
    int index=0;
    std::cerr << "Registering Scans:";
    TNodeVector::iterator node_it=oldGeneration.begin();
    for (ParticleVector::iterator it=m_particles.begin(); it!=m_particles.end(); it++){
      //create a new node in the particle tree and add it to the old tree
      //BEGIN: BUILDING TREE  
      TNode* node=0;
      // porca madonna, this weight computation is bullshit!
      // node=new TNode(it->pose, it->weight, *node_it, 0);
      // better set it to 0!!!!
      node=new TNode(it->pose, 0.0, *node_it, 0);
		  
      node->reading=0;
      it->node=node;

      //END: BUILDING TREE
      m_matcher.invalidateActiveArea();
      m_matcher.registerScan(it->map, it->pose, plainReading);
      it->previousIndex=index;
      index++;
      node_it++;
		  
    }
    std::cerr  << "Done" <<std::endl;
	
  }
  //END: BUILDING TREE

  return hasResampled;
}
