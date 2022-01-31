package com.planemo.libs.purchases;

/**
 * Created by edward on 04/07/15.
 *
 */
public class Product {

    public enum ProductType {CONSUMABLE, NON_CONSUMABLE}

    private String mSku;
    private String mDescription;
    private String mPrice;
    private String mCurrencyCode;
    private String mTitle;
    private double mPriceMicro;
    private ProductType mConsumeType;

    public String getSku() {
        return mSku;
    }

    public void setSku(String mSku) {
        this.mSku = mSku;
    }

    public String getDescription() {
        return mDescription;
    }

    public void setDescription(String mDescription) {
        this.mDescription = mDescription;
    }

    public String getPrice() {
        return mPrice;
    }

    public void setPrice(String mPrice) {
        this.mPrice = mPrice;
    }

    public String getCurrencyCode() {
        return mCurrencyCode;
    }

    public void setCurrencyCode(String mCurrencyCode) {
        this.mCurrencyCode = mCurrencyCode;
    }

    public String getTitle() {
        return mTitle;
    }

    public void setTitle(String mTitle) {
        this.mTitle = mTitle;
    }

    public void setConsumeType(ProductType type) {
        mConsumeType = type;
    }

    public ProductType getConsumeType() {
        return mConsumeType;
    }

    public void setPriceMicro(double price) {
        mPriceMicro = price;
    }

    public double getPriceMicro() {
        return mPriceMicro;
    }
}
