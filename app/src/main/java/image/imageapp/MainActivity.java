package image.imageapp;

import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.net.Uri;
import android.provider.MediaStore;
import android.provider.Settings;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.graphics.Rect;
import android.graphics.Canvas;
import android.graphics.Paint;
//GPUimage库导入
import jp.co.cyberagent.android.gpuimage.GPUImage;
import jp.co.cyberagent.android.gpuimage.GPUImageSobelEdgeDetection;
//renderscript导入
import android.renderscript.RenderScript;

import java.util.Arrays;
import java.util.List;

import network.CNNdroid;



public class MainActivity extends AppCompatActivity {

    // c++链接库
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        // Example of a call to a native method

        Button buttonLoadImage = (Button) findViewById(R.id.button);
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

            //



            //读取图片，并显示到控件imageView
            ImageView imageView = (ImageView) findViewById(R.id.imageView);
            Bitmap scaled= BitmapFactory.decodeFile(picturePath).copy(Bitmap.Config.ARGB_8888, true);
            //int nh = (int) ( d.getHeight() * (512.0 / d.getWidth()) );//把图片缩放到可以在屏幕上显示
            //
    //××××××××××××××××××××1、GPUImage实验××××××××××××××××××//
            /*GPUImage mGPUImage = new GPUImage(this);
            //mGPUImage.setGLSurfaceView((GLSurfaceView) findViewById(R.id.imageView));//加入这句，程序会崩溃
            mGPUImage.setImage(scaled); //设置gpu要处理的图片
            mGPUImage.setFilter(new GPUImageSobelEdgeDetection());//设置滤波方法
            Bitmap resultimage = mGPUImage.getBitmapWithFilterApplied();//进行gpu图像处理，获取处理后的图片*/

    //××××××××××××××××××××2、cnn实验××××××××××××××××××//
            /*try {
                // 1) Create a Renderscript object
                RenderScript myRenderScript = RenderScript.create(this);

                // 2) Construct a CNNdroid object
                //	  and provide NetFile location address.
                //String NetFile = "/sdcard/1_def.txt";
                String NetFile = "/storage/emulated/0/develop/cnn/1_def.txt";
                CNNdroid myCNN = new CNNdroid(myRenderScript, NetFile);

                // 3) Prepare your input to the network.
                //		(The input can be single or batch of images.)
                float[][][]  inputSingle = new float [3][227][227];

                // 4) Call the Compute function of the CNNdroid library
                //    and get the result of the CNN execution as an Object
                //	  when the computation is finished.
                Object output = myCNN.compute(inputSingle);
                imageView.setImageBitmap(resultimage);//设置view控件中的显示内容
                TextView tv = (TextView) findViewById(R.id.sample_text);
                tv.setText(output.toString());



            } catch (Exception e) {
               // TextView tv = (TextView) findViewById(R.id.sample_text);
               // tv.setText(stringFromJNI(picturePath));
                //tv.setText(e.toString());
                e.printStackTrace();
            }*/

    //××××××××××××××××××××3、cnn实验××××××××××××××××××//
            int[] rect=face_detection(scaled);
            Canvas canvas=new Canvas(scaled);
            Paint p=new Paint();
            p.setColor(Color.RED);
            p.setStrokeWidth(3.0f);
            canvas.drawLine(rect[0], rect[1], rect[2], rect[1], p);//up
            canvas.drawLine(rect[0], rect[1], rect[0], rect[3], p);//left
            canvas.drawLine(rect[0], rect[3], rect[2], rect[3], p);//down
            canvas.drawLine(rect[2], rect[1], rect[2], rect[3], p);
            //cnvs.drawRect(rectangle,paint);
            imageView.setImageBitmap(scaled);












        }

    }
    private int[] face_detection(Bitmap origin_image){
        float scale = 240.f/ Math.max(origin_image.getHeight(), origin_image.getWidth());
        int width = (int)(origin_image.getWidth()*scale);
        int height = (int)(origin_image.getHeight()*scale);
        Bitmap resize_image=Bitmap.createScaledBitmap(origin_image,width,height , false);

        // 保存所有的像素的数组，图片宽×高
        int[] pixels = new int[width * height];

        resize_image.getPixels(pixels, 0, width, 0, 0, width, height);

        long startime= System.currentTimeMillis();
        int[] rect=stringFromJNI(pixels,height,width);
        int[] result_rect=new int[4];
        result_rect[0]=(int)(rect[0]/scale);
        result_rect[1]=(int)(rect[1]/scale);
        result_rect[2]=(int)(rect[2]/scale);
        result_rect[3]=(int)(rect[3]/scale);
        result_rect[2]=result_rect[2]+result_rect[0];
        result_rect[3]=result_rect[3]+result_rect[1];
        long endtime= System.currentTimeMillis();
        TextView tv = (TextView) findViewById(R.id.textView);
        tv.setText("耗时： "+(endtime-startime)+"ms");




        return  result_rect;
    }
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int[] stringFromJNI(int []image_data,int image_height,int image_widht);
    private static int RESULT_LOAD_IMAGE = 1;
}

