/**
 * @file common.c
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief C++ source file/implementation of simsoft/common.h
 * @version 0.1
 * @date 2020-04-06
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#include "simsoft/common.h"

SimSoft::ReturnCode SimSoft::get_return_code() {
    return (SimSoft::ReturnCode)C_API::sim_get_return_code();
}

const char* SimSoft::get_return_code_string(ReturnCode return_code) {
    return C_API::sim_get_return_code_string((C_API::Sim_ReturnCode)return_code);
}