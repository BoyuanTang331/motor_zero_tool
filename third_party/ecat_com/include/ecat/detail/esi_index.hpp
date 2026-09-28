#pragma once

#include "EtherCATInfo.hxx"
#include "esi_index.hxx"
#include <ecat/types.hpp>
#include <map>
#include <string>
#include <vector>
#include <optional>

namespace ecat::detail
{

class esi_index
{
public:
  void load();
  std::vector<std::string> loadDevicesIndex(std::vector<std::vector<std::string>> slaveInfo);//用于load devices.index文件
  void save(bool flag = true) const;
  void rebuild();
  void rebuild(std::vector <std::string> esiPath);//配合devices.index使用，加快解析速度

  std::shared_ptr<ecat::detail::esi::Device1>
  find_device(ecat::slave_id id,
              std::optional<uint32_t> revision_no = std::nullopt,
              std::optional<uint32_t> serial_no = std::nullopt) const
  {
    return std::get<0>(find_device_and_file(id, revision_no, serial_no));
  }

  std::tuple<
    std::shared_ptr<ecat::detail::esi::Device1>,
    std::string>
  find_device_and_file(ecat::slave_id,
                       std::optional<uint32_t> revision_no = std::nullopt,
                       std::optional<uint32_t> serial_no = std::nullopt) const;

  std::shared_ptr<ecat::detail::esi::EtherCATInfo> get_parsed_file(const std::string &file) const
  {
    try
    {
      return parsed_files_.at(file);
    }
    catch (std::out_of_range &)
    {
      return nullptr;
    }
  }

private:
  std::unique_ptr<ecat::detail::generated::esi_index> index_;
  mutable std::map<std::string,
                   std::shared_ptr<ecat::detail::esi::EtherCATInfo>>
      parsed_files_;
  mutable std::multimap<ecat::slave_id, std::tuple<std::shared_ptr<ecat::detail::esi::Device1>, std::string>>
      devices_;
};

} // namespace ecat::detail
