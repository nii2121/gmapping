#include "qkcoordsel.h"
#include <qapplication.h>

class Receiver : public QObject
{
  Q_OBJECT
public slots:
  void received( int, int );
};

#include "qkcoordseltest.moc"

void Receiver::received( int x, int y )
{
  debug( "New value selected: (%d, %d)\n", x, y );
}


int main( int argc, char* argv[] )
{
  QApplication a( argc, argv );

  QKCoordSel* selector = new QKCoordSel();
  selector->resize( 200, 200 );
  selector->setXMinValue( 20 );
  selector->setXMaxValue( 130 );
  selector->setYMinValue( 50 );
  selector->setYMaxValue( 150 );
  selector->show();

  Receiver* receiver = new Receiver();
  QObject::connect( selector, SIGNAL( valueChanged( int, int ) ),
                    receiver, SLOT( received( int, int ) ) );

  a.setMainWidget( selector );
  return a.exec();
}
