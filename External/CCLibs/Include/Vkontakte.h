//
//  Vkontakte.h
//  CCLibs
//
//  Created by Mikhail Shulepov on 11/06/15.
//
//

#ifndef __CCLibs__Vkontakte__
#define __CCLibs__Vkontakte__

#include <vector>
#include <memory>
#include "SocialBasicTypes.h"

namespace Social {
    
class Vkontakte {
public:
    static const std::string PERMISSION_EMAIL;
    
    static Vkontakte *getInstance();
    
    virtual void startWithAppId(const std::string &appId) = 0;
    
    virtual void loginWithPermissions(const std::vector<std::string> &permissions, LoginCallback callback) = 0;
    
    virtual void logout() = 0;
    virtual bool isLoggedIn() const = 0;
    virtual void requestUserInfo(UserInfoCallback callback) = 0;

    virtual void share(const std::string &text, const std::string &linkTitle, const std::string &link, ShareCompletion completion) = 0;
    
    virtual bool isVkCountry() const = 0;
    
protected:
    Vkontakte() {}
    virtual ~Vkontakte() {}
};
    
}


#endif /* defined(__CCLibs__Vkontakte__) */
