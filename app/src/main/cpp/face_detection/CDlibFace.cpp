//
// Created by hjimce on 16-7-28.
//

#include "CDlibFace.h"

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
