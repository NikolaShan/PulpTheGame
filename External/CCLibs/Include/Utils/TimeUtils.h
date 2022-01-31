//
//  TimeUtils.h
//  Pulp
//
//  Created by Mikhail Shulepov on 26/07/15.
//
//

#pragma once

namespace TimeUtils {
    const time_t SECONDS_IN_MINUTE = 60;
    const time_t SECONDS_IN_HOUR = SECONDS_IN_MINUTE * 60;
    const time_t SECONDS_IN_DAY = SECONDS_IN_HOUR * 24;
    const time_t SECONDS_IN_WEEK = SECONDS_IN_DAY * 7;
    const time_t SECONDS_IN_MONTH = SECONDS_IN_DAY * 30;
}