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


#ifndef HARRAY2D_H
#define HARRAY2D_H
#include <set>
#include <utils/point.h>
#include <utils/autoptr.h>
#include "array2d.h"

template <class Cell>
class HierarchicalArray2D: public Array2D<autoptr< Array2D<Cell> > >{
	public:
		typedef std::set< point<int>, pointcomparator<int> > PointSet;
		HierarchicalArray2D(int xsize, int ysize, int patchMagnitude=5);
		HierarchicalArray2D(const HierarchicalArray2D& hg);
		HierarchicalArray2D& operator=(const HierarchicalArray2D& hg);
		virtual ~HierarchicalArray2D(){}
		void resize(int ixmin, int iymin, int ixmax, int iymax);
		inline int getPatchSize() const {return m_patchMagnitude;}
		inline int getPatchMagnitude() const {return m_patchMagnitude;}
		
		inline const Cell& cell(int x, int y) const;
		inline Cell& cell(int x, int y);
		inline bool isAllocated(int x, int y) const;
		inline AccessibilityState cellState(int x, int y) const ;
		inline IntPoint patchIndexes(int x, int y) const;
		
		inline const Cell& cell(const IntPoint& p) const { return cell(p.x,p.y); }
		inline Cell& cell(const IntPoint& p) { return cell(p.x,p.y); }
		inline bool isAllocated(const IntPoint& p) const { return isAllocated(p.x,p.y);}
		inline AccessibilityState cellState(const IntPoint& p) const { return cellState(p.x,p.y); }
		inline IntPoint patchIndexes(const IntPoint& p) const { return patchIndexes(p.x,p.y);}
		
		inline void setActiveArea(const PointSet&, bool patchCoords=false);
		const PointSet& getActiveArea() const {return m_activeArea; }
		inline void allocActiveArea();
	protected:
		virtual Array2D<Cell> * createPatch(const IntPoint& p) const;
		PointSet m_activeArea;
		int m_patchMagnitude;
		int m_patchSize;
};

template <class Cell>
HierarchicalArray2D<Cell>::HierarchicalArray2D(int xsize, int ysize, int patchMagnitude) 
  :Array2D<autoptr< Array2D<Cell> > >::Array2D((xsize>>patchMagnitude), (ysize>>patchMagnitude)){
	m_patchMagnitude=patchMagnitude;
	m_patchSize=1<<m_patchMagnitude;
}

template <class Cell>
HierarchicalArray2D<Cell>::HierarchicalArray2D(const HierarchicalArray2D& hg){
	m_xsize=hg.m_xsize;
	m_ysize=hg.m_ysize;
	m_cells=new autoptr< Array2D<Cell> >*[m_xsize];
	for (int x=0; x<m_xsize; x++){
		m_cells[x]=new autoptr< Array2D<Cell> >[m_ysize];
		for (int y=0; y<m_ysize; y++)
			m_cells[x][y]=hg.m_cells[x][y];
	}
	m_activeArea=hg.m_activeArea;
	m_patchMagnitude=hg.m_patchMagnitude;
	m_patchSize=hg.m_patchSize;
	for(PointSet::const_iterator it=m_activeArea.begin(); it!=m_activeArea.end(); it++){
		const autoptr< Array2D<Cell> >& ptr=hg.m_cells[it->x][it->y];
		Array2D<Cell>* patch=0;
		if(ptr)
			patch=new Array2D<Cell>(*ptr);
		m_cells[it->x][it->y]=autoptr< Array2D<Cell> >(patch);
	}
}

template <class Cell>
void HierarchicalArray2D<Cell>::resize(int xmin, int ymin, int xmax, int ymax){
	int xsize=xmax-xmin;
	int ysize=ymax-ymin;
	autoptr< Array2D<Cell> > ** newcells=new autoptr< Array2D<Cell> > *[xsize];
	for (int x=0; x<xsize; x++){
		newcells[x]=new autoptr< Array2D<Cell> >[ysize];
		for (int y=0; y<ysize; y++){
			newcells[x][y]=autoptr< Array2D<Cell> >(0);
		}
	}
	int dx= xmin < 0 ? 0 : xmin;
	int dy= ymin < 0 ? 0 : ymin;
	int Dx=xmax<m_xsize?xmax:m_xsize;
	int Dy=ymax<m_ysize?ymax:m_ysize;
	for (int x=dx; x<Dx; x++){
		for (int y=dy; y<Dy; y++){
			newcells[x-xmin][y-ymin]=m_cells[x][y];
		}
		delete [] m_cells[x];
	}
	delete [] m_cells;
	m_cells=newcells;
	m_xsize=xsize;
	m_ysize=ysize; 
}

template <class Cell>
HierarchicalArray2D<Cell>& HierarchicalArray2D<Cell>::operator=(const HierarchicalArray2D& hg){
//	Array2D<autoptr< Array2D<Cell> > >::operator=(hg);
	if (m_xsize!=hg.m_xsize || m_ysize!=hg.m_ysize){
		for (int i=0; i<m_xsize; i++)
			delete [] m_cells[i];
		delete [] m_cells;
		m_xsize=hg.m_xsize;
		m_ysize=hg.m_ysize;
		m_cells=new autoptr< Array2D<Cell> >*[m_xsize];
		for (int i=0; i<m_xsize; i++)
			m_cells[i]=new autoptr< Array2D<Cell> > [m_ysize];
	}
	for (int x=0; x<m_xsize; x++)
		for (int y=0; y<m_ysize; y++)
			m_cells[x][y]=hg.m_cells[x][y];
	
	m_activeArea=hg.m_activeArea;
	m_patchMagnitude=hg.m_patchMagnitude;
	m_patchSize=hg.m_patchSize;
	for(PointSet::const_iterator it=m_activeArea.begin(); it!=m_activeArea.end(); it++){
		const autoptr< Array2D<Cell> >& ptr=hg.m_cells[it->x][it->y];
		Array2D<Cell>* patch=0;
		if(ptr)
			patch=new Array2D<Cell>(*ptr);
		m_cells[it->x][it->y]=autoptr< Array2D<Cell> >(patch);
	}
	return *this;
}


template <class Cell>
void HierarchicalArray2D<Cell>::setActiveArea(const HierarchicalArray2D<Cell>::PointSet& aa, bool patchCoords){
	m_activeArea.clear();
	for (PointSet::const_iterator it= aa.begin(); it!=aa.end(); it++){
		IntPoint p;
		if (patchCoords)
			p=*it;
		else
			p=patchIndexes(*it);
		m_activeArea.insert(p);
		const autoptr< Array2D<Cell> >& ptr=m_cells[p.x][p.y];
		if(ptr){
			Array2D<Cell>* patch=new Array2D<Cell>(*ptr);
			m_cells[p.x][p.y]=autoptr< Array2D<Cell> >(patch);
		}	
	}
	//std::cout << __PRETTY_FUNCTION__ << m_activeArea.size() << std::endl;
}

template <class Cell>
Array2D<Cell>* HierarchicalArray2D<Cell>::createPatch(const IntPoint& p) const{
	return new Array2D<Cell>(1<<m_patchMagnitude, 1<<m_patchMagnitude);
}


template <class Cell>
AccessibilityState  HierarchicalArray2D<Cell>::cellState(int x, int y) const {
	if (isInside(patchIndexes(x,y)))
		if(isAllocated(x,y))
			return (AccessibilityState)((int)Inside|(int)Allocated);
		else
			return Inside;
	return Outside;
}

template <class Cell>
void HierarchicalArray2D<Cell>::allocActiveArea(){
	for (PointSet::const_iterator it= m_activeArea.begin(); it!=m_activeArea.end(); it++){
		const autoptr< Array2D<Cell> >& ptr=m_cells[it->x][it->y];
		Array2D<Cell>* patch=0;
		if (!ptr){
			patch=createPatch(*it);
			m_cells[it->x][it->y]=autoptr< Array2D<Cell> >(patch);
		}	
	}
}

template <class Cell>
bool HierarchicalArray2D<Cell>::isAllocated(int x, int y) const{
	IntPoint c=patchIndexes(x,y);
	autoptr< Array2D<Cell> >& ptr=m_cells[c.x][c.y];
	return (bool)ptr;
}

template <class Cell>
IntPoint HierarchicalArray2D<Cell>::patchIndexes(int x, int y) const{
	if (x>=0 && y>=0)
		return IntPoint(x>>m_patchMagnitude, y>>m_patchMagnitude);
	return IntPoint(-1, -1);
}

template <class Cell>
Cell& HierarchicalArray2D<Cell>::cell(int x, int y){
	IntPoint c=patchIndexes(x,y);
	assert(isInside(c.x, c.y));
	if (!m_cells[c.x][c.y]){
		Array2D<Cell>* patch=createPatch(IntPoint(x,y));
		m_cells[c.x][c.y]=autoptr< Array2D<Cell> >(patch);
		cerr << "!!! FATAL: your dick is going to fall down" << endl;
	}
	autoptr< Array2D<Cell> >& ptr=m_cells[c.x][c.y];
	return (*ptr).cell(IntPoint(x-(c.x<<m_patchMagnitude),y-(c.y<<m_patchMagnitude)));
}

template <class Cell>
const Cell& HierarchicalArray2D<Cell>::cell(int x, int y) const{
	assert(isAllocated(x,y));
	IntPoint c=patchIndexes(x,y);
	const autoptr< Array2D<Cell> >& ptr=m_cells[c.x][c.y];
	return (*ptr).cell(IntPoint(x-(c.x<<m_patchMagnitude),y-(c.y<<m_patchMagnitude)));
}

#endif

