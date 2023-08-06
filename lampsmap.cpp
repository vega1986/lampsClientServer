#include "lampsmap.h"

#include <stdexcept>
#include <string_view>
#include <vector>


//
lightning::LampsMap::LampsMap( const nlohmann::json& js )
{
  Load(js);
}


//
void lightning::LampsMap::Load( const nlohmann::json& js )
{
  using namespace nlohmann;
  const auto& lamps = js["lamps"];
  constexpr std::string_view err{ "bad input JSON" };

  m_status.clear();

  if ( not lamps.is_array() )
  {
    throw std::logic_error( err.data() );
  }

  for ( const auto & aLamp : lamps )
  {
    const auto & jID = aLamp["id"];

    if ( not jID.is_number_unsigned() )
    {
      throw std::logic_error( err.data() );
    }
    const auto id = jID.get<size_t>();

    const auto & jStatus = aLamp["status"];
    if (not jStatus.is_boolean())
    {
      throw std::logic_error(err.data());
    }
    const auto st = jStatus.get<bool>();
    m_status[id] = st;
  }
}


//
nlohmann::json lightning::LampsMap::Dump() const
{
  using namespace nlohmann;
  std::vector<json> lamps_array;
  for ( const auto [id, status] : m_status )
  {
    json item;
    item["id"]     = id;
    item["status"] = status;
    lamps_array.push_back( item );
  }
  json js;
  js["lamps"] = lamps_array;

  return js;
}


//
void lightning::LampsMap::TurnOn( const size_t j )
{
  if ( m_status.contains( j ) )
  {
    m_status[j] = true;
  }
}


//
void lightning::LampsMap::TurnOff( const size_t j )
{
  if ( m_status.contains( j ) )
  {
    m_status[j] = false;
  }
}


//
void lightning::LampsMap::Add( const size_t j )
{
  if ( not m_status.contains( j ) )
  {
    m_status[j] = false;
  }
}


//
void lightning::LampsMap::Remove( const size_t j )
{
  m_status.erase( j );
}
