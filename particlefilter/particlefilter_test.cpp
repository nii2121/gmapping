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


#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "particlefilter.h"

using namespace std;

#define test(s) {cout << s <<  " " << flush;}
#define testOk() {cout << "OK" << endl;}

struct Particle{
	double p;
	double w;
	inline operator double() const {return w;}
	inline void setWeight(double _w) {w=_w;}
};

ostream& printParticles(ostream& os, const vector<Particle>& p)
{
	for (vector<Particle>::const_iterator it=p.begin(); it!=p.end(); ++it) {
		os << it->p<< " " << (double)*it << endl;
	}
	return os;
}

struct EvolutionModel{
	Particle evolve(const Particle& p){
		Particle pn(p);
		pn.p+=.5*(drand48()-.5);
		return pn;
	}
};

struct QualificationModel{
	Particle evolve(const Particle& p){
		return p;
	}
};

struct LikelyhoodModel{
	double likelyhood(const Particle& p) const{
		double v = 1./(0.1+10*(p.p-2)*(p.p-2))+1./(0.1+10*(p.p-8)*(p.p-8));
		return v;
	}
};

int main (unsigned int argc, const char * const * argv){
	vector<Particle> particles(100);
	LikelyhoodModel likelyhoodModel;
	uniform_resampler<Particle, double> resampler;
	auxiliary_evolver <Particle, double, QualificationModel, EvolutionModel, LikelyhoodModel> auxevolver;
	evolver <Particle, EvolutionModel> evolver;

	for (vector<Particle>::iterator it=particles.begin(); it!=particles.end(); it++){
		it->w=1;
		it->p=10*(drand48());
	}

	vector<Particle> sirparticles(particles);
	vector<Particle> auxparticles(particles);

	/*sir step*/
	while (1){
		char buf[2];
		cin.getline(buf,2);
		vector<Particle> newgeneration;

		cout << "# SIR step" << endl;
		evolver.evolve(sirparticles);
		for (vector<Particle>::iterator it=sirparticles.begin(); it!=sirparticles.end(); it++){
			it->setWeight(likelyhoodModel.likelyhood(*it));
		}
		ofstream os("sir.dat");
		printParticles(os, sirparticles);
		os.close();
		newgeneration=resampler.resample(sirparticles);
		sirparticles=newgeneration;

		cout << "# AUX step" << endl;
		auxevolver.evolve(auxparticles);
		for (vector<Particle>::iterator it=auxparticles.begin(); it!=auxparticles.end(); it++){
			it->setWeight(likelyhoodModel.likelyhood(*it));
		}
		os.open("aux.dat");
		printParticles(os, auxparticles);
		os.close();
		newgeneration=resampler.resample(auxparticles);
		auxparticles=newgeneration;
		cout << "plot [0:10][0:1]\"sir.dat\" w p" << endl;
		cout << "replot \"aux.dat\" w p" << endl;
	}
}

