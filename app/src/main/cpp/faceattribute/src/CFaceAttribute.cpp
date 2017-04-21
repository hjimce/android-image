//
// Created by hjimce on 16-7-28.
//mxnet使用须知：mxnet再第一次构造网络的时候，可能并没有分配数据空间，所以第一个测试样本的前向传导时间将比第二个前向传导时间大很多
//
#include <iostream>
#include <fstream>
#include <ctime>
#include "CFaceAttribute.h"
#include "c_predict_api.h"

using namespace std;

class BufferFile {
public :
    std::string file_path_;
    int length_;
    char* buffer_;

    explicit BufferFile(std::string file_path)
            :file_path_(file_path) {

        std::ifstream ifs(file_path.c_str(), std::ios::in | std::ios::binary);
        if (!ifs) {
            std::cerr << "Can't open the file. Please check " << file_path << ". \n";
            assert(false);
        }

        ifs.seekg(0, std::ios::end);
        length_ = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::cout << file_path.c_str() << " ... "<< length_ << " bytes\n";

        buffer_ = new char[sizeof(char) * length_];
        ifs.read(buffer_, length_);
        ifs.close();
    }

    int GetLength() {
        return length_;
    }
    char* GetBuffer() {
        return buffer_;
    }

    ~BufferFile() {
        delete[] buffer_;
        buffer_ = NULL;
    }
};











CFaceAttribute::CFaceAttribute()
{
    m_network=NULL;

}

//image是输入图片，facerect是图片对应的人脸框
//cropsize是输入网络图片的大小，orisize是网络要采用random crop的时候，图片原始大小
//（scalex，scaley）是要裁剪人脸图片的时候，往外扩展的比例
//种族预测分类
CFaceAttribute::CFaceAttribute(const string& model_file,const string& trained_file,const string& mean_file,
                                const string& label_file,int cropsize,bool buffer)
{
    if(buffer)
    {
        m_network_input_size=cropsize;
        
        
        cv::FileStorage fs(mean_file, cv::FileStorage::MEMORY);
        fs["mean"] >> m_mean;
        cv::FileStorage fslabel(label_file, cv::FileStorage::MEMORY);
        std::vector<cv::String> attriblist;
        fslabel["attrib_list"]>>attriblist;
        for(int i=0;i<attriblist.size();i++)
        {
            m_labels.push_back(attriblist[i]);
        }
        // Parameters
        int dev_type = 1;  // 1: cpu, 2: gpu
        int dev_id = 0;  // arbitrary.
        mx_uint num_input_nodes = 1;  // 1 for feedforward
        const char* input_key[1] = {"data"};
        const char** input_keys = input_key;
        
        // Image size and channels
        int width = cropsize;
        int height = cropsize;
        int channels = 3;
        //( trained_width, trained_height, channel, num)
        const mx_uint input_shape_indptr[2] = { 0, 4 };
        const mx_uint input_shape_data[4] = { 1,
            static_cast<mx_uint>(channels),
            static_cast<mx_uint>(width),
            static_cast<mx_uint>(height) };
        
        //-- Create Predictor
        MXPredCreate((const char*)model_file.data(),
                     (const char*)trained_file.data(),
                     static_cast<size_t>(trained_file.length()),
                     dev_type,
                     dev_id,
                     num_input_nodes,
                     input_keys,
                     input_shape_indptr,
                     input_shape_data,
                     &m_network);
        
        
        //m_labels=get_label_list(label_file);
    }
    else
    {
        m_network_input_size=cropsize;
        
        if (!mean_file.empty()){
            cv::FileStorage fs(mean_file, cv::FileStorage::READ);
            fs["mean"] >> m_mean;
        }
        else{
            std::cout<<"mean file is empty.so image process will  no use mean file"<<std::endl;
        }



        /*cv::FileStorage fslabel(label_file, cv::FileStorage::MEMORY);
        std::vector<cv::String> attriblist;
        fslabel["attrib_list"]>>attriblist;
        for(int i=0;i<attriblist.size();i++)
        {
            m_labels.push_back(attriblist[i]);
        }*/
        m_labels=get_label_list(label_file);
        //
        BufferFile json_data(model_file);
        BufferFile param_data(trained_file);
        
        // Parameters
        int dev_type = 1;  // 1: cpu, 2: gpu
        int dev_id = 0;  // arbitrary.
        mx_uint num_input_nodes = 1;  // 1 for feedforward
        const char* input_key[1] = {"data"};
        const char** input_keys = input_key;
        
        // Image size and channels
        int width = cropsize;
        int height = cropsize;
        int channels = 3;
        //( trained_width, trained_height, channel, num)
        const mx_uint input_shape_indptr[2] = { 0, 4 };
        const mx_uint input_shape_data[4] = { 1,
            static_cast<mx_uint>(channels),
            static_cast<mx_uint>(width),
            static_cast<mx_uint>(height) };

        //-- Create Predictor
        MXPredCreate((const char*)json_data.GetBuffer(),
                     (const char*)param_data.GetBuffer(),
                     static_cast<size_t>(param_data.GetLength()),
                     dev_type,
                     dev_id,
                     num_input_nodes,
                     input_keys,
                     input_shape_indptr,
                     input_shape_data,
                     &m_network);

        MXPredForward(m_network);

        //m_labels=get_label_list(label_file);
    }


}

CFaceAttribute::~CFaceAttribute()
{
    MXPredFree(m_network);
    m_network=NULL;
}


CFaceAttribute::CFaceAttribute(const std::string& model_buffer,const std::string& trained_buffer,cv::Mat mean,
               std::vector<std::string>labels,int cropsize)
{
    m_network_input_size=cropsize;
    
    m_mean=mean.clone();
    //

    
    // Parameters
    int dev_type = 1;  // 1: cpu, 2: gpu
    int dev_id = 0;  // arbitrary.
    mx_uint num_input_nodes = 1;  // 1 for feedforward
    const char* input_key[1] = {"data"};
    const char** input_keys = input_key;
    
    // Image size and channels
    int width = cropsize;
    int height = cropsize;
    int channels = 3;
    //( trained_width, trained_height, channel, num)
    const mx_uint input_shape_indptr[2] = { 0, 4 };
    const mx_uint input_shape_data[4] = { 1,
        static_cast<mx_uint>(channels),
        static_cast<mx_uint>(width),
        static_cast<mx_uint>(height) };
    
    //-- Create Predictor
    MXPredCreate((const char*)model_buffer.data(),
                 (const char*)trained_buffer.data(),
                 static_cast<size_t>(trained_buffer.length()),
                 dev_type,
                 dev_id,
                 num_input_nodes,
                 input_keys,
                 input_shape_indptr,
                 input_shape_data,
                 &m_network);
    
    
    m_labels=labels;
}































































std::vector<std::pair<string, float>> CFaceAttribute::predict(cv::Mat image,cv::Rect facerect, float scalex,float scaley, int orisize)
{
    //std::cout<<"load model finish"<<std::endl;
    cv::Mat faceimage=Get_face(image,facerect,scalex,scaley);
    //cv::imshow("mouth.jpg",faceimage);
    //cv::imwrite("eye.jpg",faceimage);

    cv::Mat network_input_image=Std_net_image(faceimage,orisize, orisize,m_network_input_size , m_network_input_size);
    int channels = 3;
    int width = m_network_input_size;
    int height = m_network_input_size;
    int image_size=channels*width*height;



    std::vector<mx_float> input_channels = std::vector<mx_float>(image_size);
    preprocess(network_input_image, m_mean, 3, input_channels.data());
    //std::cout<<"process image finish"<<std::endl;

    MXPredSetInput(m_network, "data", input_channels.data(), image_size);
    MXPredForward(m_network);
    mx_uint output_index = 0;
    mx_uint *shape = 0;
    mx_uint shape_len;
    MXPredGetOutputShape(m_network, output_index, &shape, &shape_len);
    size_t size = 1;
    for (mx_uint i = 0; i < shape_len; ++i) size *= shape[i];
    std::vector<float> result(size);
    MXPredGetOutput(m_network, output_index, &(result[0]), size);
    //


    int top_n = m_labels.size();
    std::vector<std::pair<string, float>>outputs;
    for (int i = 0; i < top_n; i++) {
        outputs.push_back(std::pair<string, float>(m_labels[i],result[i]));
    }


    return outputs;
}

//人脸裁剪,输入为人脸框+图片
cv::Mat CFaceAttribute::Get_face(cv::Mat image,cv::Rect facerect, float scalex,float scaley)
{
    if (image.channels() !=3)
    {
        assert("***********when  predict ,image must be three channels************");
    }
    if(facerect.width<2||facerect.height<2||facerect.width>image.cols||facerect.height>image.rows||
       facerect.x<0||facerect.y<0||facerect.x>image.cols||facerect.y>image.rows)
    {
        assert("**********when  predict,  rect out of range*************");
    }
    //裁剪，并填充成正方形
    cv::Mat cropimage=CropFaceimage(image,facerect.x, facerect.x+facerect.width,facerect.y, facerect.y+facerect.height,scalex,scaley);
    return cropimage;
}
//裁剪出人脸框，同时会把裁剪出的图片，用背景填充成正方形
cv::Mat CFaceAttribute::CropFaceimage(cv::Mat img, int x1, int x2, int y1, int y2,float scalex,float scaley)
{

    int w = x2 - x1;
    int h = y2 - y1;

    int miny = max(0, int(y1 - scaley*h));
    int minx = max(0, int(x1 - scalex*w));
    int maxy = min(img.rows, int(y1 + (1 + scaley)*h));
    int maxx = min(img.cols, int(x1 + (1 + scalex)*w));
    cv::Mat roi = img(cv::Rect(minx, miny, maxx - minx, maxy - miny));

    int maxlenght = max(roi.rows, roi.cols);
    cv::Mat img0 = cv::Mat::zeros(maxlenght, maxlenght, CV_8UC3);


    int newx1 = maxlenght*.5 - roi.cols * .5;
    int newy1 = maxlenght*.5 - roi.rows * .5;
    int neww = (maxlenght*.5 + roi.cols * .5) - newx1;
    int newh = (maxlenght*.5 + roi.rows * .5) - newy1;
    cv::Rect temp(newx1, newy1, neww, newh);
    roi.copyTo(img0(temp));

    return img0;

}

//从图片image中心裁剪出(cropwidth,cropheight)大小的图片
cv::Mat CFaceAttribute::stdcrop(cv::Mat image, int cropwidth, int cropheight)
{
    assert(image.cols>cropwidth&&image.rows>cropheight);

    int offx = (image.cols - cropwidth) / 2.;
    int offy = (image.rows - cropheight) / 2.;
    if (offx >= 0 && offy >= 0)
    {
        cv::Rect crop(offx, offy, cropwidth, cropheight);
        return image(crop);
    }
    else
    {
        return cv::Mat(cv::Size(cropwidth, cropheight), image.type(), cv::mean(image));
    }

}


//裁剪，给定一张图片，先把它resize到(width,height)，然后再从中心进行裁剪(cropwidth,cropheight)大小的框
cv::Mat CFaceAttribute::Std_net_image(cv::Mat image,int width, int height,int cropwidth,int cropheigt)
{
    cv::Size geometry(width, height);
    // resize
    cv::Mat sample_resized;
    cv::resize(image, sample_resized, geometry);
    return stdcrop(sample_resized, cropwidth, cropheigt);
}

int CFaceAttribute::get_cvt_codes(int src_channels, int dst_channels)
{
    assert(src_channels != dst_channels);
    
    if (dst_channels == 3)
        return src_channels == 1 ? cv::COLOR_GRAY2BGR : cv::COLOR_BGRA2BGR;
    else if (dst_channels == 1)
        return src_channels == 3 ? cv::COLOR_BGR2GRAY : cv::COLOR_BGRA2GRAY;
    else
        throw runtime_error("unsupported color code");
}
//图片归一化处理image减去均值
void CFaceAttribute::preprocess(const cv::Mat& img,const cv::Mat& mean,int num_channels,mx_float* image_data)
{
    cv::Mat sample;
    if (img.channels() != num_channels)
        cv::cvtColor(img, sample, get_cvt_codes(img.channels(), num_channels));
    else
        sample = img;
    cv::Mat sample_float;
    sample.convertTo(sample_float, num_channels == 3 ? CV_32FC3 : CV_32FC1);



    int size = sample_float.rows * sample_float.cols * sample_float.channels();
    mx_float* ptr_image_r = image_data;
    mx_float* ptr_image_g = image_data + size / 3;
    mx_float* ptr_image_b = image_data + size / 3 * 2;

    if (mean.empty()){
        //std::cout<<"image process no use mean file"<<std::endl;
        for (int i = 0; i < sample_float.rows; i++) {
            float* datai = sample_float.ptr<float>(i);
            for (int j = 0; j < sample_float.cols; j++) {
                if (sample_float.channels() > 1)
                {
                    mx_float b = static_cast<mx_float>(*datai++);
                    mx_float g = static_cast<mx_float>(*datai++);
                    *ptr_image_g++ = g;
                    *ptr_image_b++ = b;
                }

                mx_float r = static_cast<mx_float>(*datai++);
                *ptr_image_r++ = r;

            }
        }
    }
    else{
        for (int i = 0; i < sample_float.rows; i++) {
            float* datai = sample_float.ptr<float>(i);
            const float* meani=mean.ptr<float>(i);

            for (int j = 0; j < sample_float.cols; j++) {
                if (sample_float.channels() > 1)
                {
                    mx_float b = static_cast<mx_float>((*datai++)-(*meani++));
                    mx_float g = static_cast<mx_float>((*datai++)-(*meani++));
                    *ptr_image_g++ = g;
                    *ptr_image_b++ = b;
                }

                mx_float r = static_cast<mx_float>((*datai++)-(*meani++));
                *ptr_image_r++ = r;

            }
        }

    }




}
//打开label列表文件
vector<string> CFaceAttribute::get_label_list(const string& label_file,bool buffer)
{
     vector<string> lines;
    if(buffer)
    {
        
    }
    else
    {
        string line;
        ifstream ifs(label_file.c_str());
        
        if (ifs.fail() || ifs.bad())
            throw runtime_error("failed to open:" + label_file);
        while (getline(ifs, line))
            lines.push_back(line);
    }
    return lines;
}
