//
// Created by hjimce on 16-7-28.
//

#include "CDlibFace.h"
#include <dlib/image_io.h>
#include "dlib/image_processing.h"
#include "dlib/image_processing/frontal_face_detector.h"
#include "dlib/pixel.h"
#include "dlib/array2d.h"

#include <ctime>


dlib::frontal_face_detector m_facedetector;
dlib::shape_predictor m_facelandmarkdetector;
CDlibFace::CDlibFace() {
    m_facedetector= dlib::get_frontal_face_detector();

    //dlib::deserialize("dlib/shape_predictor_68_face_landmarks.dat") >> m_facelandmarkdetector;
}
CDlibFace::~CDlibFace() {

}



//获取人脸框
std::vector<int > CDlibFace::getfacerect(const std::vector<int>img,int height,int width)
{
    dlib::array2d<dlib::rgb_pixel>image;
    image.set_size(height,width);


    for (int i = 0; i < height; i++)
    {
        for(int j=0;j<width;j++)
        {
            int clr = img[i*width+j];
            int red = (clr & 0x00ff0000) >> 16; // 取高两位
            int green = (clr & 0x0000ff00) >> 8; // 取中两位
            int blue = clr & 0x000000ff; // 取低两位
            dlib::rgb_pixel pt(red,green,blue);
            image[i][j]=pt;
        }
    }



    clock_t begin = clock();
    std::vector<dlib::rectangle> dets= m_facedetector(image);


    std::vector<int >rect;
    if (!dets.empty())
    {
        rect.push_back(dets[0].left());
        rect.push_back(dets[0].top());
        rect.push_back(dets[0].width());
        rect.push_back(dets[0].height());
    }
    return rect;

}
//获取人脸特征点
/*void CDlibFace::getfacelandmark(std::string imagefile)
{
    dlib::array2d<unsigned char> img;
    dlib::load_image(img,imagefile);
    // dlib::pyramid_down(img);

    clock_t begin = clock();
    std::vector<dlib::rectangle> dets = m_facedetector(img);
    std::vector<cv::Point2i> pts;
    for (unsigned long j = 0; j < dets.size()&&j<1; ++j)
    {
        dlib::full_object_detection shape = m_facelandmarkdetector(img, dets[j]);
        for (int i = 0; i <shape.num_parts() ; ++i) {
            cv::Point2i pt(shape.part(i).x(),shape.part(i).y());
            pts.push_back(pt);

        }

    }



}*/
