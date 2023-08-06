#include "lampsserver.h"

#include <QApplication>
#include <QFile>
#include <QMessageBox>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  quint16 port = 0;
  for ( int j = 1; j < argc - 1; ++j )
  {
    QString opt = argv[j];
    if ( opt == "--port" )
    {
      QString qsPort = argv[j + 1];
      port = qsPort.toUShort();
      break;
    }
  }
//  {
//    QFile file("lamps_list.js");
//    if ( !( file.exists() ) )
//    {
//      QMessageBox messBox;
//      messBox.setText( " File lamps_list.js does not exist " );
//      auto fnt = messBox.font();
//      fnt.setPointSize( 14 );
//      messBox.setFont( fnt );
//      messBox.exec();
//      return 0;
//    }
//  }
  LampsServer w( "lamps_list.js", port );
  w.show();
  return a.exec();
}
