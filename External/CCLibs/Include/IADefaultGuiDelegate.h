//
//  InAppGuiDelegate.h
//  DaVinciQuiz
//
//  Created by mike on 18.05.14.
//
//

#ifndef __DaVinciQuiz__InAppGuiDelegate__
#define __DaVinciQuiz__InAppGuiDelegate__

#include "IAPurchasesManager.h"

namespace cocos2d {
    class Scene;
    class Label;
}

class ProgressHUD;

namespace IA {
    class DefaultGuiDelegate : public PurchasesManager::GuiDelegate {
    public:
        DefaultGuiDelegate();
        virtual ~DefaultGuiDelegate();
        
        virtual void onStartProductsRestoration() override;
        virtual void onFinishProductsRestoration(const Products &restoredProducts) override;
        virtual void onFailProductsRestoration() override;
        
        virtual void onStartPurchaseFlow(IA::ProductPtr product) override;
        virtual void onProductPurchased(IA::ProductPtr product) override;
        virtual void onProductPurchaseFailed(IA::ProductPtr product, const std::string &error) override;
        virtual void onProductPurchaseCancelled() override;
        
    private:
        cocos2d::Scene *getCurrentScene() const;
        void removeOverlay();
        void removeOverlayWithSuccess(const std::string &message);
        void removeOverlayWithFailure(const std::string &failure);
        void presentOverlay(const std::string &message);
        ProgressHUD *getProgressHUD();
        
    private:
        bool isRestoring = false;      
        cocos2d::Label *messageLabel;
        ProgressHUD *progressHUD;
    };

}


#endif /* defined(__DaVinciQuiz__InAppGuiDelegate__) */
