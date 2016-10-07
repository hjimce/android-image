/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.example.hellojni;


import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;

public class HelloJni extends Activity {

    private static int RESULT_LOAD_IMAGE = 1;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_hello_jni);

        Button buttonLoadImage = (Button) findViewById(R.id.buttonLoadPicture);
        buttonLoadImage.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {
                Intent intent =  new  Intent(Intent.ACTION_PICK,
                        android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
                intent.setDataAndType(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, "image/*");
                startActivityForResult(intent,  RESULT_LOAD_IMAGE );
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == RESULT_LOAD_IMAGE && resultCode == RESULT_OK && null != data) {
            Uri uri = data.getData();
            String picturePath;
            if (!TextUtils.isEmpty(uri.getAuthority())) {
                Cursor cursor = getContentResolver().query(uri,
                        new String[] { MediaStore.Images.Media.DATA },null, null, null);
                if (null == cursor) {
                    Toast.makeText(this, "图片没找到", Toast.LENGTH_SHORT).show();
                    return;
                }
                cursor.moveToFirst();
                picturePath = cursor.getString(cursor.getColumnIndex(MediaStore.Images.Media.DATA));
                cursor.close();
            } else {
                picturePath = uri.getPath();
            }

            //读取图片，并显示到控件imageView
            ImageView imageView = (ImageView) findViewById(R.id.imgView);
            Bitmap d=BitmapFactory.decodeFile(picturePath);
            int nh = (int) ( d.getHeight() * (512.0 / d.getWidth()) );
            Bitmap scaled = Bitmap.createScaledBitmap(d, 512, nh, true);

            imageView.setImageBitmap(scaled);

            //final TextView text = (TextView) findViewById(R.id.textView);
            //text.setText(picturePath);


        }

    }
}
