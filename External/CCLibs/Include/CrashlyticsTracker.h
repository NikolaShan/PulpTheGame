//
//  CrashlyticsTracker.h
//  Pulp
//
//  Created by Mikhail Shulepov on 14.12.15.
//
//

#pragma once

#include <string>
#include "Utils/format.h"

namespace CrashlyticsTracker {
    void setString(const std::string &key, const std::string &value);
    void setInt(const std::string &key, const int value);
    void setFloat(const std::string &key, const float value);
    void setBool(const std::string &key, const bool value);
    
   
    void log(const std::string format, fmt::ArgList args);
    FMT_VARIADIC(void, log, const std::string);
};
