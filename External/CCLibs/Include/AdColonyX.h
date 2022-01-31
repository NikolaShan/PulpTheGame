//
//  AdColonyAds.h
//  CCLibs
//
//  Created by Mikhail Shulepov on 08/07/15.
//
//

#ifndef CCLibs_AdColonyAds_h
#define CCLibs_AdColonyAds_h

#include "Advertise.h"
#include <vector>
#include <memory>

namespace Advertise {
    class AdColonyX: public AdSystem {
    public:
        /**
         * User metadata keys
         */
       static const std::string KEY_USER_AGE;                       /**< Set the user's age */
       static const std::string KEY_USER_INTERESTS;                 /**< Set the user's interests */
       static const std::string KEY_USER_GENDER;                    /**< Set the user's gender */
       static const std::string KEY_USER_LATITUDE;                  /**< Set the user's current latitude */
       static const std::string KEY_USER_LONGITUDE;                 /**< Set the user's current longitude */
       static const std::string KEY_USER_ANNUAL_HOUSEHOLD_INCOME;   /**< Set the user's annual house hold income in United States Dollars */
       static const std::string KEY_USER_MARITAL_STATUS;            /**< Set the user's marital status */
       static const std::string KEY_USER_EDUCATION;                 /**< Set the user's education level */
       static const std::string KEY_USER_ZIPCODE;                   /**< Set the user's known zip code */
        
        /**
         * User metadata values (for pre-defined values)
         */
       static const std::string USER_MALE;                          /**< User is male */
       static const std::string USER_FEMALE;                        /**< User is female */
        
       static const std::string USER_SINGLE;                        /**< User is single */
       static const std::string USER_MARRIED;                       /**< User is married */
        
       static const std::string USER_EDUCATION_GRADE_SCHOOL;        /**< User has a basic grade school education and has not attended high school */
       static const std::string USER_EDUCATION_SOME_HIGH_SCHOOL;    /**< User has completed at least some high school but has not received a diploma */
       static const std::string USER_EDUCATION_HIGH_SCHOOL_DIPLOMA; /**< User has received a high school diploma but has not completed any college */
       static const std::string USER_EDUCATION_SOME_COLLEGE;        /**< User has completed at least some college but doesn't have a college degree */
       static const std::string USER_EDUCATION_ASSOCIATES_DEGREE;   /**< User has been awarded at least 1 associates degree, but doesn't have any higher level degrees */
       static const std::string USER_EDUCATION_BACHELORS_DEGREE;    /**< User has been awarded at least 1 bachelors degree, but does not have a graduate level degree */
       static const std::string USER_EDUCATION_GRADUATE_DEGREE;     /**< User has been awarded at least 1 masters or doctorate level degree */
        
        
    public:       
        static std::shared_ptr<AdColonyX> getInstance();
        
        AdColonyX() {}
        virtual ~AdColonyX() {}
        
        virtual void start(const std::string &appId, const std::vector<std::string> &zoneIds, bool logEnabled) = 0;
        
        ///
        /// Interstitial Ads
        ///
        //virtual void playVideoAdForZone(const std::string &zoneId, std::function<void(bool)> completion) = 0;
        
        ///
        /// V4VC - Availability and Rewards
        ///
        
        /// Returns if it is possible for the user to receive a virtual currency reward for playing an ad in the zone
        /// @return A boolean indicating whether a reward is currently available for the user.
        /// @discussion This method takes into account whether V4VC has been configured properly for the zone on the AdColony Control Panel,
        ///             whether the user's daily reward cap has been reached, and whether there are ads available.
        virtual bool isVirtualCurrencyRewardAvailableForZone(const std::string &zoneID) = 0;
        
        /// Returns if it is possible for the user to receive a virtual currency reward for playing an ad in the zone
        /// @return An integer number of remaining virtual currency rewards that can possibly occur in the near future.
        /// @discussion This method takes into account daily caps, available ads, and other variables.
        virtual int getVirtualCurrencyRewardsAvailableTodayForZone(const std::string &zoneID) = 0;
        
        /// Returns the name of the virtual currency rewarded by the zone
        /// @return A string name of the virtual currency rewarded by the zone, as configured on the AdColony Control Panel.
        virtual std::string getVirtualCurrencyNameForZone(const std::string &zoneID) = 0;
        
        /// Returns the amount of virtual currency rewarded by the zone
        /// @return An integer indicating the amount of virtual currency rewarded by the zone, as configured on the AdColony Control Panel.
        virtual int getVirtualCurrencyRewardAmountForZone(const std::string &zoneID) = 0;
        
        virtual void playRewardedVideoForZone(const std::string &zoneID, std::function<void(bool)> completion) = 0;
        
        ///
        /// User Data and Interests
        ///
        
        /// Set per-user non personally-identifiable information for use by AdColony's ad serving system.
        virtual void setUserMetadata(const std::string &metadataType, const std::string &value) = 0;
        
        ///
        ///
        ///
        
        virtual void setDefaultInterstitialZoneID(const std::string &zoneId) {
            defaultInterstitialZoneId = zoneId;
        }
        
        std::string getDefaultInterstitialZoneId() const {
            return defaultInterstitialZoneId;
        }
        
    protected:
        std::string defaultInterstitialZoneId;
    };
}

#endif
