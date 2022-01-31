//
//  LocalNotificationManager.h
//  CCLibs
//
//  Created by Mikhail Shulepov on 11/06/15.
//
//

#ifndef __CCLibs__LocalNotificationManager__
#define __CCLibs__LocalNotificationManager__

#include <string>
#include "base/CCValue.h"

class NotificationsManager {
public:
    class Handler {
    public:
        virtual ~Handler() {}
        virtual void hanleNotification(int tag, const cocos2d::ValueMap &info) = 0;
    };
    
    //registerFirstSession - only for iOS, to prevent annoying user on the first session
    static void startWithNotificationsHandler(std::shared_ptr<Handler> handler, bool registerFirstSession) {
        instance = create(handler, registerFirstSession);
    }
    
    static NotificationsManager *getInstance() {
        return instance;
    }
    
    //action used only for iOS
    virtual void scheduleLocalNotification(int tag, const std::string &message, const std::string &action, int delayInSeconds, const cocos2d::ValueMap &info) = 0;
    virtual void cancelLocalNotifications(const std::vector<int> &tags) = 0;

    
protected:
    NotificationsManager() {}
    virtual ~NotificationsManager() {}
    
private:
    static NotificationsManager *create(std::shared_ptr<Handler> handler, bool registerFirstSession);
    static NotificationsManager *instance;
};

#endif /* defined(__CCLibs__LocalNotificationManager__) */
