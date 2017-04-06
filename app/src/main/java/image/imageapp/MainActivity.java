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
import messagepack.ParamUnpacker;
//GPUimage库导入
import jp.co.cyberagent.android.gpuimage.GPUImage;
import jp.co.cyberagent.android.gpuimage.GPUImageSobelEdgeDetection;
//renderscript导入
import android.renderscript.RenderScript;

import java.util.Arrays;
import java.util.List;

import network.CNNdroid;

import static android.R.id.message;


public class MainActivity extends AppCompatActivity {

    // c++链接库
    static {
        System.loadLibrary("face_detection_static");
        //System.loadLibrary("nnpack-lib");
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
            int[]face_rect=face_detection(scaled);

            //cnn_predict(scaled,face_rect);







    //××××××××××××××××××××3、人脸检测实验××××××××××××××××××//
            /*int[] rect=face_detection(scaled);
            Canvas canvas=new Canvas(scaled);
            Paint p=new Paint();
            p.setColor(Color.RED);
            p.setStrokeWidth(3.0f);
            canvas.drawLine(rect[0], rect[1], rect[2]+rect[0], rect[1], p);//up
            canvas.drawLine(rect[0], rect[1], rect[0], rect[3]+rect[1], p);//left
            canvas.drawLine(rect[0], rect[3]+rect[1], rect[2]+rect[0], rect[3]+rect[1], p);//down
            canvas.drawLine(rect[2]+rect[0], rect[1], rect[2]+rect[0], rect[3]+rect[1], p);
            //cnvs.drawRect(rectangle,paint);
            imageView.setImageBitmap(scaled);*/












        }

    }
    private int[] cnn_predict(Bitmap origin_image,int []face_rect){
        TextView tv = (TextView) findViewById(R.id.textView);
        Bitmap faceBmp = Bitmap.createBitmap(origin_image,face_rect[0], face_rect[1], face_rect[2], face_rect[3]);
        Bitmap resize_image=Bitmap.createScaledBitmap(faceBmp,39,39 , false);
        String mean_file="/sdcard/model_mean.msg";
        //String mean_file="/storage/emulated/0/develop/cnn/paras/model_mean.msg";
        int size=39;

        float[][][]image_data=get_data(resize_image,mean_file,size);
        try {



            RenderScript myRenderScript = RenderScript.create(this);
            String NetFile = "/sdcard/androidcnn.txt";
            //String NetFile = "/storage/emulated/0/develop/cnn/androidcnn.txt";

            CNNdroid myCNN = new CNNdroid(myRenderScript, NetFile);

            long startime= System.currentTimeMillis();
            Object output = myCNN.compute(image_data);
            float[][]myreslut=(float[][])output;
            float[] temp=myreslut[0];
            int maxindex=maxIndex(temp,6);
            long endtime= System.currentTimeMillis();
            tv.setText("结果："+maxindex+"  耗时： "+(endtime-startime)+"ms");

            ImageView imageView = (ImageView) findViewById(R.id.imageView);
            imageView.setImageBitmap(origin_image);//设置view控件中的显示内容




        } catch (Exception e) {

            tv.setText(e.toString());
            e.printStackTrace();
        }


        return  face_rect;

    }
    public int maxIndex(float[]list,int lenght) {
        float maxvalue=0;
        int maxIndex=-1;
        for (int i=0;i<lenght;i++) {
            if (list[i]>maxvalue) {
                maxvalue = list[i];
                maxIndex = i;
            }
        }
        return maxIndex;
    }
    private float[][][] get_data(Bitmap origin_image,String mean_file,int size){

        float [][][] data=new float[3][size][size];

        ParamUnpacker paramUnpacker=new ParamUnpacker();
        Object objects = paramUnpacker.unpackerFunction(mean_file, float[][][][].class);
        float[][][][]mean=(float[][][][])objects;
        int width = (int)(origin_image.getWidth());
        int height = (int)(origin_image.getHeight());
        assert width==size;
        // 保存所有的像素的数组，图片宽×高
        int[] pixels = new int[width * height];
        origin_image.getPixels(pixels, 0, width, 0, 0, width, height);
        for (int i = 0; i < height; i++)
        {
            for(int j=0;j<width;j++)
            {
                int clr = pixels[i*width+j];
                int red =(clr & 0x00ff0000) >> 16; // 取高两位
                int green =(clr & 0x0000ff00) >> 8; // 取中两位
                int blue =clr & 0x000000ff; // 取低两位
                data[0][i][j]=red-mean[0][0][i][j];
                data[1][i][j]=green-mean[0][1][i][j];
                data[2][i][j]=blue-mean[0][2][i][j];
            }
        }
        return data;
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

        int[] rect=facedetection(pixels,height,width);
        //tv.setText(nnpack_time[0]);
        int[] result_rect=new int[4];
        result_rect[0]=(int)(rect[0]/scale);
        result_rect[1]=(int)(rect[1]/scale);
        result_rect[2]=(int)(rect[2]/scale);
        result_rect[3]=(int)(rect[3]/scale);
        long endtime= System.currentTimeMillis();
        TextView tv = (TextView) findViewById(R.id.textView);
        tv.setText("耗时： "+(endtime-startime)+"ms");



        startime= System.currentTimeMillis();
        float tms=nnpack();
        endtime= System.currentTimeMillis();
        tv = (TextView) findViewById(R.id.textView);
        tv.setText("nnpack： "+tms+"ms");




        return  result_rect;
    }
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int[] facedetection(int []image_data,int image_height,int image_widht);
    public native float nnpack();

    private static int RESULT_LOAD_IMAGE = 1;
}

