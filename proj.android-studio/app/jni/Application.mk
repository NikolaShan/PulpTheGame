APP_STL := gnustl_static

APP_CPPFLAGS := -frtti -std=c++11 -fsigned-char 
APP_CFLAGS := -std=gnu11
APP_LDFLAGS := -latomic


ifeq ($(NDK_DEBUG),1)
  APP_CPPFLAGS += -DCOCOS2D_DEBUG=1
  APP_OPTIM := debug
else
  APP_CPPFLAGS += -DNDEBUG
  APP_OPTIM := release
endif
