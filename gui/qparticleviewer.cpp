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
#include "qparticleviewer.h"
#include "moc_qparticleviewer.cpp"
#include <qimage.h>
#include <qpainter.h>
#include <math.h>
#include <stdio.h>
using namespace std;
//Constructor
QParticleViewer::QParticleViewer( QWidget * parent, const char * name , WFlags f, GridSlamProcessorThread* thread): QWidget(parent, name, f|WRepaintNoErase|WResizeNoErase)
{
  viewCenter=Point(0.,0.);
  setMinimumSize(500,500);
  mapscale=10;
  m_pixmap=new QPixmap(600,600);
  m_pixmap->fill(Qt::gray);
  gfs_thread=thread;
  tis=0;
  m_particleSize=0;
  m_refresh=false;
  bestMap=0;
  dragging=false;
  showPaths=0;
  showBestPath=1;
  count=0;
  writeToFile=0;

  showObjectPoints=1;
  showLandmark=0;
  fPoint = "temporary_DB.csv";
  abs_origin = new Point();
  abs_xaxis = new Point();
  abs_yaxis = new Point();
  connect(this,SIGNAL(absclickedPosition(int,int)),this,SLOT(getabsolute(int,int)));
  connect(this,SIGNAL(objclickedPosition(int,int)),this,SLOT(getObjectPoints(int,int)));
  connect(this,SIGNAL(ObjectPoint()),this,SLOT(drawObjectPoint()));


  tester1=0;  tester2=0;   tester3=0;  tester4=0;  tester5=0;


}
	  //Destructor
QParticleViewer::~QParticleViewer()
{
  if (m_pixmap)	
    delete m_pixmap;
}
void QParticleViewer::paintEvent ( QPaintEvent *paintevent )
{
  if (! m_pixmap)
    return;
  bitBlt(this,0,0,m_pixmap,0,0,m_pixmap->width(),m_pixmap->height(),CopyROP);
}
	  /*BitBlt(
	    HDC hdcDest, //コピー先デバイスコンテキストのハンドル
	    int nXDest,  //コピー先長方形の左上隅の x 座標
	    int nYDest,  //コピー先長方形の左上隅の y 座標
	    int nWidth,  //コピー先長方形の幅
	    int nHeight, //コピー先長方形の高さ
	    HDC hdcSrc,  //コピー元デバイスコンテキストのハンドル
	    int nXSrc,   //コピー元長方形の左上隅の x 座標
	    int nYSrc,   //コピー元長方形の左上隅の y 座標
	    DWORD dwRop  //ラスタオペレーションコード
	    );*/
void QParticleViewer::mousePressEvent ( QMouseEvent *event )
{
  cout << "-- MouseEvent --"<<endl;
  if (event->button()==LeftButton)
    {
      dragging=true;
      clickPos=event->pos();	
    }
  /*　クリックで座標指定	*/
  if (event->button()==RightButton)
    {
      if (absswitch)
	{
	  clickPos=event->pos();
	  emit absclickedPosition( clickPos.x(), clickPos.y() );
	}
      if(objinput)
	{
	  clickPos=event->pos();
	  emit objclickedPosition( clickPos.x(), clickPos.y() );
	}
    }
}
void QParticleViewer::mouseMoveEvent ( QMouseEvent *event )
{
  if (dragging)
    {
      cout <<"click:("<< clickPos.x() <<"," << clickPos.y() << ") --> " << "(" << event->x() <<","<<event->y()<<")"<<endl;
      QPoint delta=event->pos()-clickPos;
      clickPos=event->pos();
      cout <<"viewCenter:("<<viewCenter.x<<","<<viewCenter.y<<") --> ";
      viewCenter.x-=delta.x()/mapscale;
      viewCenter.y+=delta.y()/mapscale;
      cout <<"("<<viewCenter.x<<","<<viewCenter.y<<")"<<endl;
      update();
    }
}
void QParticleViewer::mouseReleaseEvent ( QMouseEvent *event )
{
  if (event->button()==LeftButton)
    {
      dragging=false;
    }
}
void QParticleViewer::keyPressEvent ( QKeyEvent *e )
{
  cerr << "\n-- Key Press Event --" << endl;
  switch (e->key())
    {
    case Qt::Key_B: showBestPath=!showBestPath; break;    ///qnamespace.h
    case Qt::Key_P: showPaths=!showPaths; break;
    case Qt::Key_O: showObjectPoints=!showObjectPoints ;break;
    case Qt::Key_L: showLandmark=!showLandmark ;break;

    case Qt::Key_Plus: mapscale *=1.25;cout <<mapscale <<endl;  break;
    case Qt::Key_Minus: mapscale/=1.25;cout <<mapscale <<endl;  break;
    case Qt::Key_C: viewCenter=bestParticlePose; break;
    case Qt::Key_W: WriteToTempDB(); break;
    case Qt::Key_R: ReadToTempDB(); break;
    case Qt::Key_A: absswitch=!absswitch;cout <<"### absswitch ###" << endl; break;
    case Qt::Key_Z: objinput=!objinput ;break;
    case Qt::Key_F: m_pixmap->save("test.bmp","BMP");break;

      //debug
    case Qt::Key_1: viewCenter=Point(0,0);  update();break;
    case Qt::Key_2: cout <<"arctan:"<<turnangle<<endl; break;

    case Qt::Key_3: cout << "viewCenter:("<<viewCenter.x<<","<<viewCenter.y<<")\n"
			 << "mapscale  :("<<mapscale <<")\n"
			 << "m_pixmap  :("<<m_pixmap->size().width()<<","<<m_pixmap->size().height()<<")\n"<<endl;break;
    case Qt::Key_4: viewCenter=*abs_origin; update();break;
    case Qt::Key_5:
      cout <<" #tester1:"<<tester1 <<"\n #tester2:"<<tester2<<"\n #tester3:"<<tester3<<"\n #tester4:"<<tester4<<"\n #tester5:"<<tester5<<endl;
      tester1=0;  tester2=0;   tester3=0;  tester4=0;  tester5=0; break;


    default:;
    }
}
		
void QParticleViewer::resizeEvent(QResizeEvent * sizeev)
{
  if (!m_pixmap)
    return;
  cerr << "QParticleViewer::resizeEvent" <<  sizeev->size().width()<< " " << sizeev->size().height() << endl;
  m_pixmap->resize(sizeev->size());
}
void QParticleViewer::drawParticleMove(const QParticleViewer::OrientedPointVector& oldPose, const QParticleViewer::OrientedPointVector& newPose)
{
  assert(oldPose.size()==newPose.size());
  QPainter painter(m_pixmap);
  painter.setPen(Qt::red);
  OrientedPointVector::const_iterator nit=newPose.begin();
  for(OrientedPointVector::const_iterator it=oldPose.begin(); it!=oldPose.end(); it++, nit++)
    {
      IntPoint p0=map2pic(*it);
      IntPoint p1=map2pic(*nit);
      painter.drawLine( (int)(p0.x), (int)(p0.y), (int)(p1.x), (int)(p1.y) );
    }
}
void QParticleViewer::drawFromFile()
{
  if(! tis)
    return;
  if (tis->atEnd())
    return;	
  QTextIStream& is=*tis;
	
  string line=is.readLine();
  istringstream lineStream(line);
  string recordType;
  lineStream >> recordType;
  if (recordType=="LASER_READING")
    {
      //do nothing with the laser
      cout << "l" << flush;
    }
  if (recordType=="ODO_UPDATE")
    {
      //just move the particles
      if (m_particleSize)
	m_refresh=true;
      m_oldPose=m_newPose;
      m_newPose.clear();
      unsigned int size;
      lineStream >> size;
      if (!m_particleSize)
	m_particleSize=size;
      assert(m_particleSize==size);
      for (unsigned int i=0; i< size; i++)
	{
	  OrientedPoint p;
	  double w;
	  lineStream >> p.x;
	  lineStream >> p.y;
	  lineStream >> p.theta;
	  lineStream >> w;
	  m_newPose.push_back(p);
	}
      cout << "o" << flush;
    }
  if (recordType=="SM_UPDATE")
    {
      if (m_particleSize)
	m_refresh=true;
      m_oldPose=m_newPose;
      m_newPose.clear();
      unsigned int size;
      lineStream >> size;
      if (!m_particleSize)
	m_particleSize=size;
      assert(m_particleSize==size);
      for (unsigned int i=0; i< size; i++)
	{
	  OrientedPoint p;
	  double w;
	  lineStream >> p.x;
	  lineStream >> p.y;
	  lineStream >> p.theta;
	  lineStream >> w;
	  m_newPose.push_back(p);
	}
      cout << "u" << flush;
    }
  if (recordType=="RESAMPLE")
    {
      unsigned int size;
      lineStream >> size;
      if (!m_particleSize)
	m_particleSize=size;
      assert(m_particleSize==size);
      OrientedPointVector temp(size);
      for (unsigned int i=0; i< size; i++)
	{
	  unsigned int ind;
	  lineStream >> ind;
	  temp[i]=m_newPose[ind];
	}
      m_newPose=temp;
      cout << "r" << flush;
    }
  if (m_refresh)
    {
      drawParticleMove(m_oldPose, m_newPose);
      m_refresh=false;
    }
}
void QParticleViewer::drawMap(const ScanMatcherMap& map)
{
  //cout << "Map received" << map.getMapSizeX() << " " << map.getMapSizeY() << endl;
  QPainter painter(m_pixmap);
  painter.setPen(Qt::black);
  m_pixmap->fill(QColor(255,255,255));
  unsigned int count=0;
	
  Point wmin=Point(pic2map(IntPoint(-m_pixmap->width()/2,m_pixmap->height()/2)));
  Point wmax=Point(pic2map(IntPoint(m_pixmap->width()/2,-m_pixmap->height()/2)));
  IntPoint imin=map.world2map(wmin);
  IntPoint imax=map.world2map(wmax);
  /*	cout << __PRETTY_FUNCTION__ << endl;
    cout << " viewCenter=" << viewCenter.x << "," << viewCenter.y <<   endl;	
    cout << " wmin=" << wmin.x << "," << wmin.y <<  " wmax=" << wmax.x << "," << wmax.y << endl;	
    cout << " imin=" << imin.x << "," << imin.y <<  " imax=" << imax.x << "," << imax.y << endl;
    cout << " mapSize=" << map.getMapSizeX() << "," << map.getMapSizeY() << endl;*/
  for(int x=0; x<m_pixmap->width(); x++)
    for(int y=0; y<m_pixmap->height(); y++)
      {
	//IntPoint ip=IntPoint(x,y)+imin;
	//Point p=map.map2world(ip);
	Point p=pic2map(IntPoint(x-m_pixmap->width()/2,
				 y-m_pixmap->height()/2));
	//if (map.storage().isInside(map.world2map(p)))
	{
	  double v=map.cell(p);
	  if (v>=0)
	    {
	      int grayValue=255-(int)(255.*v);
	      painter.setPen(QColor(grayValue, grayValue, grayValue));
	      painter.drawPoint(x,y);
	      count++;
	    }
	}
      }    
}
void QParticleViewer::drawFromMemory()
{
  if (! gfs_thread)
    return;
  m_pixmap->fill(Qt::white);
  GridSlamProcessorThread::EventDeque events=gfs_thread->getEvents();
  for (GridSlamProcessorThread::EventDeque::const_iterator it=events.begin(); it!=events.end();it++)
    {
      GridSlamProcessorThread::MapEvent* mapEvent= dynamic_cast<GridSlamProcessorThread::MapEvent*>(*it);
      if (mapEvent)
	{
	  //cout << "Map: bestIdx=" << mapEvent->index <<endl;
	  if (bestMap)
	    delete bestMap;
	  else 
	    {
				
	    }
	  bestMap=mapEvent->pmap;
	  mapEvent->pmap=0;
	  bestParticlePose=mapEvent->pose;
	  delete mapEvent;
	}else
	{
	  GridSlamProcessorThread::DoneEvent* doneEvent= dynamic_cast<GridSlamProcessorThread::DoneEvent*>(*it);
	  if (doneEvent)
	    {
	      gfs_thread->stop();
	      delete doneEvent;
	    } else
	    history.push_back(*it);
	}	
			
    }
  if (bestMap)
    drawMap(*bestMap);
	
  unsigned int particleSize=0;
  std::vector<OrientedPoint> oldPose, newPose;
  vector<unsigned int> indexes;
	
  GridSlamProcessorThread::EventDeque::reverse_iterator it=history.rbegin();
  while (!particleSize && it!=history.rend())
    {
      GridSlamProcessorThread::ParticleMoveEvent* move= dynamic_cast<GridSlamProcessorThread::ParticleMoveEvent*>(*it);
      GridSlamProcessorThread::ResampleEvent* resample= dynamic_cast<GridSlamProcessorThread::ResampleEvent*>(*it);
      if (move)
	particleSize=move->hypotheses.size();
      if (resample)
	particleSize=resample->indexes.size();
      it++;
    }
	
  //check for the best index
  double wmax=-1e2000;
  unsigned int bestIdx=0;
  bool emitted=false;
  for (unsigned int i=0; i<particleSize; i++)
    {
      unsigned int currentIndex=i;
      bool done=false;
      for(GridSlamProcessorThread::EventDeque::reverse_iterator it=history.rbegin(); it!=history.rend()&& !done; it++)
	{
	  GridSlamProcessorThread::ParticleMoveEvent* move= dynamic_cast<GridSlamProcessorThread::ParticleMoveEvent*>(*it);
	  if (move && move->scanmatched)
	    {
	      double cw=move->weightSums[currentIndex];
	      if (cw>wmax)
		{
		  wmax=cw;
		  bestIdx=currentIndex;
		} 
	      done=true;
	      if (! emitted)
		{
		  emit neffChanged(move->neff/particleSize);
		  emitted=true;
		}
	    }
	  GridSlamProcessorThread::ResampleEvent* resample= dynamic_cast<GridSlamProcessorThread::ResampleEvent*>(*it);
	  if (resample)
	    {
	      currentIndex=resample->indexes[currentIndex];
	    }
	}
    }
  //cout << "bestIdx=" << bestIdx << endl;
  QPainter painter(m_pixmap);
  emit ObjectPoint();
  if (showObjectPoints)    
    for ( int i = 0; i < (int)object.size(); ++i )
      {
	painter.setPen(Qt::magenta);
	painter.drawEllipse(object[i].draw_x,object[i].draw_y,5,5);
      }
  if(showLandmark)
    {
      painter.setPen(Qt::green);
      draworigenx = (int)(  ( abs_origin->x - viewCenter.x ) * mapscale + ( m_pixmap->size().width() ) /2);
      draworigeny = (int)( -( abs_origin->y - viewCenter.y ) * mapscale + ( m_pixmap->size().height() ) /2);
      drawyaxisx  = (int)(  ( abs_yaxis->x  - viewCenter.x ) * mapscale + ( m_pixmap->size().width() ) /2);
      drawyaxisy  = (int)( -( abs_yaxis->y  - viewCenter.y ) * mapscale + ( m_pixmap->size().height() ) /2);
      painter.drawLine ( draworigenx,draworigeny, drawyaxisx, drawyaxisy ) ;
      /*      
	      for ( int i = 0; i+1 < (int)object.size(); ++i )
	      {
	      painter.drawLine (object[i].draw_x,object[i].draw_y,object[i+1].draw_x,object[i+1].draw_y);
	      }
      */
    }
  for (unsigned int i=0; i<particleSize+1; i++)
    {
      painter.setPen(Qt::yellow);
      unsigned int currentIndex=i;
      if (i==particleSize && showBestPath)
	{
	  currentIndex=bestIdx;
	  painter.setPen(Qt::red);
	}
      bool first=true;
      OrientedPoint pnew;
      for(GridSlamProcessorThread::EventDeque::reverse_iterator it=history.rbegin(); it!=history.rend(); it++)
	{
	  GridSlamProcessorThread::ParticleMoveEvent* move= dynamic_cast<GridSlamProcessorThread::ParticleMoveEvent*>(*it);
	  if (move)
	    {
	      OrientedPoint pold=move->hypotheses[currentIndex];
	      IntPoint p0=map2pic(pold)+IntPoint(m_pixmap->width()/2,m_pixmap->height()/2);
	      IntPoint p1=map2pic(pnew)+IntPoint(m_pixmap->width()/2,m_pixmap->height()/2);
	      if (first)
		{
		  painter.drawPoint(p0.x, p0.y);
		}
	      else 
		{
		  painter.drawLine(p0.x, p0.y, p1.x, p1.y);
		}
	      first=false;
	      if (!((showPaths&&i<particleSize ) || (showBestPath&&i==particleSize) ) )
		break;
	      pnew=pold;
	    }
	  GridSlamProcessorThread::ResampleEvent* resample= dynamic_cast<GridSlamProcessorThread::ResampleEvent*>(*it);
	  if (resample && ! first)
	    {
	      currentIndex=resample->indexes[currentIndex];
	    }
	}
    }
  if (writeToFile && bestMap)
    {
      if (! (count%writeToFile) )
	{
	  char name[100];
	  sprintf(name,"dump-%05d.png", count/writeToFile);
	  cout << " Writing " << name <<" ..." << flush;
	  QImage image=m_pixmap->convertToImage();
	  bool rv=image.save(name,"PNG");
	  if (rv)
	    cout << " Done";
	  else
	    cout << " ERROR";
	  cout << endl;
	}
      count++;
    }
}
  void QParticleViewer::timerEvent(QTimerEvent * te) 
  {
    if (te->timerId()==timer) 
      {
	if ( tis)
	  drawFromFile();
	else
	  {
	    drawFromMemory();
	    update();
	  }
      }
  }
  void QParticleViewer::start(int period)
  {
    timer=startTimer(period);
  }
  void QParticleViewer::refreshParameters()
  {
    //scanmatcher
    matchingParameters.maxrange=gfs_thread->getlaserMaxRange();
    matchingParameters.urange=gfs_thread->getusableRange();
    matchingParameters.ssigma=gfs_thread->getgaussianSigma();
    //   matchingParameters.sreg=gfs_thread->getregScore();
    //   matchingParameters.scrit=gfs_thread->getcritScore();
    matchingParameters.ksize=gfs_thread->getkernelSize();
    matchingParameters.lstep=gfs_thread->getoptLinearDelta();
    matchingParameters.astep=gfs_thread->getoptAngularDelta();
    matchingParameters.iterations=gfs_thread->getoptRecursiveIterations();
    //start
    startParameters.srr=gfs_thread->getsrr();
    startParameters.stt=gfs_thread->getstt();
    startParameters.str=gfs_thread->getstr();
    startParameters.srt=gfs_thread->getsrt();
	
    startParameters.xmin=gfs_thread->getxmin();
    startParameters.ymin=gfs_thread->getymin();
    startParameters.xmax=gfs_thread->getxmax();
    startParameters.ymax=gfs_thread->getymax();
    startParameters.delta=gfs_thread->getdelta();
	
    startParameters.particles=gfs_thread->getParticles().size();
    startParameters.resampleThreshold=gfs_thread->getresampleThreshold();
    startParameters.drawFromObservation=gfs_thread->getcovScanmatch();
    startParameters.outFileName=0;
  }
  void QParticleViewer::start()
  {
    gfs_thread->setMatchingParameters(
				      matchingParameters.urange, 
				      matchingParameters.maxrange, 
				      matchingParameters.ssigma, 
				      matchingParameters.ksize, 
				      matchingParameters.lstep, 
				      matchingParameters.astep, 
				      matchingParameters.iterations, 
				      startParameters.lsigma,
				      startParameters.lgain,
				      startParameters.lskip);
    gfs_thread->setMotionModelParameters(
					 startParameters.srr,
					 startParameters.srt,
					 startParameters.srt,
					 startParameters.stt);
    gfs_thread->setUpdateDistances(
				   startParameters.linearUpdate,
				   startParameters.angularUpdate,
				   startParameters.resampleThreshold
				   );
    ((GridSlamProcessor*)(gfs_thread))->init(
					     startParameters.particles,
					     startParameters.xmin, 
					     startParameters.ymin, 
					     startParameters.xmax, 
					     startParameters.ymax, 
					     startParameters.delta, 
					     startParameters.initialPose,
					     startParameters.drawFromObservation);
    gfs_thread->start();
  }
  void QParticleViewer::setMatchingParameters(const QParticleViewer::MatchingParameters& mp)
  {
    matchingParameters=mp;
  }
  void QParticleViewer::setStartParameters(const QParticleViewer::StartParameters& sp)
  {
    startParameters=sp;
  }
  void QParticleViewer::stop()
  {
    gfs_thread->stop();
  }
  void QParticleViewer::loadFile(const char * fn)
  {
    gfs_thread->loadFiles(fn);
    /*	
      startParameters.initialPose=
      gfs_thread->boundingBox(
      startParameters.xmin, 
      startParameters.ymin, 
      startParameters.xmax,
      startParameters.ymax);
    */	
  }
  void  QParticleViewer::received( int x, int y )
  {
    debug( "New value selected: (%d, %d)\n", x, y );
  }
void QParticleViewer::getabsolute(int x,int y)
{
  if(!abs_yaxis->x && !abs_yaxis->y)
    { 
      *abs_yaxis=Point(viewCenter.x + (x-((m_pixmap->size().width())/2))/mapscale,
		       viewCenter.y - (y-((m_pixmap->size().height())/2))/mapscale);
      cout << "y_axis:(" <<abs_yaxis->x<<","<<abs_yaxis->y<<")"<< endl;
    }
  else if(!abs_origin->x && !abs_origin->y)
    {
      *abs_origin=Point(viewCenter.x + (x-((m_pixmap->size().width())/2))/mapscale,
			viewCenter.y - (y-((m_pixmap->size().height())/2))/mapscale);
      cout << "abs_origin:(" <<abs_origin->x<<","<<abs_origin->y <<")"<<endl;
    }
  else if(!abs_xaxis->x && !abs_xaxis->y)
    {
      *abs_xaxis=Point(viewCenter.x + (x-((m_pixmap->size().width())/2))/mapscale,
		       viewCenter.y - (y-((m_pixmap->size().height())/2))/mapscale);
      cout << "x_axis:(" <<abs_xaxis->x<<","<<abs_xaxis->y<<")"<<endl;
      cout << "#### got 3 points ####" <<endl;
      cout << "abs_y_axis:(" <<abs_yaxis->x <<","<<abs_yaxis->y <<")\n"
	   << "abs_origin:(" <<abs_origin->x<<","<<abs_origin->y<<")\n"
	   << "abs_x_axis:(" <<abs_xaxis->x <<","<<abs_xaxis->y <<")"<< endl;

      turnangle=atan2(abs_yaxis->y - abs_origin->y,abs_yaxis->x - abs_origin->x);
      showLandmark=1;
      //スイッチをOFF
      absswitch=0;
      cout <<"### absswitch OFF ###" << endl;
      ReadToTempDB();
    } 
  else
    {
      //ランドマークのリセット
      *abs_origin=Point(0,0);
      *abs_xaxis=Point(0,0);
      *abs_yaxis=Point(0,0);
      showLandmark=0;
      cout << "### reset absolute point ###" << endl;
      //スイッチをOFF
      absswitch=0;
      cout <<"### absswitch OFF ###" << endl;
    }
  /*
    else 
    {
    cout <<"clickPos:("<<x<<","<<y<<")"<<endl;
    *abs_object=Point(viewCenter.x + (x-((m_pixmap->size().width())/2))/mapscale,
    viewCenter.y - (y-((m_pixmap->size().height())/2))/mapscale);
    cout << "abs_object:(" <<abs_object->x<<","<<abs_object->y <<")"<<endl;
    }  
  */
}

  //構造体へ値を入力
void QParticleViewer::getObjectPoints(int x,int y)
{
  ObjectPoints temp;
  sprintf(temp.name,"Object%d",object.size()+1);
  temp.x = static_cast<float>(viewCenter.x + (x-((m_pixmap->size().width() )/2))/mapscale);
  temp.y = static_cast<float>(viewCenter.y - (y-((m_pixmap->size().height())/2))/mapscale);
  object.push_back(temp);
 
  ///////////////////////////
  //構造体の内容を表示
  for ( int i = 0; i < (int)object.size(); ++i )
    {
      cout <<object[i].name<<" :("<< object[i].x << ',' << object[i].y <<")"<< endl;
    }
  ////////////////////////////
}


void QParticleViewer::drawObjectPoint()
{
  for ( int i = 0; i < (int)object.size(); ++i )
    {
      object[i].draw_x=(int)(  ( object[i].x - viewCenter.x ) * mapscale + ( m_pixmap->size().width() ) /2);
      object[i].draw_y=(int)( -( object[i].y - viewCenter.y ) * mapscale + ( m_pixmap->size().height()) /2);
      //   cout <<object[i].name<<" :("<< object[i].draw_x << ',' << object[i].draw_y <<")"<< endl;
    }
}



  /* follows are under constraction*/
  /*
   */
void QParticleViewer::WriteToTempDB()
{
  fp = fopen( fPoint, "w" );
  if( fp == NULL )      
    {cout << fPoint <<" ファイルが開けません" <<endl; return;}

  for ( int i = 0; i < (int)object.size(); ++i )
    {
      f1=cos(-turnangle)*(object[i].x - abs_origin->x) -sin(-turnangle)*(object[i].y - abs_origin->y);
      f2=sin(-turnangle)*(object[i].x - abs_origin->x) +cos(-turnangle)*(object[i].y - abs_origin->y);
      fprintf( fp, "%s,%f,%f", object[i].name, f1,f2 );
    }
  cout << fPoint << "ファイル書き込みが終わりました\n" << endl;
  fclose( fp );
}

void QParticleViewer::ReadToTempDB()
{
  object.clear();
  cout << "ファイルを読み込みます" << endl;
  fp = fopen(fPoint, "r" );
  if( fp == NULL )
    { cout << fPoint << " ファイルが開けません" << endl;return;}

  while( ( EOFchecker = fscanf( fp, "%[^,],%f,%f", str, &f1, &f2 ) ) != EOF )
    {
      ObjectPoints temp;
      sprintf(temp.name,"%s",str);
      temp.x=cos(turnangle)*(f1) -sin(turnangle)*(f2)+abs_origin->x;
      temp.y=sin(turnangle)*(f1) +cos(turnangle)*(f2)+abs_origin->y;
      object.push_back(temp);
    }
  cout << fPoint << "ファイル読み込みが終わりました\n" << endl;
  fclose( fp );

  for ( int i = 0; i < (int)object.size(); ++i )
    {
      cout <<object[i].name<<" :("<< object[i].x << ',' << object[i].y <<")"<< endl;
    }
}

