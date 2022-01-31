//
//  Facebook.h
//  CCLibs
//
//  Created by Mikhail Shulepov on 11/06/15.
//
//

#ifndef __CCLibs__Facebook__
#define __CCLibs__Facebook__

#include <vector>
#include <memory>
#include "SocialBasicTypes.h"

namespace Social {
    
class Facebook {
public:
    using InvitationCallback = std::function<void(bool)>;
    
    static const std::string PERMISSION_EMAIL;
    static const std::string PERMISSION_PUBLIC_PROFILE;
    static const std::string PERMISSION_USER_FRIENDS;
    
    static Facebook *getInstance();

    virtual void start() = 0;
    virtual void loginWithReadPermissions(const std::vector<std::string> &permissions, LoginCallback callback) = 0;
    
    virtual void logout() = 0;
    virtual bool isLoggedIn() const = 0;
    virtual UserInfoPtr getUserInfo() = 0; //may return null
    
    virtual bool shareLink(const std::string &link, const std::string &title, const std::string &imageUrl, const std::string &contentDescription, ShareCompletion completion) = 0;
    
    virtual bool canInviteFriends() const = 0;
    virtual bool inviteFriends(const std::string &appLinkUrl, InvitationCallback callback) = 0;
    
    //advanced (app center):
    //score posting
    //achievements
    //requests
    
protected:
    Facebook() {}
    virtual ~Facebook() {}
};
    
}

#endif /* defined(__CCLibs__Facebook__) */
