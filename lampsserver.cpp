#include "lampsserver.h"
#include "ui_lampsserver.h"
#include "nlohmann/json.hpp"

#include <QMessageBox>
#include <QTcpSocket>

#include <fstream>
#include <sstream>


LampsServer::LampsServer( const QString & file, const quint16 port, QWidget *parent )
  :
    QMainWindow( parent ),
    ui(new Ui::LampsServer),
    m_file( file ),
    m_port( port )
{
  ui->setupUi(this);

  // Set m_data from file
  using namespace nlohmann;
  std::ifstream ist( m_file.toStdString() );
  if ( ist.good() )
  {
    json jsData = json::parse( ist );
    m_data.Load( jsData );
    UpdateListWidgetLamps();
  }

  // Set tcp server for listen port
  m_server = new QTcpServer( this );
  const auto lis = m_server->listen(QHostAddress::Any, m_port );
  if ( lis )
  {
    connect( m_server, SIGNAL( newConnection() ),
             this    , SLOT  ( SlotNewConnection() ) );
  }
}


LampsServer::~LampsServer()
{
  const auto js = m_data.Dump();
  const auto str = js.dump(2);
  std::ofstream ost( m_file.toStdString() );
  ost << str;
  ost.close();
  delete ui;
}


void LampsServer::SlotNewConnection()
{
  QTcpSocket* socket = m_server->nextPendingConnection();

  connect( socket, SIGNAL( disconnected() ),
           socket, SLOT  ( deleteLater() ) );

  connect( socket, SIGNAL( stateChanged(QAbstractSocket::SocketState) ),
           this  , SLOT  ( SlotSocketStateChanged(QAbstractSocket::SocketState) ) );

  connect( socket, SIGNAL( readyRead() ),
           this  , SLOT  ( SlotReadClient() ) );

  SendDataToClient( socket );
  // увеличиваем количество подключенных клиентов
  ++m_clients;
  ui->labelStatus->setText( "Connected" );
  ui->labelStatus->setStyleSheet( "color: rgb(0, 255, 0);" );
}


void LampsServer::SlotReadClient()
{
  QTcpSocket * socket = qobject_cast<QTcpSocket*>( sender() );
  QDataStream in( socket );
  in.setVersion(QDataStream::Qt_5_10);
  while ( true )
  {
    if ( m_bytes == 0 )
    {
      if ( socket->bytesAvailable() < sizeof( quint16 ) )
      {
        break;
      }
      in >> m_bytes;
    }
    if ( socket->bytesAvailable() < m_bytes )
    {
      break;
    }
    QString qstr;
    in >> qstr;
    {
      using namespace nlohmann;
      std::istringstream ist( qstr.toStdString() );
      json jsData = json::parse( ist );
      m_data.Load( jsData );
      UpdateListWidgetLamps();
    }
    m_bytes = 0;
  }
}


void LampsServer::UpdateListWidgetLamps()
{
  ui->listWidgetLamps->clear();
  for ( const auto [id, status] : m_data.Get() )
  {
    QString caption = QString("Лампочка ") + QString::number( id );
    QListWidgetItem * lwi = new QListWidgetItem( caption, ui->listWidgetLamps );
    lwi->setBackground( ( status ) ? Qt::green : Qt::red );
    ui->listWidgetLamps->addItem( lwi );
  }
}


void LampsServer::SendDataToClient( QTcpSocket * socket )
{
  const auto str = m_data.Dump().dump();
  QString qstr = QString::fromStdString( str );

  QByteArray arrBlock;
  QDataStream out( &arrBlock, QIODevice::WriteOnly );
  out.setVersion( QDataStream::Qt_5_10 );

  out << quint16(0) << qstr;
  out.device()->seek(0);
  out << quint16( arrBlock.size() - sizeof( quint16 ) );
  socket->write( arrBlock );
}


void LampsServer::SlotSocketStateChanged( QAbstractSocket::SocketState state )
{
  if ( state == QAbstractSocket::UnconnectedState )
  {
    --m_clients;
    if ( m_clients == 0 )
    {
      ui->labelStatus->setText( "Disconnected" );
      ui->labelStatus->setStyleSheet( "color: rgb(255, 0, 0);" );
    }
  }
}
