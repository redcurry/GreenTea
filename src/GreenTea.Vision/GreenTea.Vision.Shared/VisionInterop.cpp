#include "VisionInterop.h"

using namespace std;

Vision* CreateVision()
{
    return new Vision();
}

void FreeVision(Vision* ptr)
{
    delete ptr;
}

void SetPreview1(Vision* ptr, uchar* preview1Data, int width, int height)
{
    const Mat preview1(height, width, CV_8UC4, preview1Data);
    ptr->SetPreview1(preview1);
}

void SetPreview2(Vision* ptr, uchar* preview2Data, int width, int height)
{
    const Mat preview2(height, width, CV_8UC4, preview2Data);
    ptr->SetPreview2(preview2);
}

void StitchPreview(Vision* ptr, uchar* outputData, int width, int height)
{
    Mat output(height, width, CV_8UC4, outputData);
    ptr->StitchPreview(output);
}
