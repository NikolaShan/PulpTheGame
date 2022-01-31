package com.planemo.libs.ads;

import com.chartboost.sdk.ChartboostDelegate;
import com.chartboost.sdk.Model.CBError;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class ChartboostMultiplexerDelegate extends ChartboostDelegate {
    @SuppressWarnings("unused")
    private static final String TAG = ChartboostMultiplexerDelegate.class.getName();

    private static ChartboostMultiplexerDelegate sInstance = new ChartboostMultiplexerDelegate();
    private Iterator<ChartboostDelegate> mIterator;

    private ChartboostMultiplexerDelegate() {
    }

    public static ChartboostMultiplexerDelegate getInstance() {
        return sInstance;
    }


    ///
    /// Delegates
    ///

    private List<ChartboostDelegate> mDelegates = new ArrayList<>();

    public void addDelegate(ChartboostDelegate delegate) {
        if (!mDelegates.contains(delegate)) {
            mDelegates.add(delegate);
        }
    }

    public void removeDelegate(ChartboostDelegate delegate) {
        mDelegates.remove(delegate);
    }

    //to remove delegate while iterating
    public void removeCurrentDelegate() {
        if (mIterator != null) {
            mIterator.remove();
        }
    }

    ///
    /// Forwarding events
    ///

    @Override
    public boolean shouldRequestInterstitial(String location) {
        return true;
    }

    @Override
    public boolean shouldDisplayInterstitial(String location) {
        return true;
    }

    @Override
    public void didCacheInterstitial(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didCacheInterstitial(location);
        }
        mIterator = null;
    }

    @Override
    public void didFailToLoadInterstitial(String location, CBError.CBImpressionError error) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didFailToLoadInterstitial(location, error);
        }
        mIterator = null;
    }

    @Override
    public void didDismissInterstitial(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didDismissInterstitial(location);
        }
        mIterator = null;
    }

    @Override
    public void didCloseInterstitial(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didCloseInterstitial(location);
        }
        mIterator = null;
    }

    @Override
    public void didClickInterstitial(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didClickInterstitial(location);
        }
        mIterator = null;
    }

    @Override
    public void didDisplayInterstitial(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didDisplayInterstitial(location);
        }
        mIterator = null;
    }

    @Override
    public boolean shouldRequestMoreApps(String location) {
        return true;
    }

    @Override
    public void didCacheMoreApps(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didCacheMoreApps(location);
        }
        mIterator = null;
    }

    @Override
    public boolean shouldDisplayMoreApps(String location) {
        return true;
    }

    @Override
    public void didFailToLoadMoreApps(String location, CBError.CBImpressionError error) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didFailToLoadMoreApps(location, error);
        }
        mIterator = null;
    }

    @Override
    public void didDismissMoreApps(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didDismissMoreApps(location);
        }
        mIterator = null;
    }

    @Override
    public void didCloseMoreApps(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didCloseMoreApps(location);
        }
        mIterator = null;
    }

    @Override
    public void didClickMoreApps(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didClickMoreApps(location);
        }
        mIterator = null;
    }

    @Override
    public void didDisplayMoreApps(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didDisplayMoreApps(location);
        }
        mIterator = null;
    }

    @Override
    public void didFailToRecordClick(String uri, CBError.CBClickError error) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didFailToRecordClick(uri, error);
        }
        mIterator = null;
    }

    @Override
    public void didPauseClickForConfirmation() {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didPauseClickForConfirmation();
        }
        mIterator = null;
    }

    @Override
    public boolean shouldDisplayRewardedVideo(String location) {
        return true;
    }

    @Override
    public void didCacheRewardedVideo(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didCacheRewardedVideo(location);
        }
        mIterator = null;
    }

    @Override
    public void didFailToLoadRewardedVideo(String location, CBError.CBImpressionError error) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didFailToLoadRewardedVideo(location, error);
        }
        mIterator = null;
    }

    @Override
    public void didDismissRewardedVideo(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didDismissRewardedVideo(location);
        }
        mIterator = null;
    }

    @Override
    public void didCloseRewardedVideo(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didCloseRewardedVideo(location);
        }
        mIterator = null;
    }

    @Override
    public void didClickRewardedVideo(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didClickRewardedVideo(location);
        }
        mIterator = null;
    }

    @Override
    public void didCompleteRewardedVideo(String location, int reward) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didCompleteRewardedVideo(location, reward);
        }
        mIterator = null;
    }

    @Override
    public void didDisplayRewardedVideo(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didDisplayRewardedVideo(location);
        }
        mIterator = null;
    }

    @Override
    public void willDisplayVideo(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().willDisplayVideo(location);
        }
        mIterator = null;
    }

    @Override
    public void didCacheInPlay(String location) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didCacheInPlay(location);
        }
        mIterator = null;
    }

    @Override
    public void didFailToLoadInPlay(String location, CBError.CBImpressionError error) {
        mIterator = mDelegates.iterator();
        while (mIterator.hasNext()) {
            mIterator.next().didFailToLoadInPlay(location, error);
        }
        mIterator = null;
    }
}
