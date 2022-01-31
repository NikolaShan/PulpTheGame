#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"
#include "../JniSupport.h"

#include "private/IAStore.h"

USING_NS_CC;


static const std::string CB_CLASS_NAME = "com/planemo/libs/purchases/PurchaseManager";

class AndroidStore: public IA::Store {    
    

public:
    AndroidStore(IA::Store::Delegate *delegate): IA::Store(delegate) {
        JNIEnv *env = JniHelper::getEnv();
        JniMethodInfo startMethodInfo;

        if (! JniHelper::getStaticMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(),  "init", "(J)V")) {
            CCASSERT(false, "ERROR: Can't find signature for PurchaseManager::init");
        }
        long address =  reinterpret_cast<long>(delegate);
        jlong pointerToNativeListener = address;
        env->CallStaticVoidMethod(startMethodInfo.classID, startMethodInfo.methodID, pointerToNativeListener);
        env->DeleteLocalRef(startMethodInfo.classID);
    }
    
    virtual ~AndroidStore() {
        
    }

private:
    jobject getJavaInstance() const {
        JniMethodInfo methodInfo;
        const std::string methodSignature = "()L" + CB_CLASS_NAME + ";";
        if (! JniHelper::getStaticMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getInstance", methodSignature.c_str())) {
            CCASSERT(false, "ERROR: Can't find method for PurchaseManager::getInstance");
        }
        jobject jPurchaseManager = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return jPurchaseManager;
    }

    std::string callJavaGetPropertyByProductId(const std::string &methodName, const std::string &productID) const{
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), methodName.c_str(), "(Ljava/lang/String;)Ljava/lang/String;")) {
            CCASSERT(false, ("Can't find signature for PurchaseManager::" + methodName).c_str());
            return "";
        }
            
        jstring jidentifier = methodInfo.env->NewStringUTF(productID.c_str()); 
        jstring jtitle = (jstring)methodInfo.env->CallObjectMethod(getJavaInstance(), methodInfo.methodID, jidentifier);
        std::string title = JniHelper::jstring2string(jtitle);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jidentifier);
        methodInfo.env->DeleteLocalRef(jtitle);
        return title;
    }
       
public:
    virtual void requestProducts(const std::vector<std::string> &products, const std::vector<IA::ProductType> &types) override {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "requestProducts", "(Ljava/util/List;Ljava/util/List;)V")) {
            CCASSERT(false, "Can't find signature for PurchaseManager::requestProducts");
            return;
        }

        std::vector<std::string> typesString;
        for(auto type: types){
            typesString.push_back(type == IA::ProductType::CONSUMABLE ? "CONSUMABLE" : "NON_CONSUMABLE");
        }

        jobject jlist = JniSupport::StdStringVectorToJavaStringArrayList(products);
        jobject jlistTypes = JniSupport::StdStringVectorToJavaStringArrayList(typesString);
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jlist, jlistTypes);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jlist);
        methodInfo.env->DeleteLocalRef(jlistTypes);
    }
    
    //Product type here required for android implementation
    virtual void buyProduct(const std::string &identifier) override {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "buyProduct", "(Ljava/lang/String;)V")) {
            CCASSERT(false, "Can't find signature for PurchaseManager::buyProduct");
            return;
        }

        jstring jidentifier = methodInfo.env->NewStringUTF(identifier.c_str());
        methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jidentifier);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jidentifier);
    }
    
    virtual bool isPurchasesEnabled() const override {
        return true;
    }
    
    virtual void restorePurchasedProducts() override {
        //nothing to do
    }
    
    //returns true for iOS, false for Android (hide restore purchases button)
    virtual bool canRestorePurchases() const override {
        return false;
    }
    
    virtual std::string getLocalizedTitle(const std::string &productID) const override {
        return callJavaGetPropertyByProductId("getLocalizedTitle", productID);
    }
    
    virtual std::string getLocalizedDescription(const std::string &productID) const override {
        return callJavaGetPropertyByProductId("getLocalizedDescription", productID);
    }
    
    virtual std::string getProductPrice(const std::string &productID) const override {
        return callJavaGetPropertyByProductId("getProductPrice", productID);
    }
    
    virtual double getProductPriceValue(const std::string &productID) const override {
        JniMethodInfo methodInfo;
        if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getProductPriceValue", "(Ljava/lang/String;)D")) {
            CCASSERT(false, "Can't find signature for PurchaseManager::getProductPriceValue");
            return 0;
        }
            
        jstring jidentifier = methodInfo.env->NewStringUTF(productID.c_str()); 
        double value = methodInfo.env->CallDoubleMethod(getJavaInstance(), methodInfo.methodID, jidentifier);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jidentifier);
        return value;
    }
};

std::shared_ptr<IA::Store> IA::Store::create(Store::Delegate *delegate) {
    return std::make_shared<AndroidStore>(delegate);
}

#ifdef __cplusplus
    extern "C" {
    #endif

    void Java_com_planemo_libs_purchases_PurchaseManager_00024Listener_onProductsLoaded(JNIEnv *env, jobject thiz, jobjectArray products, jlong pointerToNativeListener) {
        IA::Store::Delegate *delegate = reinterpret_cast<IA::Store::Delegate*>(pointerToNativeListener);
        delegate->productsLoaded(JniSupport::ArrayJavaObjectsToVectorString(products));
        CCLOG("00024Listener_onProductsLoaded");
    }

    void Java_com_planemo_libs_purchases_PurchaseManager_00024Listener_onProductsLoadFailed(JNIEnv *env, jobject thiz, jlong pointerToNativeListener) {
        IA::Store::Delegate *delegate = reinterpret_cast<IA::Store::Delegate*>(pointerToNativeListener);
        delegate->productsLoadFailed();
        CCLOG("00024Listener_onProductsLoadFailed");
    }

    void Java_com_planemo_libs_purchases_PurchaseManager_00024Listener_onProductPurchaseFailed(JNIEnv *env, jobject thiz, jstring productId, jstring error, jlong pointerToNativeListener) {
        IA::Store::Delegate *delegate = reinterpret_cast<IA::Store::Delegate*>(pointerToNativeListener);
        delegate->productPurchaseFailed(JniHelper::jstring2string(productId), JniHelper::jstring2string(error));
        CCLOG("00024Listener_onProductPurchaseFailed");
    }

    void Java_com_planemo_libs_purchases_PurchaseManager_00024Listener_onProductPurchased(JNIEnv *env, jobject thiz, jstring productId, jlong pointerToNativeListener) {
        IA::Store::Delegate *delegate = reinterpret_cast<IA::Store::Delegate*>(pointerToNativeListener);
        delegate->productPurchased(JniHelper::jstring2string(productId));
        CCLOG("00024Listener_onProductPurchased");
    }

    void Java_com_planemo_libs_purchases_PurchaseManager_00024Listener_onProductPurchaseCancelled(JNIEnv *env, jobject thiz, jstring productId, jlong pointerToNativeListener) {
        IA::Store::Delegate *delegate = reinterpret_cast<IA::Store::Delegate*>(pointerToNativeListener);
        delegate->productPurchaseCancelled(JniHelper::jstring2string(productId));
        CCLOG("00024Listener_onProductPurchaseCancelled");
    }


#ifdef __cplusplus
}
#endif



