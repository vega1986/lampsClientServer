#pragma once

#include <map>
#include "nlohmann/json.hpp"

namespace lightning
{

class LampsMap
{
private:


  std::map<size_t, bool> m_status;


public:


  LampsMap() = default;


  LampsMap( const nlohmann::json & js ); ///< конструктор


  void Load( const nlohmann::json & js ); ///< загрузка


  nlohmann::json Dump() const; ///< преобразовать в json


  void TurnOn( const size_t j ); ///< включить лампочку


  void TurnOff( const size_t j ); ///< выключить лампочку


  void Add( const size_t j ); ///< добавить лампочку в выключенном состоянии, если она уже есть, ничего не делать


  void Remove( const size_t j ); ///< удалить лампочку


  const std::map<size_t, bool>& Get(){ return m_status; }


};

}
