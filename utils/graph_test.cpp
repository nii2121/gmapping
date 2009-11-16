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
#include "graph.h"

class DoubleGraph: public Graph<double, double>{
	public:
		virtual double edgeWeight(Edge*e) const {return e->info;}
};

int main(int argc, char **  argv){
	DoubleGraph g1;
	DoubleGraph g3;
	
	double d1=1, d2=2, d3=3, d4=4;
	DoubleGraph::Vertex* v1=g1.addVertex(d1);
	DoubleGraph::Vertex* v2=g1.addVertex(d2);
	DoubleGraph::Vertex* v3=g1.addVertex(d3);
	DoubleGraph::Vertex* v4=g1.addVertex(d4);
	g1.print(std::cout);
	g1.removeVertex(v4);
	
	double ve1=2000;
	//	double ve2=3000, ve3=6000;	
	DoubleGraph::Edge* e1= g1.addEdge(v1,v2,ve1);
// 	DoubleGraph::Edge* e2= g1.addEdge(v1,v3,ve2);
// 	DoubleGraph::Edge* e3= g1.addEdge(v2,v3,ve3);
	g1.print(std::cout);

	std::cout << "getRacheableNodes()" << std::endl;	
 	DoubleGraph::VertexList l=g1.getReacheableNodes(v2, 4000);
	std::cout << "getRacheableNodes() done" << std::endl;
	for (DoubleGraph::VertexListCIt it=l.begin(); it!=l.end(); it++)
		std::cout << *it << std::endl;
	
	DoubleGraph g2(g1);	
	g2.print(std::cout);

	g3=g1;
	
	g1.removeVertex(v3);
	g1.print(std::cout);
	std::cout << "@RemoveVertex ok" << std::endl;
	
	g1.removeEdge(e1);
	std::cout << "RemoveEdge ok" << std::endl;
	g1.print(std::cout);
}



