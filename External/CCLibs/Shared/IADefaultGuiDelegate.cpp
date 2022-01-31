//
//  DefaultGuiDelegate.cpp
//  DaVinciQuiz
//
//  Created by mike on 18.05.14.
//
//

#include "IADefaultGuiDelegate.h"

#include "base/CCDirector.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCEventDispatcher.h"
#include "2d/CCScene.h"
#include "2d/CCLabel.h"
#include "2d/CCLayer.h"
#include "2d/CCActionInstant.h"
#include "2d/CCActionInterval.h"

#include "UI/UIProgressHUD.h"

#include "Utils/Localization.h"

USING_NS_CC;

namespace IA {

DefaultGuiDelegate::DefaultGuiDelegate(): progressHUD(nullptr) {

}

DefaultGuiDelegate::~DefaultGuiDelegate() {
    if (progressHUD) {
        progressHUD->dismiss();
        progressHUD->release();
    }
}

cocos2d::Scene *DefaultGuiDelegate::getCurrentScene() const {
    return Director::getInstance()->getRunningScene();
}

void DefaultGuiDelegate::removeOverlay() {
    getProgressHUD()->dismiss();
}
    
ProgressHUD *DefaultGuiDelegate::getProgressHUD() {
    if (!progressHUD) {
        progressHUD = ProgressHUD::create();
        progressHUD->retain();
    }
    return progressHUD;
}
    
void DefaultGuiDelegate::removeOverlayWithSuccess(const std::string &message) {
    getProgressHUD()->setSwallowTouches(false);
    getProgressHUD()->showSuccessWithStatus(message);
}
    
void DefaultGuiDelegate::removeOverlayWithFailure(const std::string &message) {
    getProgressHUD()->setSwallowTouches(false);
    getProgressHUD()->showFailureWithStatus(message);
}

void DefaultGuiDelegate::presentOverlay(const std::string &message) {
    getProgressHUD()->setSwallowTouches(true);
    getProgressHUD()->showLoadingWithStatus(message);
}

void DefaultGuiDelegate::onStartProductsRestoration() {
    isRestoring = true;
    presentOverlay(Localization::getInstance()->getString("IA/Restoring", "Restoring..."));
}
    
void DefaultGuiDelegate::onFinishProductsRestoration(const Products &restoredProducts) {
    if (isRestoring) {
        if (restoredProducts.empty()) {
            removeOverlayWithSuccess(Localization::getInstance()->getString("IA/NothingToRestore", "Nothing to restore"));
        } else {
            std::string message = Localization::getInstance()->getString("IA/RestoreSucceed", "- Restored -");
            for (int i = 0; i < restoredProducts.size(); ++i) {
                auto product = restoredProducts[i];
                message += "\n";
                message += product->getLocalizedTitle();
            }
            removeOverlayWithSuccess(message);
        }
    }
}
    
void DefaultGuiDelegate::onFailProductsRestoration() {
    if (isRestoring) {
        removeOverlayWithFailure(Localization::getInstance()->getString("IA/Error", "Error: try again later..."));
    }
}

void DefaultGuiDelegate::onStartPurchaseFlow(IA::ProductPtr product) {
    isRestoring = false;
    presentOverlay(Localization::getInstance()->getString("IA/Loading", "Loading..."));
}

void DefaultGuiDelegate::onProductPurchased(IA::ProductPtr product) {
    if (!isRestoring) {
        const std::string purchasedResult = Localization::getInstance()
            ->getFormattedStringF("IA/PurchaseSucceed", "- Purchased successfully-\n{}", product->getLocalizedTitle());
        removeOverlayWithSuccess(purchasedResult);
    }
}

void DefaultGuiDelegate::onProductPurchaseFailed(IA::ProductPtr product, const std::string &error) {
    if (!isRestoring) {
        removeOverlayWithFailure(Localization::getInstance()->getString("IA/Error", "Error: try again later..."));
    }
}

void DefaultGuiDelegate::onProductPurchaseCancelled() {
    if (!isRestoring) {
        removeOverlay();
    }
}

}