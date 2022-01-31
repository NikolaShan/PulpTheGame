//
//  IATypes.h
//  CCLibs
//
//  Created by Mikhail Shulepov on 12/06/15.
//
//

#ifndef CCLibs_IATypes_h
#define CCLibs_IATypes_h

namespace IA {
    enum class ProductType {
        CONSUMABLE,
        NON_CONSUMABLE
    };
    
    enum class ProductsLoadStatus {
        NONE,
        LOADING,
        LOAD_SUCCESSFULL,
        LOAD_FAILED
    };
}

#endif
