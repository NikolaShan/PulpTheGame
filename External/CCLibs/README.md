# Initial setup #

- add repository as submodule into folder 'External';

## iOS ##

- enable cocoapods support (fix all warnings);
- convert project to ARC;
- add **@ synthesize window;** to AppController.m
- add required source file to the xCode from folders: ** *Include, iOS, Shared* **
- add headers search path in the *Build Settings* to the folder ** *Include* **
- update main.m: **UIApplicationMain(argc, argv, nil, @"AppDelegateMultiplexer")**

## Android ##

- to run sample project perform **download-deps.py** firstly
- copy required java source files from folder ** *AndroidJava* ** to ** *proj.android/src/com/planemo* **
- add required libraries from ** *DependenciesAndroid* **
- setup Android.mk: add LOCAL_C_INCLUDES with path to the ** *Include* ** folder  
  list required src files in LOCAL_SRC_FILES
- [implement callback](https://bitbucket.org/snippets/Planemo/bE4aq) for Activity onCreate (this method is called before applicationDidFinishLaunching); 


# Detailed libraries setup & usage #

- [Advertise](https://bitbucket.org/Planemo/cclibs/wiki/Ads)
- [Ads Mediation](https://bitbucket.org/Planemo/cclibs/wiki/Ads%20Mediation)
- [Analytics](https://bitbucket.org/Planemo/cclibs/wiki/Analytics)
- [In-App purchases](https://bitbucket.org/Planemo/cclibs/wiki/In-App%20Purchases)
- Game Services
- [Share](https://bitbucket.org/Planemo/cclibs/wiki/Share)
- [Facebook](https://bitbucket.org/Planemo/cclibs/wiki/Facebook)
- [Vkontakte](https://bitbucket.org/Planemo/cclibs/wiki/Vkontakte)
- [Notifications](https://bitbucket.org/Planemo/cclibs/wiki/Notifications)
- [Cross Promo](https://bitbucket.org/Planemo/cclibs/wiki/Cross%20Promo%20Kit)
- [Localization](https://bitbucket.org/Planemo/cclibs/wiki/Localization)