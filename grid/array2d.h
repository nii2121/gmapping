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


#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <assert.h>
#include <utils/point.h>
#include "accessstate.h"

template<class Cell> class Array2D{
	public:
		Array2D(int xsize=0, int ysize=0);
		Array2D& operator=(const Array2D &);
		Array2D(const Array2D<Cell> &);
		~Array2D();
		void resize(int xmin, int ymin, int xmax, int ymax);
		
		inline bool isInside(int x, int y) const;
		inline const Cell& cell(int x, int y) const;
		inline Cell& cell(int x, int y);
		inline AccessibilityState cellState(int x, int y) const { return (AccessibilityState) (isInside(x,y)?(Inside|Allocated):Outside);}
		
		inline bool isInside(const IntPoint& p) const { return isInside(p.x, p.y);}
		inline const Cell& cell(const IntPoint& p) const {return cell(p.x,p.y);}
		inline Cell& cell(const IntPoint& p) {return cell(p.x,p.y);}
		inline AccessibilityState cellState(const IntPoint& p) const { return cellState(p.x, p.y);}
		
		inline int getPatchSize() const{return 0;}
		inline int getPatchMagnitude() const{return 0;}
		inline int getXSize() const {return m_xsize;}
		inline int getYSize() const {return m_ysize;}
		inline Cell** cells() {return m_cells;}
		Cell ** m_cells;
	protected:
		int m_xsize, m_ysize;
};


template<class Cell> 
Array2D<Cell>::Array2D(int xsize, int ysize){
//	assert(xsize>0);
//	assert(ysize>0);
	m_xsize=xsize;
	m_ysize=ysize;
	if (m_xsize>0 && m_ysize>0){
		m_cells=new Cell*[m_xsize];
		for (int i=0; i<m_xsize; i++)
			m_cells[i]=new Cell[m_ysize];
	}
	else{
		m_xsize=m_ysize=0;
		m_cells=0;
	}
}

template <class Cell> 
Array2D<Cell> & Array2D<Cell>::operator=(const Array2D<Cell> & g){
	if (m_xsize!=g.m_xsize || m_ysize!=g.m_ysize){
		for (int i=0; i<m_xsize; i++)
			delete [] m_cells[i];
		delete [] m_cells;
		m_xsize=g.m_xsize;
		m_ysize=g.m_ysize;
		m_cells=new Cell*[m_xsize];
		for (int i=0; i<m_xsize; i++)
			m_cells[i]=new Cell[m_ysize];
	}
	for (int x=0; x<m_xsize; x++)
		for (int y=0; y<m_ysize; y++)
			m_cells[x][y]=g.m_cells[x][y];
	
	return *this;
}

template  <class Cell> 
Array2D<Cell>::Array2D(const Array2D<Cell> & g){
	m_xsize=g.m_xsize;
	m_ysize=g.m_ysize;
	m_cells=new Cell*[m_xsize];
	for (int x=0; x<m_xsize; x++){
		m_cells[x]=new Cell[m_ysize];
		for (int y=0; y<m_ysize; y++)
			m_cells[x][y]=g.m_cells[x][y];
	}
}

template  <class Cell>
Array2D<Cell>::~Array2D(){
  for (int i=0; i<m_xsize; i++){
    delete [] m_cells[i];
    m_cells[i]=0;
  }
  delete [] m_cells;
}

template <class Cell>
void Array2D<Cell>::resize(int xmin, int ymin, int xmax, int ymax){
	int xsize=xmax-xmin;
	int ysize=ymax-ymin;
	Cell ** newcells=new Cell *[xsize];
	for (int x=0; x<xsize; x++){
		newcells[x]=new Cell[ysize];
	}
	int dx= xmin < 0 ? 0 : xmin;
	int dy= ymin < 0 ? 0 : ymin;
	int Dx=xmax<this->m_xsize?xmax:this->m_xsize;
	int Dy=ymax<this->m_ysize?ymax:this->m_ysize;
	for (int x=dx; x<Dx; x++){
		for (int y=dy; y<Dy; y++){
			newcells[x-xmin][y-ymin]=this->m_cells[x][y];
		}
		delete [] this->m_cells[x];
	}
	delete [] this->m_cells;
	this->m_cells=newcells;
	this->m_xsize=xsize;
	this->m_ysize=ysize; 
}

template  <class Cell>
inline bool Array2D<Cell>::isInside(int x, int y) const{
	return x>=0 && y>=0 && x<m_xsize && y<m_ysize; 
}

template  <class Cell>
inline const Cell& Array2D<Cell>::cell(int x, int y) const{
	assert(isInside(x,y));
	return m_cells[x][y];
}


template  <class Cell>
inline Cell& Array2D<Cell>::cell(int x, int y){
	assert(isInside(x,y));
	return m_cells[x][y];
}

#endif

