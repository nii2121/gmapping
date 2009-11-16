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


#ifndef GRAPH_H
#define GRAPH_H

#include <ostream>
#include <map>
#include <list>
#include <set>
#include <assert.h>
#include <values.h>


template <typename VertexInfo, typename EdgeInfo>
class Graph{
	public:
		struct Vertex;
		struct Edge;
		typedef typename std::set<Vertex*> VertexSet;
		typedef typename std::set<Vertex*>::iterator VertexSetIt;
		typedef typename std::set<Vertex*>::const_iterator VertexSetCIt;

		typedef typename std::list<Vertex*> VertexList;
		typedef typename std::list<Vertex*>::iterator VertexListIt;
		typedef typename std::list<Vertex*>::iterator VertexListCIt;
		
		
		typedef typename std::set<Edge*> EdgeSet;
		typedef typename std::set<Edge*>::iterator EdgeSetIt;
		typedef typename std::set<Edge*>::const_iterator EdgeSetCIt;
		
		typedef typename std::list<Edge*> EdgeList;
		typedef typename std::list<Edge*>::iterator EdgeListIt;
		typedef typename std::list<Edge*>::const_iterator EdgeListCIt;
		
		typedef typename std::map<const Vertex*, Vertex*> VertexMap;
		typedef typename std::map<const Vertex*, Vertex*>::iterator VertexMapIt;
		typedef typename std::map<const Edge*, Edge*> EdgeMap;
		typedef typename std::map<const Edge*, Edge*>::iterator EdgeMapIt;
		
		struct Vertex{
			Vertex(VertexInfo& vi):info(vi){ distance=-1; }
			VertexInfo info;
			EdgeList edges;
			double distance;
		};
		
		struct Edge{
			Edge(EdgeInfo& ei):info(ei){}
			EdgeInfo info;
			Vertex* first;
			Vertex* second;
		};

		Graph();
		
		Graph(const Graph& g);
		Graph& operator=(const Graph&);
		virtual ~Graph();
		Vertex* addVertex(VertexInfo & vi);
		bool removeVertex(Vertex* v);
		Edge* addEdge(Vertex * first, Vertex* second, EdgeInfo& ei);
		bool removeEdge(Edge* e);
		std::ostream& print(std::ostream& os) const;
		
		VertexList getReacheableNodes(Vertex* v, double distance);
		
		inline VertexSet& vertexes() {return m_vertexes;}
		inline EdgeSet& edges() {return m_edges;}
		inline const VertexSet& vertexes() const {return m_vertexes;}
		inline const EdgeSet& edges() const {return m_edges;}
		
		
		virtual double edgeWeight(Edge*e) const=0;
	protected:
		VertexSet m_vertexes;
		EdgeSet m_edges;
};


template <typename VertexInfo, typename EdgeInfo>
Graph<VertexInfo, EdgeInfo>::Graph(){
}

template <typename VertexInfo, typename EdgeInfo>
Graph<VertexInfo, EdgeInfo>::Graph(const Graph<VertexInfo, EdgeInfo>& g){
	//construct the copy maps;
	VertexMap vmap;
	for (VertexSetCIt it=g.m_vertexes.begin(); it!=g.m_vertexes.end(); it++){
		Vertex* v=new Vertex((**it));
		vmap.insert(std::make_pair(*it, v));
		m_vertexes.insert(v);
	}
	
	//construct the edge map, and resolve the edge-referenced vertexes;
	EdgeMap emap;
	for (EdgeSetCIt it=g.m_edges.begin(); it!=g.m_edges.end(); it++){
		Edge* e=new Edge(*(*it));
		emap.insert(std::make_pair(*it, e));
		m_edges.insert(e);
		
		VertexMapIt vfirst=vmap.find(e->first);
		assert(vfirst!=vmap.end());
		VertexMapIt vsecond=vmap.find(e->second);
		assert(vsecond!=vmap.end());
		e->first=vfirst->second;
		e->second=vsecond->second;
	}

	//resolve the vertex-referenced edges;
	for (VertexSetIt it=m_vertexes.begin(); it!=m_vertexes.end(); it++){
		for (EdgeListIt edge=(*it)->edges.begin(); edge!=(*it)->edges.end(); edge++){
			EdgeMapIt edge_it=emap.find(*edge);
			assert(edge_it!=emap.end());
			*edge=edge_it->second;
		}
	}
}
template <typename VertexInfo, typename EdgeInfo>
Graph<VertexInfo, EdgeInfo>::~Graph(){
	for (EdgeSetIt it=m_edges.begin(); it!=m_edges.end(); it++)
		delete *it;
	for (VertexSetIt it=m_vertexes.begin(); it!=m_vertexes.end(); it++)
		delete *it;
}


template <class VertexInfo, class EdgeInfo>
Graph<VertexInfo, EdgeInfo>& Graph<VertexInfo, EdgeInfo>::operator=(const Graph& g){
	
	for (EdgeSetIt it=m_edges.begin(); it!=m_edges.end(); it++)
		delete *it;
	for (VertexSetIt it=m_vertexes.begin(); it!=m_vertexes.end(); it++)
		delete *it;

	//construct the copy maps;
	VertexMap vmap;
	for (VertexSetCIt it=g.m_vertexes.begin(); it!=g.m_vertexes.end(); it++){
		Vertex* v=new Vertex((**it));
		vmap.insert(std::make_pair(*it, v));
		m_vertexes.insert(v);
	}
	
	//construct the edge map, and resolve the edge-referenced vertexes;
	EdgeMap emap;
	for (EdgeSetCIt it=g.m_edges.begin(); it!=g.m_edges.end(); it++){
		Edge* e=new Edge(*(*it));
		emap.insert(std::make_pair(*it, e));
		m_edges.insert(e);
		
		VertexMapIt vfirst=vmap.find(e->first);
		assert(vfirst!=vmap.end());
		VertexMapIt vsecond=vmap.find(e->second);
		assert(vsecond!=vmap.end());
		e->first=vfirst->second;
		e->second=vsecond->second;
	}

	//resolve the vertex-referenced edges;
	for (VertexSetIt it=m_vertexes.begin(); it!=m_vertexes.end(); it++){
		for (EdgeListIt edge=(*it)->edges.begin(); edge!=(*it)->edges.end(); edge++){
			EdgeMapIt edge_it=emap.find(*edge);
			assert(edge_it!=emap.end());
			*edge=edge_it->second;
		}
	}
	
	return *this;
}

template <typename VertexInfo, typename EdgeInfo>
typename Graph<VertexInfo ,EdgeInfo>::Vertex* Graph<VertexInfo, EdgeInfo>::addVertex(VertexInfo & vi){
	Vertex* v=new Vertex(vi);
	m_vertexes.insert(v);
	return v;
}

template <class VertexInfo, class EdgeInfo>
bool Graph<VertexInfo, EdgeInfo>::removeVertex(Graph<VertexInfo, EdgeInfo>::Vertex* v){
	VertexSetIt it=m_vertexes.find(v);
	if (it==m_vertexes.end())
		return false;
	m_vertexes.erase(it);
	EdgeList& l=v->edges;
	for (EdgeListIt it=l.begin(); it!= l.end(); it++){
		Edge* e=*it;
		EdgeSetIt eit=m_edges.find(e);
		assert(eit!=m_edges.end());
		m_edges.erase(eit);
		Vertex* v2=e->first==v?e->second:e->first;
		if (v2!=v){
			EdgeList& q=v2->edges;
			typename std::list<EdgeListIt> r;
			for (EdgeListIt it=q.begin(); it!= q.end(); it++){
				if((*it)->first==v || (*it)->second==v)
					r.push_back(it);
			}
			for (typename std::list<EdgeListIt>::iterator it=r.begin(); it!=r.end(); it++)
				q.erase(*it);
		}
		delete e;
	}
	delete v;
	return true;
}


template <class VertexInfo, class EdgeInfo>
typename Graph<VertexInfo, EdgeInfo>::Edge* Graph<VertexInfo, EdgeInfo>::addEdge(
	Graph<VertexInfo, EdgeInfo>::Vertex * first, 
	Graph<VertexInfo, EdgeInfo>::Vertex* second, EdgeInfo& ei){
	
	Edge* e=new Edge(ei);
	e->first=first;
	e->second=second;
	
	m_edges.insert(e);
	
	first->edges.push_back(e);
	if (first!=second)
		second->edges.push_back(e);
	return e;	
}

template <class VertexInfo, class EdgeInfo>
bool Graph<VertexInfo, EdgeInfo>::removeEdge(Graph<VertexInfo, EdgeInfo>::Edge* e){
	using namespace std;
	//remove the edge from the set
	EdgeSetIt eit=m_edges.find(e);
	if(eit==m_edges.end())
		return false;
	m_edges.erase(eit);
		
	//remove from the list of the first node;
	EdgeListIt it=e->first->edges.begin();
	while(it!=e->first->edges.end() && *it!=e)
		it++;
	assert(*it==e);
// 	std::cout << "Removing " << e->first << " "<<*it << std::endl;
	e->first->edges.erase(it);
	
	//remove from the list of the second node;
	if (e->first!=e->second){
		it=e->second->edges.begin();
		while(it!=e->second->edges.end() && *it!=e)
			it++;
		assert(*it==e);
// 		std::cout << "Removing " << e->second << " " <<*it << std::endl;
		e->second->edges.erase(it);
	}
	delete e;
	return true;
}

template <class VertexInfo, class EdgeInfo>
typename Graph<VertexInfo, EdgeInfo>::VertexList 
Graph<VertexInfo, EdgeInfo>::getReacheableNodes(Graph<VertexInfo, EdgeInfo>::Vertex* v, double distance){
	VertexList vl;
	VertexList border;
	vl.push_back(v);
	border.push_back(v);
	v->distance=0;
	//unsigned int count=0;
	while (!border.empty()){
		VertexListIt imin=border.end();
		double dmin=MAXDOUBLE;
		VertexListIt it;
		for (it=border.begin(); it!=border.end(); ++it){
			if ((*it)->distance<dmin){
				dmin=(*it)->distance;
				imin=it;
			}
		}
				
		Vertex* current=*imin;
		border.erase(imin);
		double current_distance=current->distance;
		for (EdgeListCIt it=current->edges.begin(); it!=current->edges.end(); it++){
			assert((*it)->first!=(*it)->second);
			Vertex* other=((*it)->first==current)?(*it)->second:(*it)->first;
			Edge* e=*it;
			double nd=edgeWeight(e);
			assert(nd>0.);
			//if (!((++count)%100)) std::cerr << "." << std::flush;
			double tempDist=current_distance+nd;
			if ( tempDist<distance ){
				if ( other->distance<0. ){  //other has never been touched
					vl.push_back(other);
					other->distance=tempDist;
					border.push_back(other);
// 					std::cout << "insert "<< current_distance+nd 
// 					          <<" " << other<<  std::endl;
				}
				
				if (tempDist<other->distance){
					other->distance=tempDist;
				}
			} 
//			else std::cout << "fail" << other << std::endl;
		}
	}
	for (VertexListIt it=vl.begin(); it!=vl.end(); it++){
// 		std::cout << *it <<" " <<(*it)->distance << std::endl;
		(*it)->distance=-1;

	}
	return vl;
}

template <class VertexInfo, class EdgeInfo>
std::ostream& Graph<VertexInfo, EdgeInfo>::print(std::ostream& os) const {
	os << "***VERTEXES***" << std::endl;
	for (VertexSetCIt it=m_vertexes.begin(); it!=m_vertexes.end(); it++){
		os << "Node=" << *it << " Info=" <<(*it)->info << std::endl;
	}
	
	os << "***EDGES***" << std::endl;
	for (EdgeSetCIt it=m_edges.begin(); it!=m_edges.end(); it++){
		os << "Edge=" << *it << " Info=" <<(*it)->info << std::endl;
	}
	
	os << "***LINKS***" << std::endl;
	for (VertexSetCIt it=m_vertexes.begin(); it!=m_vertexes.end(); it++){
		os << "Node=" << *it << " Info=" <<(*it)->info << std::endl;
		for (EdgeListCIt edge=(*it)->edges.begin(); edge!=(*it)->edges.end(); edge++){
			os << "\t Edge=" << *edge << " Info=" << (*edge)->info << std::endl;
			os << "\t\t First=" << (*edge)->first << " Second=" << (*edge)->second << std::endl;
		}
	}
	return os;
}

#endif



