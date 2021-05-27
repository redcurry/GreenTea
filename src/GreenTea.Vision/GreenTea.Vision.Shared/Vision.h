#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

namespace GreenTea
{
    class Vision
    {
    public:
        Vision();

        void SetPreview1(const Mat& preview1);
        void SetPreview2(const Mat& preview2);
        void StitchPreview(Mat& output);

        //void SetImage1(const Mat& image1);
        //void SetImage2(const Mat& image1);
        //void StitchImage(Mat& output);

    private:
        Ptr<AKAZE> feature2d;
        Ptr<BFMatcher> matcher;

        Mat mask;
        bool maskCreated = false;

        bool preview1Set = false;
        Mat originalPreview1;
        vector<KeyPoint> previewKeypoints1;
        Mat previewDescriptors1;
        bool preview1Detected = false;

        bool preview2Set = false;
        Mat originalPreview2;
        vector<KeyPoint> previewKeypoints2;
        Mat previewDescriptors2;

        vector<Point2f> corners1;
        vector<Point2f> corners2;

        Mat gray1;
        Mat gray2;
    };
}

