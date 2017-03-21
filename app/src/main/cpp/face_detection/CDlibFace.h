//
// Created by hjimce on 16-7-28.
//

#ifndef TINY_CNN_CDLIBFACE_H
#define TINY_CNN_CDLIBFACE_H

#include <vector>
#include <iostream>



class CDlibFace {
public:
    CDlibFace();
    ~CDlibFace();
    std::vector<int > getfacerect(const std::vector<int>img,int height,int width);
    void getfacelandmark(std::string imagefile);

private:




};


#endif //TINY_CNN_CDLIBFACE_H
