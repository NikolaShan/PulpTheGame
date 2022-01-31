package com.planemo.libs.purchases;

import android.content.Intent;

/**
 * Created by edward on 04/07/15.
 *
 */

public class IAConstants {
    public static final int GOOGLE_API_VERSION = 3;

    public static final String PRODUCT_TYPE_MANAGED = "inapp";
    public static final String PRODUCT_TYPE_SUBSCRIPTION = "subs";
    public static final String VENDING_BIND = "com.android.vending.billing.InAppBillingService.BIND";
    public static final String VENDING_PACKAGE = "com.android.vending";



    public static final int BILLING_RESPONSE_RESULT_OK = 0;
    public static final int BILLING_RESPONSE_RESULT_USER_CANCELED = 1;
    public static final int BILLING_RESPONSE_RESULT_SERVICE_UNAVAILABLE = 2;
    public static final int BILLING_RESPONSE_RESULT_BILLING_UNAVAILABLE = 3;
    public static final int BILLING_RESPONSE_RESULT_ITEM_UNAVAILABLE = 4;
    public static final int BILLING_RESPONSE_RESULT_DEVELOPER_ERROR = 5;
    public static final int BILLING_RESPONSE_RESULT_ERROR = 6;
    public static final int BILLING_RESPONSE_RESULT_ITEM_ALREADY_OWNED = 7;
    public static final int BILLING_RESPONSE_RESULT_ITEM_NOT_OWNED = 8;

    public static final String BILLING_RESPONSE_RESULT_OK_MESSAGE = "Success";
    public static final String BILLING_RESPONSE_RESULT_USER_CANCELED_MESSAGE = "User pressed back or canceled a dialog";
    public static final String BILLING_RESPONSE_RESULT_SERVICE_UNAVAILABLE_MESSAGE = "Network connection is down";
    public static final String BILLING_RESPONSE_RESULT_BILLING_UNAVAILABLE_MESSAGE = "Billing API version is not supported for the type requested";
    public static final String BILLING_RESPONSE_RESULT_ITEM_UNAVAILABLE_MESSAGE = "Requested product is not available for purchase";
    public static final String BILLING_RESPONSE_RESULT_DEVELOPER_ERROR_MESSAGE = "Invalid arguments provided to the API. This error can also indicate that the application was not correctly signed or properly set up for In-app Billing in Google Play, or does not have the necessary permissions in its manifest";
    public static final String BILLING_RESPONSE_RESULT_ERROR_MESSAGE = "Fatal error during the API action";
    public static final String BILLING_RESPONSE_RESULT_ITEM_ALREADY_OWNED_MESSAGE = "Failure to purchase since item is already owned";
    public static final String BILLING_RESPONSE_RESULT_ITEM_NOT_OWNED_MESSAGE = "Failure to consume since item is not owned";



    public static final String ACTIVITY_RESULT_CANCELED = "ACTIVITY_RESULT_CANCELED";
    public static final String RESPONSE_CODE = "RESPONSE_CODE";
    public static final String DETAILS_LIST = "DETAILS_LIST";
    public static final String ITEM_ID_LIST = "ITEM_ID_LIST";
    public static final String BUY_INTENT = "BUY_INTENT";
    public static final String INAPP_PURCHASE_DATA_LIST = "INAPP_PURCHASE_DATA_LIST";
    public static final String INAPP_PURCHASE_ITEM_LIST = "INAPP_PURCHASE_ITEM_LIST";
    public static final String INAPP_PURCHASE_DATA = "INAPP_PURCHASE_DATA";
    public static final String INAPP_DATA_SIGNATURE = "INAPP_DATA_SIGNATURE";
    public static final String INAPP_DATA_SIGNATURE_LIST = "INAPP_DATA_SIGNATURE_LIST";
    public static final String RESPONSE_ORDER_ID = "orderId";
    public static final String RESPONSE_PRODUCT_ID = "productId";
    public static final String RESPONSE_TYPE = "type";
    public static final String RESPONSE_TITLE = "title";
    public static final String RESPONSE_DESCRIPTION = "description";
    public static final String RESPONSE_PRICE = "price";
    public static final String RESPONSE_PRICE_CURRENCY = "price_currency_code";
    public static final String RESPONSE_PRICE_MICROS = "price_amount_micros";
    public static final String RESPONSE_PURCHASE_TOKEN = "purchaseToken";
    public static final String RESPONSE_PURCHASE_TIME = "purchaseTime";
    public static final String RESPONSE_DEVELOPER_PAYLOAD = "developerPayload";

    public static final int BILLING_ERROR_FAILED_LOAD_PURCHASES = 100;
    public static final int BILLING_ERROR_FAILED_TO_INITIALIZE_PURCHASE = 101;
    public static final int BILLING_ERROR_INVALID_SIGNATURE = 102;
    public static final int BILLING_ERROR_LOST_CONTEXT = 103;
    public static final int BILLING_ERROR_OTHER_ERROR = 110;
}