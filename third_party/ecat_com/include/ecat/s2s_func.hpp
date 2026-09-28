#ifndef ECAT_S2S_FUNC_HPP
#define ECAT_S2S_FUNC_HPP

#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <type_traits>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "ecat/types.hpp"


namespace ecat {


class S2SConfig
{
public:
    S2SConfig(){};
    ~S2SConfig(){};

    static S2SConfig& use();

protected:
    static S2SConfig mInstance;

    std::vector<std::pair<std::pair<std::uint16_t, pdo_entry_idx>, std::pair<std::uint16_t, pdo_entry_idx>>> mTotalS2S;

    std::map<std::pair<std::uint16_t, pdo_entry_idx>, std::pair<void **, int>> address;

public:
    void run();

    void count();

    void regist(std::pair<std::uint16_t, pdo_entry_idx> src, std::pair<std::uint16_t, pdo_entry_idx> dst);

    void get(void **addr, int size, std::pair<std::uint16_t, pdo_entry_idx> pdoInfo);

    bool has(std::pair<std::uint16_t, pdo_entry_idx> pdoInfo);

    std::int32_t getPdoValue(std::pair<std::uint16_t, pdo_entry_idx> pdoInfo);

    bool setPdoValue(std::pair<std::uint16_t, pdo_entry_idx> pdoInfo, int value);
};
}

#endif