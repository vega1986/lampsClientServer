#ifndef LAMPSSERVER_H
#define LAMPSSERVER_H

#include <QMainWindow>
#include <QTcpServer>

#include "lampsmap.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LampsServer; }
QT_END_NAMESPACE


///
/// \brief LampsServer
///        загружает данные о лампочках из локального файла.
///        Добавление / удаление лампочек осуществляется клиентами.
///        Так же клиенты включают и выключают лампочки.
///        Обновлённая конфигурация записывается в локальный json файл при закрытии
///        приложения.
///
class LampsServer : public QMainWindow
{
  Q_OBJECT

public:


  ///
  /// \brief Конструктор
  /// \param file - входной файл
  /// \param port - порт
  /// \param parent - родительский виджет
  ///
  LampsServer( const QString & file, const quint16 port, QWidget *parent = nullptr );


  ///
  /// \brief Деструктор
  ///
  ~LampsServer();

private slots:


  ///
  /// \brief Реакция нашего сервера на новое соединение
  ///
  void SlotNewConnection();


  ///
  /// \brief Реакция сервера на появление новыйх байтов в очереди сокета
  ///
  void SlotReadClient();


  ///
  /// \brief Реакция сервера на изменение состояния сокета одного из клиентов
  /// \param state - новое состояние сокета клиента
  ///
  void SlotSocketStateChanged( QAbstractSocket::SocketState state );

private:
        Ui::LampsServer     * ui;

  const QString               m_file;              ///< конфигурация лампочек
  const quint16               m_port    = 0;       ///< прослушиваемый порт
        QTcpServer          * m_server  = nullptr; ///< tcp сервер для добавления новых подключений
        quint16               m_bytes   = 0;       ///< размер транша в байтах
        lightning::LampsMap   m_data;              ///< наши лампочки со статусом
        quint16               m_clients = 0;       ///< всего клиентов сейчас подключено

  ///
  /// \brief Обновляем и разукрашиваем список лампочек в соответствии с обновленными данными
  ///
  void UpdateListWidgetLamps();


  ///
  /// \brief Посылаем конфигурацию лампочек через сокет
  /// \param socket - сокет, через который пересылаем данные
  ///
  void SendDataToClient( QTcpSocket* socket );


};
#endif // LAMPSSERVER_H
