#pragma once

#include "Vision.h"

using namespace GreenTea;

extern "C"
{
    Vision* CreateVision();
    void FreeVision(Vision* ptr);

    void SetPreview1(Vision* ptr, uchar* preview1Data, int width, int height);
    void SetPreview2(Vision* ptr, uchar* preview2Data, int width, int height);
    void StitchPreview(Vision* ptr, uchar* outputData, int width, int height);
}

