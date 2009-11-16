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


#ifndef _POINT_H_
#define _POINT_H_
#include <assert.h>
#include <math.h>
#include <iostream>

template <class T>
struct point{
	inline point(){}
	inline point(T _x, T _y):x(_x),y(_y){}
	T x, y;
};

template <class T>
inline point<T> operator+(const point<T>& p1, const point<T>& p2){
	return point<T>(p1.x+p2.x, p1.y+p2.y);
}

template <class T>
inline point<T> operator - (const point<T> & p1, const point<T> & p2){
	return point<T>(p1.x-p2.x, p1.y-p2.y);
}

template <class T>
inline point<T> operator * (const point<T>& p, const T& v){
	return point<T>(p.x*v, p.y*v);
}

template <class T>
inline point<T> operator * (const T& v, const point<T>& p){
	return point<T>(p.x*v, p.y*v);
}

template <class T>
inline T operator * (const point<T>& p1, const point<T>& p2){
	return p1.x*p2.x+p1.y*p2.y;
}


template <class T, class A>
struct orientedpoint: public point<T>{
	inline orientedpoint(){}
	inline orientedpoint(const point<T>& p);
	inline orientedpoint(T x, T y, A _theta): point<T>(x,y), theta(_theta){}
	inline orientedpoint<T,A> rotate(A alpha){
		T s=sin(alpha), c=cos(alpha);
		A a=alpha+theta;
		a=atan2(sin(a),cos(a));
		return orientedpoint(
			c*this->x-s*this->y,
			s*this->x+c*this->y, 
			a);
	}
	A theta;
};

template <class T, class A>
orientedpoint<T,A>::orientedpoint(const point<T>& p){
	this->x=p.x;
	this->y=p.y;
	this->theta=0.;
}


template <class T, class A>
orientedpoint<T,A> operator+(const orientedpoint<T,A>& p1, const orientedpoint<T,A>& p2){
	return orientedpoint<T,A>(p1.x+p2.x, p1.y+p2.y, p1.theta+p2.theta);
}

template <class T, class A>
orientedpoint<T,A> operator - (const orientedpoint<T,A> & p1, const orientedpoint<T,A> & p2){
	return orientedpoint<T,A>(p1.x-p2.x, p1.y-p2.y, p1.theta-p2.theta);
}

template <class T, class A>
orientedpoint<T,A> operator * (const orientedpoint<T,A>& p, const T& v){
	return orientedpoint<T,A>(p.x*v, p.y*v, p.theta*v);
}

template <class T, class A>
orientedpoint<T,A> operator * (const T& v, const orientedpoint<T,A>& p){
	return orientedpoint<T,A>(p.x*v, p.y*v, p.theta*v);
}

template <class T, class A>
orientedpoint<T,A> absoluteDifference(const orientedpoint<T,A>& p1,const orientedpoint<T,A>& p2){
	orientedpoint<T,A> delta=p1-p2;
	delta.theta=atan2(sin(delta.theta), cos(delta.theta));
	double s=sin(p2.theta), c=cos(p2.theta);
	return orientedpoint<T,A>(c*delta.x+s*delta.y, 
	                         -s*delta.x+c*delta.y, delta.theta);
}

template <class T, class A>
orientedpoint<T,A> absoluteSum(const orientedpoint<T,A>& p1,const orientedpoint<T,A>& p2){
	double s=sin(p1.theta), c=cos(p1.theta);
	return orientedpoint<T,A>(c*p2.x-s*p2.y,
				  s*p2.x+c*p2.y, p2.theta) + p1;
}

template <class T>
struct pointcomparator{
	bool operator ()(const point<T>& a, const point<T>& b) const {
		return a.x<b.x || (a.x==b.x && a.y<b.y);
	}	
};

template <class T>
inline point<T> max(const point<T>& p1, const point<T>& p2){
	point<T> p=p1;
	p.x=p.x>p2.x?p.x:p2.x;
	p.y=p.y>p2.y?p.y:p2.y;
	return p;
}
template <class T>
inline point<T> min(const point<T>& p1, const point<T>& p2){
	point<T> p=p1;
	p.x=p.x<p2.x?p.x:p2.x;
	p.y=p.y<p2.y?p.y:p2.y;
	return p;
}

typedef point<int> IntPoint;
typedef point<double> Point;
typedef orientedpoint<double, double> OrientedPoint;

#endif
