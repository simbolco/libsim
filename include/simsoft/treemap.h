/**
 * @file treemap.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for treemaps
 * @version 0.1
 * @date 2020-01-10
 * 
 * @copyright Copyright (c) 2020 LGPLv3
 * 
 */

#ifndef SIMSOFT_TREEMAP_H_
#define SIMSOFT_TREEMAP_H_

#include "./common.h"
#include "./allocator.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */

        typedef struct Sim_TreeMap {
            const struct {
                size_t key_type : 2;                                   // Sim_DataType
                size_t key_size : (sizeof(size_t)*8)-2;                // size of keys
                ssize_t (*key_comparison_func_ptr)(const void *const); // key comparison function
            } _key_properties;  // properties of treemap keys
            size_t _value_size; // size of treemap values
            const Sim_Allocator *const _allocator_ptr; // node allocator
            void* _root_ptr; // pointer to tree map root

            size_t count;   // amount of items stored in the treemap
        } Sim_TreeMap;

        
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
#       include "./exception.hh"
        
        template <
            class K,
            class V,
            class _Cmp,
            class _Alloc = Allocator
        >
        class TreeMap {

        };

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_TREEMAP_H_ */
