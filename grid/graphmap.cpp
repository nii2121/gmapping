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


#ifndef GRAPHMAP_H
#define GRAPHMAP_H
#include <list>
#include <utils/point.h>
#include <utils/graph.h>
#include <grid/map.h>

class RasterMap;

struct GraphMapPatch{
	typedef typename std::list<IntPoint> PointList; 
	/**Renders the map relatively to the center of the patch*/
	//void render(RenderMap rmap);
	/**returns the lower left corner of the patch, relative to the center*/
	//Point minBoundary() const; 
	/**returns the upper right corner of the patch, relative to the center*/
	//Point maxBoundary() const; // 

	OrientedPoint center;
	PointList m_points;
};

struct Covariance3{
	double sxx, sxy, sxt, syy, syt ,stt;
};

struct GraphMapEdge{
	Covariance3 covariance;
	GraphMapPatch* first, *second;
	inline operator double() const{
		return sqrt((first->center-second->center)*(first->center-second->center));
	}
};


struct GraphPatchGraph: public Graph<GraphMapPatch, Covariance3>{
	void addEdge(Vertex* v1, Vertex* v2, const Covariance3& covariance);
};

void GraphPatchGraph::addEdge(GraphPatchGraph::Vertex* v1, GraphPatchGraph::VertexVertex* v2, 
	const Covariance3& cov){
	GraphMapEdge gme;
	gme.covariance=cov;
	gme.first=v1;
	gme.second=v2;
	return Graph<GraphMapPatch, Covariance3>::addEdge(v1,v2,gme);
}

struct GraphPatchDirectoryCell: public std::set<GraphMapPatch::Vertex*> {
	GraphPatchDirectoryCell(double);
};

typedef Map<GraphPatchDirectoryCell>, Array2D::set<GraphPatchDirectoryCell> >

#endif