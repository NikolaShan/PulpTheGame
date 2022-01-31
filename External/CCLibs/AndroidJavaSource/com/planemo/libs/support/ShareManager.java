package com.planemo.libs.support;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Environment;
import com.planemo.libs.MultiplexerActivity;
import org.cocos2dx.lib.Cocos2dxHelper;

import java.io.*;

@SuppressWarnings("unused")
public class ShareManager {
    @SuppressWarnings("unused")
    private static final String TAG = ShareManager.class.getName();

    private String mImagePath;
    private String mText;
    private String mLinkUrl;

    public void setImageFilePath(String path) {
        mImagePath = path;
    }

    public void setText(String text) {
        mText = text;
    }

    public void setLinkUrl(String url) {
        mLinkUrl = url;
    }

    public void doShare() {
        Intent share = new Intent(android.content.Intent.ACTION_SEND);
        share.setType("text/plain");
        share.addFlags(Intent.FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET);

        if (mText != null) {
            share.putExtra(Intent.EXTRA_SUBJECT, mText);
        }

        if (mLinkUrl != null) {
            share.putExtra(Intent.EXTRA_TEXT, mLinkUrl);
        }

        if (mImagePath != null && !mImagePath.equals("")) {
            share.setType("image/*");
            share.putExtra(Intent.EXTRA_STREAM,getUriWithImageForIntentSharing(mImagePath));
        }
        MultiplexerActivity.getInstance().startActivity(share);
    }


    public static Uri getUriWithImageForIntentSharing(String imagePath){
        try {
            String pathTempImage = Environment.getExternalStorageDirectory() + File.separator + "share_img.jpg";
            BitmapFactory.Options options = new BitmapFactory.Options();
            options.inPreferredConfig = Bitmap.Config.ARGB_8888;
            Bitmap bitmap = BitmapFactory.decodeFile(imagePath, options);
            ByteArrayOutputStream bytes = new ByteArrayOutputStream();
            bitmap.compress(Bitmap.CompressFormat.JPEG, 100, bytes);

            File file = new File(pathTempImage);
            FileOutputStream fo = new FileOutputStream(file);
            fo.write(bytes.toByteArray());
            return Uri.fromFile(file);
        } catch (IOException e) {
            e.printStackTrace();
            return Uri.EMPTY;
        }
    }
}
