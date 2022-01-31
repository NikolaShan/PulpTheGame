package com.planemo.libs.purchases;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.*;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import com.android.vending.billing.IInAppBillingService;
import com.planemo.libs.MultiplexerActivity;
import com.planemo.libs.support.AppUtils;
import org.cocos2dx.lib.Cocos2dxHelper;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.*;
import com.planemo.libs.purchases.Product.ProductType;


/**
 * Created by edward on 02/07/15.
 *
 */
@SuppressWarnings("unused")
public class PurchaseManager {
    public static final String TAG = PurchaseManager.class.getName();
    private static final int REQUEST_PURCHASE_CODE = 1001;
    private static final String DEV_PAYLOAD = "devPalksjrjkasd0983kksldksl";
    private static PurchaseManager mInstance;
    private HashMap<String, Product> mProductsMap; // (sku, product)
    private PurchaseManagerNativeListener mNativeManagerListener;
    private String mCurrentSkuProduct = "";

    private IInAppBillingService mService;
    private ServiceConnection mServiceConn = new ServiceConnection() {
        @Override
        public void onServiceDisconnected(ComponentName name) {
            mService = null;
        }

        @Override
        public void onServiceConnected(ComponentName name,
                                       IBinder service) {
            mService = IInAppBillingService.Stub.asInterface(service);
        }
    };

    private PurchaseManager(long pointerToNativeDelegate) {
        mNativeManagerListener = new PurchaseManagerNativeListener(pointerToNativeDelegate);
        ActivityLifecycleListener lifecycleTracker = new ActivityLifecycleListener();
        MultiplexerActivity.getInstance().addLifecycleListener(lifecycleTracker);
    }

    public static void init(long pointerToNativeDelegate) {
        Log.v(TAG, " init " + pointerToNativeDelegate);
        mInstance = new PurchaseManager(pointerToNativeDelegate);
    }

    public static PurchaseManager getInstance() {
        return mInstance;
    }


    public void requestProducts(List<String> idList, List<String> productTypeList) {
        Log.v(TAG, "Test request products " + idList);

        if (mService == null || idList == null) {
            mNativeManagerListener.onProductsLoadFailed();
            return;
        }

        final HashMap<String, ProductType> tempMapSkuTypes = new HashMap<>(idList.size());
        if(productTypeList != null && idList.size() == productTypeList.size()){
            for(int i = 0; i < idList.size(); i++){
                ProductType type = productTypeList.get(i).equalsIgnoreCase("CONSUMABLE") ? ProductType.CONSUMABLE : ProductType.NON_CONSUMABLE;
                tempMapSkuTypes.put(idList.get(i), type);
            }
        }

        ArrayList<String> skuList = new ArrayList<>(idList);
        final Bundle querySkus = new Bundle();
        querySkus.putStringArrayList(IAConstants.ITEM_ID_LIST, skuList);

        new AsyncTask<Void, Void, Void>() {
            @Override
            protected Void doInBackground(Void... params) {
                try {
                    Bundle skuDetails = mService.getSkuDetails(IAConstants.GOOGLE_API_VERSION,
                            AppUtils.getPackageName(), IAConstants.PRODUCT_TYPE_MANAGED, querySkus);

                    int response = skuDetails.getInt(IAConstants.RESPONSE_CODE);
                    if (response == 0) {
                        mProductsMap = new HashMap<>();
                        ArrayList<String> responseList
                                = skuDetails.getStringArrayList(IAConstants.DETAILS_LIST);
                        if (responseList != null) {
                            ArrayList<String> idsList = new ArrayList<>(responseList.size());
                            for (String thisResponse : responseList) {
                                JSONObject object = new JSONObject(thisResponse);
                                Product product = new Product();
                                product.setSku(object.optString(IAConstants.RESPONSE_PRODUCT_ID));
                                product.setTitle(object.optString(IAConstants.RESPONSE_TITLE));
                                product.setDescription(object.optString(IAConstants.RESPONSE_DESCRIPTION));
                                product.setPrice(object.optString(IAConstants.RESPONSE_PRICE));
                                product.setCurrencyCode(object.optString(IAConstants.RESPONSE_PRICE_CURRENCY));
                                product.setPriceMicro(object.optInt(IAConstants.RESPONSE_PRICE_MICROS));
                                product.setConsumeType(tempMapSkuTypes.get(product.getSku()));
                                mProductsMap.put(product.getSku(), product);
                                idsList.add(product.getSku());
                            }
                            mNativeManagerListener.onProductsLoaded(idsList.toArray());
                            checkOwnedProducts();
                        }
                    } else {
                        mNativeManagerListener.onProductsLoadFailed();
                    }

                } catch (RemoteException | org.json.JSONException e) {
                    e.printStackTrace();
                }
                return null;
            }
        }.execute();
    }

    public void buyProduct(String sku) {
        Log.v(TAG, "Test buy product ");
        try {
            mCurrentSkuProduct = sku;
            Bundle buyIntentBundle = mService.getBuyIntent(IAConstants.GOOGLE_API_VERSION, AppUtils.getPackageName(),
                    sku, IAConstants.PRODUCT_TYPE_MANAGED, DEV_PAYLOAD);
            if (buyIntentBundle.getInt(IAConstants.RESPONSE_CODE) == 0) {
                PendingIntent pendingIntent = buyIntentBundle.getParcelable(IAConstants.BUY_INTENT);

                Cocos2dxHelper.getActivity().startIntentSenderForResult(pendingIntent.getIntentSender(),
                        REQUEST_PURCHASE_CODE, new Intent(), 0, 0, 0);
            } else {
                mNativeManagerListener.onProductPurchaseFailed(sku, "Response error");
            }

        } catch (RemoteException | IntentSender.SendIntentException e) {
            e.printStackTrace();
            mNativeManagerListener.onProductPurchaseFailed(sku, "Exception");
        }
    }

    public void checkOwnedProducts() {
        new AsyncTask<Void, Void, Void>() {
            @Override
            protected Void doInBackground(Void... params) {
                try {
                    Bundle ownedItems = mService.getPurchases(IAConstants.GOOGLE_API_VERSION, AppUtils.getPackageName(),
                            IAConstants.PRODUCT_TYPE_MANAGED, null);
                    int response = ownedItems.getInt(IAConstants.RESPONSE_CODE);
                    if (response == 0) {
                        ArrayList<String> ownedSkus =
                                ownedItems.getStringArrayList(IAConstants.INAPP_PURCHASE_ITEM_LIST);
                        ArrayList<String> purchaseDataList =
                                ownedItems.getStringArrayList(IAConstants.INAPP_PURCHASE_DATA_LIST);

                        for (int i = 0; i < purchaseDataList.size(); ++i) {
                            String purchaseData = purchaseDataList.get(i);
                            String sku = ownedSkus.get(i);
                            if (isConsumable(sku)) {
                                //Consume old purchases
                                JSONObject jo = new JSONObject(purchaseData);
                                consumeProduct(jo.getString(IAConstants.RESPONSE_PURCHASE_TOKEN));
                            }else{
                                //Restore non-consumable (no-ads, pro etc.)
                                mNativeManagerListener.onProductPurchased(sku);
                            }
                            // do something with this purchase information
                            // e.g. display the updated list of products owned by user
                        }
                        // if continuationToken != null, call getPurchases again
                        // and pass in the token to retrieve more items
                    }

                } catch (RemoteException | JSONException e) {
                    e.printStackTrace();
                }
                return null;
            }
        }.execute();
    }

    public String getLocalizedTitle(String productId) {
        if (mProductsMap != null) {
            Product product = mProductsMap.get(productId);
            if (product != null)
                return product.getTitle();
        }
        return "";
    }


    public String getLocalizedDescription(String productId) {
        if (mProductsMap != null) {
            Product product = mProductsMap.get(productId);
            if (product != null)
                return product.getDescription();
        }
        return "";
    }

    public String getProductPrice(String productId) {
        if (mProductsMap != null) {
            Product product = mProductsMap.get(productId);
            if (product != null)
                return product.getPrice();
        }
        return "";
    }


    public double getProductPriceValue(String productId) {
        if (mProductsMap != null) {
            Product product = mProductsMap.get(productId);
            if (product != null)
                return product.getPriceMicro();
        }
        return 0;
    }


    private void consumeProduct(final String token) {
        if (token == null || mService == null) return;
        new AsyncTask<Void, Void, Void>() {
            @Override
            protected Void doInBackground(Void... params) {
                try {
                    int response = mService.consumePurchase(IAConstants.GOOGLE_API_VERSION, AppUtils.getPackageName(), token);
                    if (response == 0) {
                        Log.v(TAG, "Purchase has been consumed");
                    } else {
                        Log.v(TAG, "Purchase has not been consumed");
                    }

                } catch (RemoteException e) {
                    e.printStackTrace();
                }
                return null;
            }
        }.execute();
    }

    private boolean isConsumable(String sku) {
        if (mProductsMap != null && sku != null) {
            Product product = mProductsMap.get(sku);
            if (product != null)
                return product.getConsumeType() == ProductType.CONSUMABLE;
        }
        return true;
    }

    private class ActivityLifecycleListener extends MultiplexerActivity.AbstractLifecycleListener {
        public ActivityLifecycleListener() {

        }

        @Override
        public void onCreate(Activity activity, Bundle savedInstanceState) {
            Intent serviceIntent = new Intent(IAConstants.VENDING_BIND);
            serviceIntent.setPackage(IAConstants.VENDING_PACKAGE);
            activity.bindService(serviceIntent, mServiceConn, Context.BIND_AUTO_CREATE);
        }

        @Override
        public void onDestroy(Activity activity) {
            if (mService != null && mServiceConn != null) {
                activity.unbindService(mServiceConn);
            }
        }

        @Override
        public boolean onActivityResult(Activity activity, int requestCode, int resultCode, Intent data) {

            if (requestCode == REQUEST_PURCHASE_CODE) {
                int responseCode = data.getIntExtra(IAConstants.RESPONSE_CODE, 0);
                String purchaseData = data.getStringExtra(IAConstants.INAPP_PURCHASE_DATA);
                //  String dataSignature = data.getStringExtra(IAConstants.INAPP_DATA_SIGNATURE);
                if(purchaseData != null) {
                    try {
                        JSONObject jo = new JSONObject(purchaseData);
                        String sku = jo.getString(IAConstants.RESPONSE_PRODUCT_ID);
                        if (resultCode == Activity.RESULT_OK && responseCode == IAConstants.BILLING_RESPONSE_RESULT_OK
                                && jo.getString(IAConstants.RESPONSE_DEVELOPER_PAYLOAD).equals(DEV_PAYLOAD)) {
                            //product has been purchased
                            mNativeManagerListener.onProductPurchased(sku);
                            if (isConsumable(sku)) {
                                consumeProduct(jo.getString(IAConstants.RESPONSE_PURCHASE_TOKEN));
                            }
                        } else {
                            checkResponseCode(responseCode, sku);
                        }
                    } catch (Exception e) {
                        checkResponseCode(responseCode, mCurrentSkuProduct);
                        e.printStackTrace();
                    }
                }else{
                    checkResponseCode(responseCode, mCurrentSkuProduct);
                }

            }
            return false;
        }
    }

    private void checkResponseCode(int responseCode, String sku) {
        String errMsg;
        switch (responseCode) {
            case IAConstants.BILLING_RESPONSE_RESULT_USER_CANCELED:
                mNativeManagerListener.onProductPurchaseCancelled(sku);
                return;
            case IAConstants.BILLING_RESPONSE_RESULT_SERVICE_UNAVAILABLE:
                errMsg = IAConstants.BILLING_RESPONSE_RESULT_SERVICE_UNAVAILABLE_MESSAGE;
                break;
            case IAConstants.BILLING_RESPONSE_RESULT_BILLING_UNAVAILABLE:
                errMsg = IAConstants.BILLING_RESPONSE_RESULT_BILLING_UNAVAILABLE_MESSAGE;
                break;
            case IAConstants.BILLING_RESPONSE_RESULT_ITEM_UNAVAILABLE:
                errMsg = IAConstants.BILLING_RESPONSE_RESULT_ITEM_UNAVAILABLE_MESSAGE;
                break;
            case IAConstants.BILLING_RESPONSE_RESULT_DEVELOPER_ERROR:
                errMsg = IAConstants.BILLING_RESPONSE_RESULT_DEVELOPER_ERROR_MESSAGE;
                break;
            case IAConstants.BILLING_RESPONSE_RESULT_ERROR:
                errMsg = IAConstants.BILLING_RESPONSE_RESULT_ERROR_MESSAGE;
                break;
            case IAConstants.BILLING_RESPONSE_RESULT_ITEM_ALREADY_OWNED:
                errMsg = IAConstants.BILLING_RESPONSE_RESULT_ITEM_ALREADY_OWNED_MESSAGE;
                break;
            case IAConstants.BILLING_RESPONSE_RESULT_ITEM_NOT_OWNED:
                errMsg = IAConstants.BILLING_RESPONSE_RESULT_ITEM_NOT_OWNED_MESSAGE;
                break;
            default:
                errMsg = IAConstants.ACTIVITY_RESULT_CANCELED;
                break;
        }
        mNativeManagerListener.onProductPurchaseFailed(sku, errMsg);
    }

    public static class Listener {
        public native void onProductsLoaded(Object[] productIDs, long pointerToNativeListener);
        public native void onProductsLoadFailed(long pointerToNativeListener);
        public native void onProductPurchaseFailed(String productId, String error, long pointerToNativeListener);
        public native void onProductPurchased(String productId, long pointerToNativeListener);
        public native void onProductPurchaseCancelled(String productId, long pointerToNativeListener);
    }

    public class PurchaseManagerNativeListener {
        private final long mPointerToNativeListener;
        private Listener nativeListener = new Listener();

        public PurchaseManagerNativeListener(long nativeListenerPointer) {
            mPointerToNativeListener = nativeListenerPointer;
        }

        public void onProductsLoaded(final Object[] productIDs) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onProductsLoaded(productIDs, mPointerToNativeListener);
                }
            });
        }

        public void onProductsLoadFailed() {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onProductsLoadFailed(mPointerToNativeListener);
                }
            });
        }

        public void onProductPurchaseFailed(final String productId, final String error) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onProductPurchaseFailed(productId, error, mPointerToNativeListener);
                }
            });
        }

        public void onProductPurchased(final String productId) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onProductPurchased(productId, mPointerToNativeListener);
                }
            });
        }

        public void onProductPurchaseCancelled(final String productId) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onProductPurchaseCancelled(productId, mPointerToNativeListener);
                }
            });
        }
    }
}