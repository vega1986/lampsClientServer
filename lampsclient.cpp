#include "lampsclient.h"
#include "ui_lampsclient.h"
#include "nlohmann/json.hpp"

#include <string>
#include <sstream>

#include <QInputDialog>


LampsClient::LampsClient(QWidget *parent)
  : QMainWindow(parent)    ,
    m_data()               ,
    ui(new Ui::LampsClient)
{
  ui->setupUi(this);
  connect( ui->pushButtonConnect, SIGNAL( clicked() ), this, SLOT( SlotConnectClick() ) );

  m_socket = new QTcpSocket( this );
  connect( m_socket, SIGNAL( stateChanged(QAbstractSocket::SocketState) ),
           this    , SLOT  ( SlotSocketStateChanged(QAbstractSocket::SocketState) ) );
  connect( m_socket, SIGNAL( readyRead() ),
           this    , SLOT  ( SlotReadData() ) );

  connect( ui->pushButtonOn    , SIGNAL( clicked() ),
           this                , SLOT  ( SlotLampOn() ) );

  connect( ui->pushButtonOff   , SIGNAL( clicked() ),
           this                , SLOT  ( SlotLampOff() ) );

  connect( ui->pushButtonAdd   , SIGNAL( clicked() ),
           this                , SLOT  ( SlotLampAdd() ) );

  connect( ui->pushButtonRemove, SIGNAL( clicked() ),
           this                , SLOT  ( SlotLampRemove() ) );
}


LampsClient::~LampsClient()
{
  delete ui;
}


void LampsClient::SlotConnectClick()
{
  m_socket->disconnectFromHost();
  m_socket->connectToHost( ui->lineEditIP->text(), ui->spinBoxPort->value() );
}


void LampsClient::SlotSocketStateChanged( QAbstractSocket::SocketState state )
{
  if ( state == QAbstractSocket::SocketState::ConnectedState )
  {
    ui->labelStatus->setText( "V" );
    ui->labelStatus->setStyleSheet( "color: rgb(0, 255, 0);" );
  }
  else
  {
    ui->labelStatus->setText( "X" );
    ui->labelStatus->setStyleSheet( "color: rgb(255, 0, 0);" );
  }
}


void LampsClient::SlotReadData()
{
  QDataStream in( m_socket );
  in.setVersion(QDataStream::Qt_5_10);
  while ( true )
  {
    if ( m_bytes == 0 )
    {
      if ( m_socket->bytesAvailable() < sizeof( quint16 ) )
      {
        break;
      }
      in >> m_bytes;
    }
    if ( m_socket->bytesAvailable() < m_bytes )
    {
      break;
    }
    QString qstr;
    in >> qstr;
    {
      std::string str = qstr.toStdString();
      std::istringstream ist( str );
      using namespace nlohmann;
      json jsData = json::parse( ist );
      m_data.Load( jsData );
      UpdateComboBox();
    }
    m_bytes = 0;
  }
}


void LampsClient::SlotLampOn()
{
  if ( ui->lampsList->currentIndex() >= 0 )
  {
    const auto id = ui->lampsList->currentData().value<size_t>();
    m_data.TurnOn( id );
    SendToServer();
  }
}


void LampsClient::SlotLampOff()
{
  if ( ui->lampsList->currentIndex() >= 0 )
  {
    const auto id = ui->lampsList->currentData().value<size_t>();
    m_data.TurnOff( id );
    SendToServer();
  }
}


void LampsClient::SlotLampAdd()
{
  QInputDialog dialog;
  auto fnt = dialog.font();
  fnt.setPointSize( 14 );
  dialog.setFont( fnt );
  dialog.setLabelText( " lamp id: " );
  dialog.setInputMode( QInputDialog::InputMode::IntInput );
  dialog.setIntRange( 0, 1000 );
  const auto nl = ui->lampsList->count();
  const auto idDefault = ( nl == 0 ) ? size_t( 0 ) : ui->lampsList->itemData( nl - 1 ).value<size_t>() + 1;
  dialog.setIntValue( int( idDefault ) );
  if ( dialog.exec() )
  {
    const size_t id =  dialog.intValue();
    m_data.Add( id );
    UpdateComboBox();
    SendToServer();
  }
}


void LampsClient::SlotLampRemove()
{
  if ( ui->lampsList->currentIndex() >= 0 )
  {
    const auto id = ui->lampsList->currentData().value<size_t>();
    m_data.Remove( id );
    UpdateComboBox();
    SendToServer();
  }
}


void LampsClient::UpdateComboBox()
{
  ui->lampsList->clear();
  for (auto [id, st] : m_data.Get() )
  {
    ui->lampsList->addItem( QString("Лампочка ") + QString::number( id ), QVariant( size_t( id ) ) );
  }
}


void LampsClient::SendToServer()
{
  if ( m_socket->state() == QAbstractSocket::SocketState::ConnectedState )
  {
    QByteArray arrBlock;
    QDataStream out( &arrBlock, QIODevice::WriteOnly );
    out.setVersion( QDataStream::Qt_5_10 );
    const auto str = m_data.Dump().dump();
    QString qstr = QString::fromStdString( str );
    out << quint16( 0 ) << qstr;
    out.device()->seek( 0 );
    out << quint16( arrBlock.size() - sizeof(  quint16 ) );
    m_socket->write( arrBlock );
  }
}
