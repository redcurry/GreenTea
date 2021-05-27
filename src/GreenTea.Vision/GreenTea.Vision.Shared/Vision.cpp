#include "Vision.h"

using namespace std;
using namespace cv;

GreenTea::Vision::Vision()
{
    feature2d = AKAZE::create(AKAZE::DESCRIPTOR_MLDB, 0, 3, 0.001, 1, 1);
    matcher = BFMatcher::create(NORM_HAMMING);
}

void GreenTea::Vision::SetPreview1(const Mat& preview1)
{
    originalPreview1 = preview1;
    preview1Set = true;

    //cvtColor(preview1, gray1, COLOR_RGBA2GRAY);

    //goodFeaturesToTrack(gray1, corners1, 2000, 0.02, 10, noArray(), 5, false);

    /*
    feature2d->detectAndCompute(gray, noArray(), previewKeypoints1, previewDescriptors1);

    if (!maskCreated)
    {
        Mat mask_1ch;
        mask_1ch.create(preview1.size(), CV_8U);
        mask_1ch.setTo(Scalar::all(0));

        const auto right_edge = static_cast<int>(preview1.cols * 0.5);
        Mat mask_part_1(mask_1ch, Rect(0, 0, right_edge, preview1.rows));
        mask_part_1.setTo(Scalar::all(255));

        Mat mask_color;
        cvtColor(mask_1ch, mask_color, COLOR_GRAY2RGBA);

        mask_color.convertTo(mask, CV_32FC4);

        maskCreated = true;
    }

    preview1Set = true;
    */
}

void GreenTea::Vision::SetPreview2(const Mat& preview2)
{
    originalPreview2 = preview2;

    if (!preview1Detected)
    {
        cvtColor(originalPreview1, gray1, COLOR_RGBA2GRAY);
        goodFeaturesToTrack(gray1, corners1, 2000, 0.02, 10, noArray(), 5, false);
        preview1Detected = true;
    }

/*
    cvtColor(preview2, gray2, COLOR_RGBA2GRAY);
    feature2d->detectAndCompute(gray, noArray(), previewKeypoints2, previewDescriptors2);
*/
    preview2Set = true;
}

void GreenTea::Vision::StitchPreview(Mat& output)
{
    if (!preview2Set)
    {
        originalPreview1.copyTo(output);
        return;
    }

    cvtColor(originalPreview2, gray2, COLOR_RGBA2GRAY);

    vector<uchar> features_found;
    calcOpticalFlowPyrLK(gray1, gray2, corners1, corners2, features_found, noArray(), Size(10, 10), 5);

    vector<Point2f> src_pts, dst_pts;
    for (int i = 0; i < (int)corners1.size(); i++)
    {
        if (features_found[i])
        {
            src_pts.push_back(corners1[i]);
            dst_pts.push_back(corners2[i]);
        }
    }
/*
    vector<vector<DMatch>> matches;
    matcher->knnMatch(previewDescriptors1, previewDescriptors2, matches, 2);

    const float ratio_thresh = 0.75;
    vector<DMatch> good_matches;
    for (auto i = 0; i < matches.size(); i++)
    {
        if (matches[i][0].distance < ratio_thresh * matches[i][1].distance)
            good_matches.push_back(matches[i][0]);
    }

    // TODO: Few matches found
    if (good_matches.size() < 10)
    {
        cout << "Not enough matches" << endl;
        return;
    }

    vector<Point2f> src_pts, dst_pts;
    for (auto& good_match : good_matches)
    {
        src_pts.push_back(previewKeypoints1[good_match.queryIdx].pt);
        dst_pts.push_back(previewKeypoints2[good_match.trainIdx].pt);
    }
    */

    auto h = findHomography(dst_pts, src_pts, LMEDS, 5.0);

    warpPerspective(originalPreview2, output, h, Size(output.cols, output.rows));

    const auto right_edge = static_cast<int>(originalPreview1.cols * 0.5);
    Mat output_part_1(output, Rect(0, 0, right_edge, originalPreview1.rows));
    Mat preview_part_1(originalPreview1, Rect(0, 0, right_edge, originalPreview1.rows));

    preview_part_1.copyTo(output_part_1);

/*
    Mat output_f;
    output.convertTo(output_f, CV_32FC4);

    Mat preview1_f;
    originalPreview1.convertTo(preview1_f, CV_32FC4);

    Mat combined_f = output_f.mul(1.0 - mask / 255.0) + preview1_f.mul(mask / 255.0);
    combined_f.convertTo(output, CV_8UC4);
    */
}

/*
void GreenTea::Vision::SetImage1(const Mat& image1)
{
}

void GreenTea::Vision::SetImage2(const Mat& image1)
{
}

void GreenTea::Vision::StitchImage(Mat& output)
{
}
*/
