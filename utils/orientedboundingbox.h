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


#ifndef  ORIENTENDBOUNDINGBOX_H
#define  ORIENTENDBOUNDINGBOX_H


#include <stdio.h>
#include <math.h>

#include <utils/point.h>

template<class NUMERIC>
class OrientedBoundingBox {

 public:
  OrientedBoundingBox(std::vector< point<NUMERIC> > p);  
  double area();

 protected:
  Point ul;
  Point ur;
  Point ll;
  Point lr;  
};

#include "orientedboundingbox.hxx"

#endif

