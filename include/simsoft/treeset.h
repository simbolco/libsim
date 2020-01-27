/**
 * @file treeset.h
 * @author Simon Struthers (snstruthers@gmail.com)
 * @brief Header for treesets
 * @version 0.1
 * @date 2020-01-10
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef SIMSOFT_TREESET_H_
#define SIMSOFT_TREESET_H_

#include "./common.h"
#include "./allocator.h"

CPP_NAMESPACE_START(SimSoft)
    CPP_NAMESPACE_C_API_START /* C API */

        typedef struct Sim_TreeSet {
            const struct {
                size_t item_type : 2;                                   // Sim_DataType
                size_t item_size : (sizeof(size_t)*8)-2;                // size of items
                ssize_t (*item_comparison_func_ptr)(const void *const); // item comparison function
            } _item_properties;  // properties of treeset items
            const Sim_Allocator *const _allocator_ptr; // node allocator
            void* _root_ptr; // pointer to tree map root

            size_t count;   // amount of items stored in the treeset
        } Sim_TreeSet;

        
    
    CPP_NAMESPACE_C_API_END /* end C API */

#   ifdef __cplusplus /* C++ API */
#       include "./exception.hh"
        
        template <
            class T,
            class _Cmp,
            class _Alloc = Allocator
        >
        class TreeSet {

        };

#   endif /* end C++ API */
CPP_NAMESPACE_END(SimSoft) /* end SimSoft namespace */

#endif /* SIMSOFT_TREESET_H_ */
