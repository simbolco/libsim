/**
 * @file libsim.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Include header for the entire SimSoft library.
 * @version 0.2
 * @date 2020-07-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

/**
 * @mainpage
 * 
 * @tableofcontents
 * 
 * @section intro_sec Introduction
 * 
 * The <b>Sim</b>on Bolivar & Company <b>Soft</b>ware <b>Library</b></i> is (intended to be) a
 * cross-platform multilingual library containing various useful features to ease software
 * development.
 * 
 */

/**
 * @defgroup containers Containers
 * @brief Generic containers
 * @par
 * The SimSoft Library contains various general-purpose generic container types, similar to those
 * found in the C++ standard library, but with facilities such that they can easily be used in C.
 */

#ifndef SIMSOFT_LIBSIM_H_
#define SIMSOFT_LIBSIM_H_

#include "allocator.h"
#include "cmdlnopt.h"
#include "debug.h"
#include "dynlib.h"
#include "except.h"
#include "hashmap.h"
#include "hashset.h"
#include "memmgmt.h"
#include "random.h"
#include "string.h"
#include "utf8.h"
#include "util.h"
#include "vector.h"

#endif // SIMSOFT_LIBSIM_H_
