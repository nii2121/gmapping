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


#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_blas.h>
#include <math.h>
#include "utils/stat.h"
#include <values.h>

double sampleGaussian(double sigma, unsigned long int S) {
	static gsl_rng * r = NULL;
	if(r==NULL) {
		gsl_rng_env_setup();
		r = gsl_rng_alloc (gsl_rng_default);
	}
	if (S!=0)
		gsl_rng_set(r, S);
	if (sigma==0)
		return 0;
	return gsl_ran_gaussian (r,sigma);
}

double evalGaussian(double sigmaSquare, double delta){
	if (sigmaSquare<=0)
		sigmaSquare=1e-4;
	return exp(-.5*delta*delta/sigmaSquare)/sqrt(2*M_PI*sigmaSquare);
}

double evalLogGaussian(double sigmaSquare, double delta){
	if (sigmaSquare<=0)
		sigmaSquare=1e-4;
	return -.5*delta*delta/sigmaSquare-.5*log(2*M_PI*sigmaSquare);
}


Covariance3 Covariance3::zero={0.,0.,0.,0.,0.,0.};

Covariance3 Covariance3::operator + (const Covariance3 & cov) const{
	Covariance3 r(*this);
	r.xx+=cov.xx;
	r.yy+=cov.yy;
	r.tt+=cov.tt;
	r.xy+=cov.xy;
	r.yt+=cov.yt;
	r.xt+=cov.xt;
	return r;
}

EigenCovariance3::EigenCovariance3(){}

EigenCovariance3::EigenCovariance3(const Covariance3& cov){
	static gsl_eigen_symmv_workspace * m_eigenspace=NULL;
	static gsl_matrix * m_cmat=NULL;
	static gsl_matrix * m_evec=NULL;
	static gsl_vector * m_eval=NULL;
	static gsl_vector * m_noise=NULL;
	static gsl_vector * m_pnoise=NULL;
	
	if (m_eigenspace==NULL){
		m_eigenspace=gsl_eigen_symmv_alloc(3);
		m_cmat=gsl_matrix_alloc(3,3);
		m_evec=gsl_matrix_alloc(3,3);
		m_eval=gsl_vector_alloc(3);
		m_noise=gsl_vector_alloc(3);
		m_pnoise=gsl_vector_alloc(3);
	}

	gsl_matrix_set(m_cmat,0,0,cov.xx); gsl_matrix_set(m_cmat,0,1,cov.xy); gsl_matrix_set(m_cmat,0,2,cov.xt);
	gsl_matrix_set(m_cmat,1,0,cov.xy); gsl_matrix_set(m_cmat,1,1,cov.yy); gsl_matrix_set(m_cmat,1,2,cov.yt);
	gsl_matrix_set(m_cmat,2,0,cov.xt); gsl_matrix_set(m_cmat,2,1,cov.yt); gsl_matrix_set(m_cmat,2,2,cov.tt);
	gsl_eigen_symmv (m_cmat, m_eval,  m_evec, m_eigenspace);
	for (int i=0; i<3; i++){
		eval[i]=gsl_vector_get(m_eval,i);
		for (int j=0; j<3; j++)
			evec[i][j]=gsl_matrix_get(m_evec,i,j);
	}
}

EigenCovariance3 EigenCovariance3::rotate(double angle) const{
	static gsl_matrix * m_rmat=NULL;
	static gsl_matrix * m_vmat=NULL;
	static gsl_matrix * m_result=NULL;
	if (m_rmat==NULL){
		m_rmat=gsl_matrix_alloc(3,3);
		m_vmat=gsl_matrix_alloc(3,3);
		m_result=gsl_matrix_alloc(3,3);
	}
	
	double c=cos(angle);
	double s=sin(angle);
	gsl_matrix_set(m_rmat,0,0, c ); gsl_matrix_set(m_rmat,0,1, -s); gsl_matrix_set(m_rmat,0,2, 0.);
	gsl_matrix_set(m_rmat,1,0, s ); gsl_matrix_set(m_rmat,1,1,  c); gsl_matrix_set(m_rmat,1,2, 0.);
	gsl_matrix_set(m_rmat,2,0, 0.); gsl_matrix_set(m_rmat,2,1, 0.); gsl_matrix_set(m_rmat,2,2, 1.);
	
	for (unsigned int i=0; i<3; i++)
		for (unsigned int j=0; j<3; j++)
			gsl_matrix_set(m_vmat,i,j,evec[i][j]);
	gsl_blas_dgemm (CblasNoTrans, CblasNoTrans, 1., m_rmat, m_vmat, 0., m_result);
	EigenCovariance3 ecov(*this);
	for (int i=0; i<3; i++){
		for (int j=0; j<3; j++)
			ecov.evec[i][j]=gsl_matrix_get(m_result,i,j);
	}
	return ecov;
}

OrientedPoint EigenCovariance3::sample() const{
	static gsl_matrix * m_evec=NULL;
	static gsl_vector * m_noise=NULL;
	static gsl_vector * m_pnoise=NULL;
	if (m_evec==NULL){
		m_evec=gsl_matrix_alloc(3,3);
		m_noise=gsl_vector_alloc(3);
		m_pnoise=gsl_vector_alloc(3);
	}
	for (int i=0; i<3; i++){
		for (int j=0; j<3; j++)
			gsl_matrix_set(m_evec,i,j, evec[i][j]);
	}
	for (int i=0; i<3; i++){
		double v=sampleGaussian(sqrt(eval[i]));
		if(isnan(v))
			v=0;
		gsl_vector_set(m_pnoise,i, v);
	}
	gsl_blas_dgemv (CblasNoTrans, 1., m_evec, m_pnoise, 0, m_noise);
	OrientedPoint ret(gsl_vector_get(m_noise,0),gsl_vector_get(m_noise,1),gsl_vector_get(m_noise,2));
	ret.theta=atan2(sin(ret.theta), cos(ret.theta));
	return ret;
}

double Gaussian3::eval(const OrientedPoint& p) const{
	OrientedPoint q=p-mean;
	q.theta=atan2(sin(p.theta-mean.theta),cos(p.theta-mean.theta));
	double v1,v2,v3;
	v1 = covariance.evec[0][0]*q.x+covariance.evec[1][0]*q.y+covariance.evec[2][0]*q.theta;
	v2 = covariance.evec[0][1]*q.x+covariance.evec[1][1]*q.y+covariance.evec[2][1]*q.theta;
	v3 = covariance.evec[0][2]*q.x+covariance.evec[1][2]*q.y+covariance.evec[2][2]*q.theta;
	return evalLogGaussian(covariance.eval[0], v1)+evalLogGaussian(covariance.eval[1], v2)+evalLogGaussian(covariance.eval[2], v3);
}
