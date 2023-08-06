#ifndef LAMPSCLIENT_H
#define LAMPSCLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <vector>

#include "lampsmap.h"


QT_BEGIN_NAMESPACE
namespace Ui { class LampsClient; }
QT_END_NAMESPACE


///
/// \brief LampsClient
///        подключается к серверу, который хранит конфигурацию лампочек,
///        берет данные от сервера (который отправляет конфигурацию при новом подключении),
///        формирует список доступных лампочек и позволяет добавлять, удалять, включать, выключать
///        лампочки и передавать данные серверу.
///        Данные передаются в виде строки текста, описывающий корректный json объект.
///
class LampsClient : public QMainWindow
{

  Q_OBJECT

public:


  ///
  /// \brief Конструктор
  /// \param parent
  ///
  LampsClient(QWidget *parent = nullptr);


  ///
  /// \brief Деструктор
  ~LampsClient();

private slots:


  ///
  /// \brief Реакция клиента на нажатие кнопи подключения
  ///
  void SlotConnectClick();


  ///
  /// \brief Реакция клиента на изменения статуса сокета
  /// \param state - новое состояние сокета
  ///
  void SlotSocketStateChanged( QAbstractSocket::SocketState state );


  ///
  /// \brief Реакция клиента на появление новых байтов в очереди сокета
  ///
  void SlotReadData();


  ///
  /// \brief Реакция клиента на нажатие кнопки включения
  ///
  void SlotLampOn();


  ///
  /// \brief Реакция клиента на нажатие кнопки выключения
  ///
  void SlotLampOff();


  ///
  /// \brief Реакция клиента на нажатие кнопки добавления лампы
  ///
  void SlotLampAdd();


  ///
  /// \brief Реакция клиента на нажатие кнопки удаления
  ///
  void SlotLampRemove();

private:
  QTcpSocket          * m_socket = nullptr; ///< сокет, посредстом которого осуществляем обмен данными с сервером
  quint16               m_bytes  = 0;       ///< размер транша в байтах
  lightning::LampsMap   m_data;             ///< наши лампочки со статусом

  Ui::LampsClient     * ui;


  ///
  /// \brief Обновить список доступных лампочек
  ///
  void UpdateComboBox();


  ///
  /// \brief Послать обновленные данные на сервер
  ///
  void SendToServer();
};
#endif // LAMPSCLIENT_H
