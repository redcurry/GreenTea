#pragma once

#include <opencv2/opencv.hpp>

using namespace cv;

namespace GreenTea
{
    class Vision
    {
    public:
        void SetFirstImage(const Mat& image);
        void SetSecondImage(const Mat& image);

    private:
        Mat firstImage;
        Mat secondImage;
    };
}

