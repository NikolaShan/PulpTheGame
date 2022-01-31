//
//  UserInfo.h
//  CCLibs
//
//  Created by Mikhail Shulepov on 11/06/15.
//
//

#ifndef __CCLibs__UserInfo__
#define __CCLibs__UserInfo__

#include <string>
#include <functional>
#include <algorithm>
#include <memory>

namespace Social {
    class UserInfo {
    public:
        std::string name;
        
        std::string firstName;
        std::string lastName;
        
        std::string email;
        std::string avatarUrl;
    };
    
    class LoginResult {
    public:
        static const std::string ErrorCancelled;
        
        static LoginResult failure(const std::string &error) {
            LoginResult result;
            result.error = error;
            return result;
        }
        
        static LoginResult success(const std::vector<std::string> &grantedPermissions) {
            LoginResult result;
            result.loggedIn = true;
            result.grantedPermissions = grantedPermissions;
            return result;
        }
        
        static LoginResult cancellation() {
            LoginResult result;
            result.error = ErrorCancelled;
            return result;
        }
        
        bool isCancelled() const {
            return error == ErrorCancelled;
        }
        
        bool hasPermission(const std::string &str) const {
            return std::find(grantedPermissions.begin(), grantedPermissions.end(), str) != grantedPermissions.end();
        }
        
        bool loggedIn = false;
        std::string error;
        std::vector<std::string> grantedPermissions;
    };
    
    using UserInfoPtr = std::shared_ptr<UserInfo>;
    
    using UserInfoCallback = std::function<void(UserInfoPtr)>;
    using LoginCallback = std::function<void(LoginResult)>;
    using ShareCompletion = std::function<void(bool)>;
};



#endif /* defined(__CCLibs__UserInfo__) */
