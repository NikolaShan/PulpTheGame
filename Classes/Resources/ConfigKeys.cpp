#include "ConfigKeys.h"
namespace R {
namespace Config {
namespace GameCenter {
const std::string PATH = "game_center";
namespace Achievements {
const std::string PATH = "game_center_achievements";
const std::string DESTROY = "game_center_achievements_destroy";
const std::string ATTEMPTS = "game_center_achievements_attempts";
const std::string LEVELS = "game_center_achievements_levels";
namespace Hidden {
const std::string PATH = "game_center_achievements_hidden";
const std::string LAST_CHANCE = "game_center_achievements_hidden_last_chance";
const std::string LONG_SESSION = "game_center_achievements_hidden_long_session";
const std::string CHAMPION = "game_center_achievements_hidden_champion";
const std::string DESTROYER = "game_center_achievements_hidden_destroyer";
const std::string SOCIAL = "game_center_achievements_hidden_social";
} //hidden
} //achievements
const std::string MAIN_LEADERBOARD = "game_center_main_leaderboard";
const std::string ENDLESS_LEADERBOARD = "game_center_endless_leaderboard";
} //game_center
const std::string SKIPS_PER_DAY_FOR_INVITE = "skips_per_day_for_invite";
namespace SecondChance {
const std::string PATH = "second_chance";
const std::string MIN_DEFEATS_COUNT = "second_chance_min_defeats_count";
const std::string PERCENT_COMPLETION = "second_chance_percent_completion";
} //second_chance
const std::string SHARE_URL = "share_url";
namespace Analytics {
const std::string PATH = "analytics";
namespace Tune {
const std::string PATH = "analytics_tune";
const std::string ADVERTISER_ID = "analytics_tune_advertiser_id";
const std::string CONVERSION_KEY = "analytics_tune_conversion_key";
} //tune
} //analytics
namespace Admob {
const std::string PATH = "admob";
const std::string REWARDED_VIDEO = "admob_rewarded_video";
const std::string BANNER = "admob_banner";
const std::string APP_ID = "admob_app_id";
const std::string INTERSTITIAL = "admob_interstitial";
} //admob
const std::string ATTEMPTS_TO_SKIP_FOR_VIDEO = "attempts_to_skip_for_video";
namespace Social {
const std::string PATH = "social";
const std::string FACEBOOK_INVITE_LINK = "social_facebook_invite_link";
const std::string VK_APP_ID = "social_vk_app_id";
} //social
const std::string RATE_URL = "rate_url";
const std::string NEW_SYSTEM_RATE_ENABLED = "new_system_rate_enabled";
namespace Ads {
const std::string PATH = "ads";
const std::string INTERVAL_INCREASE_STEP = "ads_interval_increase_step";
const std::string INTERSTITIAL_MIN_EVENTS_COUNT = "ads_interstitial_min_events_count";
const std::string MIN_INTERVAL_B_INTERSTITIALS = "ads_min_interval_b_interstitials";
const std::string MIN_INTERVAL_FROM_APP_LAUNCH = "ads_min_interval_from_app_launch";
const std::string ON_VICTORY = "ads_on_victory";
} //ads

} //R
}//Config