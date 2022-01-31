#pragma once
#include <string>
#include <vector>

namespace R {
namespace Config {
namespace GameCenter {
extern const std::string PATH;
namespace Achievements {
extern const std::string PATH;
extern const std::string DESTROY;
extern const std::string ATTEMPTS;
extern const std::string LEVELS;
namespace Hidden {
extern const std::string PATH;
extern const std::string LAST_CHANCE;
extern const std::string LONG_SESSION;
extern const std::string CHAMPION;
extern const std::string DESTROYER;
extern const std::string SOCIAL;
} //hidden
} //achievements
extern const std::string MAIN_LEADERBOARD;
extern const std::string ENDLESS_LEADERBOARD;
} //game_center
extern const std::string SKIPS_PER_DAY_FOR_INVITE;
namespace SecondChance {
extern const std::string PATH;
extern const std::string MIN_DEFEATS_COUNT;
extern const std::string PERCENT_COMPLETION;
} //second_chance
extern const std::string SHARE_URL;
namespace Analytics {
extern const std::string PATH;
namespace Tune {
extern const std::string PATH;
extern const std::string ADVERTISER_ID;
extern const std::string CONVERSION_KEY;
} //tune
} //analytics
namespace Admob {
extern const std::string PATH;
extern const std::string REWARDED_VIDEO;
extern const std::string BANNER;
extern const std::string APP_ID;
extern const std::string INTERSTITIAL;
} //admob
extern const std::string ATTEMPTS_TO_SKIP_FOR_VIDEO;
namespace Social {
extern const std::string PATH;
extern const std::string FACEBOOK_INVITE_LINK;
extern const std::string VK_APP_ID;
} //social
extern const std::string RATE_URL;
extern const std::string NEW_SYSTEM_RATE_ENABLED;
namespace Ads {
extern const std::string PATH;
extern const std::string INTERVAL_INCREASE_STEP;
extern const std::string INTERSTITIAL_MIN_EVENTS_COUNT;
extern const std::string MIN_INTERVAL_B_INTERSTITIALS;
extern const std::string MIN_INTERVAL_FROM_APP_LAUNCH;
extern const std::string ON_VICTORY;
} //ads

} //R
}//Config