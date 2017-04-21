//
// Created by hjimce on 17-4-18.
//

#include "mutil_task.h"
#include <vector>
#include <CFaceAttribute.h>

using namespace std;
mutil_task::mutil_task() {

}
mutil_task::~mutil_task() {

    for(int i=0;i<m_tasks.size();i++){
        delete m_tasks[i].net;
    }


}
//dlib第48~68号点，属于嘴巴部分点;如果使用的其它点序号库，需要修改
cv::Rect  mutil_task::getmouthrect(cv::Mat img,vector<cv::Point2i>landmarks, float scalex, float scaley)
{

    vector<cv::Point2i>mouth;
    for(int i=48;i<68;i++)
    {
        mouth.push_back(landmarks[i]);
    }
    cv::Rect mouthrect=getboundingRect(mouth);

    int left_miny = max(0, int(mouthrect.y - scaley*mouthrect.height));
    int left_minx = max(0, int(mouthrect.x - scalex*mouthrect.width));
    int left_maxy = min(img.rows, int(mouthrect.y + (1 + scaley)*mouthrect.height));
    int left_maxx = min(img.cols, int(mouthrect.x + (1 + scalex)*mouthrect.width));

    mouthrect=cv::Rect(left_minx, left_miny, left_maxx - left_minx, left_maxy - left_miny);


    return mouthrect;
}


//获取嘴巴rect
/*********************************************开口闭口识别*********************************/


cv::Rect mutil_task::getboundingRect(vector<cv::Point2i>pts)
{
    int minx=1e10;
    int maxx=0;
    int miny=1e10;
    int maxy=0;
    for(int i=0;i<pts.size();i++)
    {
        if (pts[i].x<minx){
            minx=pts[i].x;
        }
        if (pts[i].y<miny){
            miny=pts[i].y;
        }

        if (pts[i].x>maxx){
            maxx=pts[i].x;
        }

        if (pts[i].y>maxy){
            maxy=pts[i].y;
        }

    }

    return  cv::Rect(minx,miny,maxx-minx,maxy-miny);

}

void mutil_task::add_mouthtask(string model_file, string trained_file, string label_file,bool is_buffer) {

    TASK task;
    task.task_name="mouth";
    string meanfile;
    task.net= new CFaceAttribute(model_file, trained_file, meanfile, label_file,39,is_buffer);
    task.scalex=0.f;
    task.scaley=0.1f;
    task.image_size=45;

    m_tasks.push_back(task);

}
void mutil_task::add_lighttask(string model_file, string trained_file,
                               string mean_file, string label_file,bool is_buffer){
    TASK task;
    task.task_name="light";

    task.net= new CFaceAttribute(model_file, trained_file, mean_file, label_file,39,is_buffer);
    task.scalex=0.4f;
    task.scaley=0.4f;
    task.image_size=45;

    m_tasks.push_back(task);
    std::cout<<"load light"<<std::endl;
}
std::vector<std::vector<std::pair<string, float>>>
mutil_task::predict(const cv::Mat &image, const cv::Rect &face_rect,const std::vector<cv::Point2i>&landmarks) {

    std::vector<std::vector<std::pair<string, float>>>recognition;
    for(int i=0;i<m_tasks.size();i++){
        TASK *task=&m_tasks[i];
        cv::Rect rect;
        if(task->task_name=="mouth"){
            rect=getmouthrect(image,landmarks);
        }
        else if(m_tasks[i].task_name=="light"){
            rect=face_rect;
        }

        std::vector<std::pair<string, float>>result=task->net->predict(image,rect,task->scalex,task->scaley,task->image_size);
        recognition.push_back(result);


    }
    return recognition;
}