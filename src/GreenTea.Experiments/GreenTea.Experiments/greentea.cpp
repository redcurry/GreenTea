#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>

using namespace std;
using namespace chrono;
using namespace cv;
using namespace detail;

// Used for timing
steady_clock::time_point start_time;
steady_clock::time_point end_time;
duration<double> elapsed;

Mat create_final_image(const Mat& image_1, const Mat& image_2, const vector<Point2f>& keypoints_1, const vector<Point2f>& keypoints_2);

int feature_match(int argc, char** argv)
{
    const int n_req_params = 2;
    const double resize_scale = 0.2;
    const int n_min_matches = 10;

    if (argc < n_req_params + 1)
    {
        cout << "Needs parameters" << endl;
        return EXIT_FAILURE;
    }

    const string path_1 = string(argv[1]);
    const string path_2 = string(argv[2]);

    // Load images
    Mat image_1_full = imread(path_1, IMREAD_COLOR);
    Mat image_2_full = imread(path_2, IMREAD_COLOR);

    cout << "Original image size = " << image_1_full.size() << endl;

    // Resize for performance
    Mat image_1, image_2;
    resize(image_1_full, image_1, Size(), resize_scale, resize_scale, INTER_AREA);
    resize(image_2_full, image_2, Size(), resize_scale, resize_scale, INTER_AREA);

    cout << "Resized image size = " << image_1.size() << endl;

    // Convert to grayscale
    Mat image_1_gy, image_2_gy;
    cvtColor(image_1, image_1_gy, COLOR_BGR2GRAY);
    cvtColor(image_2, image_2_gy, COLOR_BGR2GRAY);

    start_time = high_resolution_clock::now();

    auto feature2d = cv::AKAZE::create(AKAZE::DESCRIPTOR_MLDB, 0, 3, 0.002, 1, 1);
    vector<KeyPoint> keypoints_1, keypoints_2;
    Mat descr_1, descr_2;
    feature2d->detectAndCompute(image_1_gy, noArray(), keypoints_1, descr_1);
    feature2d->detectAndCompute(image_2_gy, noArray(), keypoints_2, descr_2);

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "detectAndComute: " << elapsed.count() << endl;

    cout << "Number of keypoints 1 = " << keypoints_1.size() << ", and keypoints 2 = " << keypoints_2.size() << endl;

    start_time = high_resolution_clock::now();

    auto matcher = cv::BFMatcher::create(NORM_HAMMING);

    // knn matches
    vector<vector<DMatch>> matches;
    matcher->knnMatch(descr_1, descr_2, matches, 2);

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "match: " << elapsed.count() << endl;

    cout << "Number of matches = " << matches.size() << endl;

    // Filter knn matches
    const float ratio_thresh = 0.75;
    vector<DMatch> good_matches;
    for (int i = 0; i < matches.size(); i++)
    {
        if (matches[i][0].distance < ratio_thresh * matches[i][1].distance)
            good_matches.push_back(matches[i][0]);
    }

    cout << "Number of good matches = " << good_matches.size() << endl;

///*
    // Draw matches
    Mat out_matches;
    drawMatches(image_1, keypoints_1, image_2, keypoints_2, good_matches, out_matches);
    imshow("matches", out_matches);
    waitKey(0);
//*/

    if (good_matches.size() < n_min_matches)
    {
        cout << "Not a good match, only " << good_matches.size() << " feature matches were found." << endl;
        return EXIT_SUCCESS;
    }

    vector<Point2f> src_pts, dst_pts;

    for (auto& good_match : good_matches)
    {
        src_pts.push_back(keypoints_1[good_match.queryIdx].pt);
        dst_pts.push_back(keypoints_2[good_match.trainIdx].pt);
    }

    vector<Point2f> keypoints_1_full, keypoints_2_full;
    for (auto& k_1 : src_pts)
        keypoints_1_full.push_back(k_1 / resize_scale);
    for (auto& k_2 : dst_pts)
        keypoints_2_full.push_back(k_2 / resize_scale);

    start_time = high_resolution_clock::now();

    Mat final_image = create_final_image(image_1_full, image_2_full, keypoints_1_full, keypoints_2_full);

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "create_final_image: " << elapsed.count() << endl;

    imwrite("final.jpg", final_image);

    return EXIT_SUCCESS;
}

Mat create_final_image(const Mat& image_1, const Mat& image_2, const vector<Point2f>& keypoints_1, const vector<Point2f>& keypoints_2)
{
    start_time = high_resolution_clock::now();

    Mat h = findHomography(keypoints_2, keypoints_1, LMEDS, 5.0);

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "findHomography: " << elapsed.count() << endl;

    start_time = high_resolution_clock::now();

    Mat final_image;
    warpPerspective(image_2, final_image, h, Size(image_1.cols, image_1.rows));

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "warpPerspetive: " << elapsed.count() << endl;

    Mat mask;
    mask.create(image_1.size(), CV_8U);
    mask.setTo(Scalar::all(0));

    const auto right_edge = static_cast<int>(image_1.cols * 0.5);
    Mat mask_part_1(mask, Rect(0, 0, right_edge, image_1.rows));
    mask_part_1.setTo(Scalar::all(255));

    start_time = high_resolution_clock::now();

    Mat mask_blurred;
    GaussianBlur(mask, mask_blurred, Size(81, 81), 0);

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "GaussianBlur: " << elapsed.count() << endl;

    start_time = high_resolution_clock::now();

    Mat mask_3chan;
    cvtColor(mask_blurred, mask_3chan, COLOR_GRAY2BGR);

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "cvtColor: " << elapsed.count() << endl;

    start_time = high_resolution_clock::now();

    Mat mask_3chan_f;
    mask_3chan.convertTo(mask_3chan_f, CV_32FC3);

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "mask convertTo: " << elapsed.count() << endl;

    start_time = high_resolution_clock::now();

    Mat final_f;
    final_image.convertTo(final_f, CV_32FC3);

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "final_image convertTo: " << elapsed.count() << endl;

    start_time = high_resolution_clock::now();

    Mat image_1_f;
    image_1.convertTo(image_1_f, CV_32FC3);

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "image_1_f convertTo: " << elapsed.count() << endl;

    start_time = high_resolution_clock::now();

    Mat combined_f = final_f.mul(1.0 - mask_3chan_f / 255.0) + image_1_f.mul(mask_3chan_f / 255.0);
    Mat combined;
    combined_f.convertTo(combined, CV_8UC3);

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "combined calc and convertTo: " << elapsed.count() << endl;

    //Mat combined = final_image.mul(1.0 - mask_3chan / 255.0) + image_1.mul(mask_3chan / 255.0);

    return combined;

//    const auto right_edge = static_cast<int>(image_1.cols * 0.55);
    Mat part_1(final_image, Rect(0, 0, right_edge, image_1.rows));
    image_1(Rect(0, 0, right_edge, image_1.rows)).copyTo(part_1);

    return final_image;
}

int optical_flow(int argc, char** argv)
{
    const int n_req_params = 2;
    const double scale = 0.2;

    if (argc < n_req_params + 1)
    {
        cout << "Needs parameters" << endl;
        return EXIT_FAILURE;
    }

    const string path_1 = string(argv[1]);
    const string path_2 = string(argv[2]);

    // Load images
    Mat image_1_full = imread(path_1, IMREAD_COLOR);
    Mat image_2_full = imread(path_2, IMREAD_COLOR);

    // Pre-compute size-dependent parameters
    double avgSize = (image_1_full.size().width + image_1_full.size().height) / 2.0;
    double minDistance = (15.0 / 560.0) * avgSize * scale;
    int blockRadius = (int)((2.0 / 560.0) * avgSize * scale);
    int blockSize = 2 * blockRadius + 1;
    int winSize = (int)((10.0 / 560.0) * avgSize * scale);
    int levels = (int)((5.0 / 560.0) * avgSize * scale);
    double farCornerDistance = (200.0 / 560.0) * avgSize * scale;

    cout << "avgSize=" << avgSize << endl;
    cout << "minDistance=" << minDistance << endl;
    cout << "blockRadius=" << blockRadius << endl;
    cout << "blockSize=" << blockSize << endl;
    cout << "levels=" << levels << endl;
    cout << "farCornerDistance=" << farCornerDistance << endl;

    // Resize for performance
    Mat image_1, image_2;
    resize(image_1_full, image_1, Size(), scale, scale, INTER_AREA);
    resize(image_2_full, image_2, Size(), scale, scale, INTER_AREA);

    // Convert to grayscale
    Mat image_1_gy, image_2_gy;
    cvtColor(image_1, image_1_gy, COLOR_BGR2GRAY);
    cvtColor(image_2, image_2_gy, COLOR_BGR2GRAY);

    start_time = high_resolution_clock::now();

    vector<Point2f> corners_1, corners_2;
    goodFeaturesToTrack(image_1_gy, corners_1, 5000, 0.02, minDistance, noArray(), blockSize, false, 0.04);

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "goodFeaturesToTrack: " << elapsed.count() << endl;

    start_time = high_resolution_clock::now();

    vector<uchar> features_found;
    calcOpticalFlowPyrLK(image_1_gy, image_2_gy, corners_1, corners_2,
        features_found, noArray(), Size(winSize, winSize), levels);

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "calcOpticalFlow: " << elapsed.count() << endl;

    vector<Point2f> keypoints_1, keypoints_2;

    Mat line_image = image_1.clone();

    vector<double> corner_dists;
    for (int i = 0; i < (int)corners_1.size(); i++)
        corner_dists.push_back(norm(corners_1[i] - corners_2[i]));
    auto corner_dist_avg = mean(corner_dists);
    cout << "corner_dist_avg=" << corner_dist_avg << endl;

    for (int i = 0; i < (int)corners_1.size(); i++)
    {
        if (!features_found[i])
            continue;

        if (cv::norm(corners_1[i] - corners_2[i]) > corner_dist_avg[0] * 1.2)
            continue;

        keypoints_1.push_back(corners_1[i]);
        keypoints_2.push_back(corners_2[i]);

        cv::line(line_image, corners_1[i], corners_2[i], cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
    }

    imshow("lines", line_image);
    waitKey(0);

    //Mat h = findHomography(keypoints_2, keypoints_1, RANSAC, 5.0);

    vector<Point2f> keypoints_1_full, keypoints_2_full;
    for (auto k_1 : keypoints_1)
        keypoints_1_full.push_back(k_1 / scale);
    for (auto k_2 : keypoints_2)
        keypoints_2_full.push_back(k_2 / scale);

    start_time = high_resolution_clock::now();

    Mat final_image = create_final_image(image_1_full, image_2_full, keypoints_1_full, keypoints_2_full);

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "create_final_image: " << elapsed.count() << endl;

    imwrite("final.jpg", final_image);

    return EXIT_SUCCESS;
}

// See https://docs.opencv.org/master/d9/dd8/samples_2cpp_2stitching_detailed_8cpp-example.html
int stitching(int argc, char** argv)
{
    const int n_req_params = 2;
    const double resize_scale = 0.3;

    if (argc < n_req_params + 1)
    {
        cout << "Needs parameters" << endl;
        return EXIT_FAILURE;
    }

    const string path_1 = string(argv[1]);
    const string path_2 = string(argv[2]);

    // Load images
    Mat image_1 = imread(path_1, IMREAD_COLOR);
    Mat image_2 = imread(path_2, IMREAD_COLOR);

    // Resize for performance
    resize(image_1, image_1, Size(), resize_scale, resize_scale);
    resize(image_2, image_2, Size(), resize_scale, resize_scale);

    // Convert to grayscale
    Mat image_1_gy;
    cvtColor(image_1, image_1_gy, COLOR_BGR2GRAY);
    Mat image_2_gy;
    cvtColor(image_2, image_2_gy, COLOR_BGR2GRAY);

    // Define the keypoint finder
    //auto featureFinder = ORB::create();
    auto featureFinder = BRISK::create(20);

    start_time = high_resolution_clock::now();

    ImageFeatures features_1, features_2;
    computeImageFeatures(featureFinder, image_1_gy, features_1);
    computeImageFeatures(featureFinder, image_2_gy, features_2);

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "compute features: " << elapsed.count() << endl;

///*
    // Show keypoints found
    cv::Mat out_image_1;
    cv::drawKeypoints(image_1, features_1.keypoints, out_image_1);
    cv::imshow("keypoints1", out_image_1);
    cv::Mat out_image_2;
    cv::drawKeypoints(image_2, features_2.keypoints, out_image_2);
    cv::imshow("keypoints2", out_image_2);
    cv::waitKey(0);
//*/

    auto features = vector<ImageFeatures>{ features_1, features_2 };

    start_time = high_resolution_clock::now();

    BestOf2NearestMatcher matcher;
    vector<MatchesInfo> pairwise_matches;
    matcher(features, pairwise_matches);

    end_time = high_resolution_clock::now();
    elapsed = end_time - start_time;
    cout << "matching: " << elapsed.count() << endl;

    cout << "number of pairwise matches = " << pairwise_matches.size() << endl;

    HomographyBasedEstimator estimator;
    vector<CameraParams> cameras;
    auto success = estimator(features, pairwise_matches, cameras);
    if (!success)
    {
        cout << "Homography estimation failed" << endl;
        return EXIT_FAILURE;
    }

    Mat mask_1, mask_2;
    mask_1.create(image_1_gy.size(), CV_8U);
    mask_1.setTo(Scalar::all(255));
    mask_2.create(image_2_gy.size(), CV_8U);
    mask_2.setTo(Scalar::all(255));

    auto warper_creator = makePtr<cv::CylindricalWarper>();
    auto warper = warper_creator->create(1000.0);

    UMat mask_1_warped, mask_2_warped;
    UMat image_1_warped, image_2_warped;
    Size size_1, size_2;

    Mat K_1, K_2;
    cameras[0].K().convertTo(K_1, CV_32F);
    cameras[1].K().convertTo(K_2, CV_32F);

    Mat R_1, R_2;
    cameras[0].R.convertTo(R_1, CV_32F);
    cameras[1].R.convertTo(R_2, CV_32F);

    auto corner_1 = warper->warp(image_1_gy, K_1, R_1, INTER_NEAREST, BORDER_CONSTANT, image_1_warped);
    auto corner_2 = warper->warp(image_2_gy, K_2, R_2, INTER_NEAREST, BORDER_CONSTANT, image_2_warped);

    size_1 = image_1_warped.size();
    size_2 = image_2_warped.size();

    cout << size_1.width << "x" << size_1.height << endl;

    warper->warp(mask_1, K_1, R_1, INTER_NEAREST, BORDER_CONSTANT, mask_1_warped);
    warper->warp(mask_2, K_2, R_2, INTER_NEAREST, BORDER_CONSTANT, mask_2_warped);

/*
    // Show warped images
    cv::imshow("warped1", image_1_warped);
    cv::imshow("warped2", image_2_warped);
    cv::waitKey(0);
*/
/*
    auto compensator = ExposureCompensator::createDefault(ExposureCompensator::GAIN_BLOCKS); // try other types

    compensator->feed(vector<Point>{corner_1, corner_2},
        vector<UMat>{image_1_warped, image_2_warped}, vector<UMat>{mask_1_warped, mask_2_warped});
    compensator->apply(0, corner_1, image_1_warped, mask_1_warped);
    compensator->apply(0, corner_1, image_2_warped, mask_2_warped);
*/
    // Show compensated images
    namedWindow("warped1", cv::WINDOW_NORMAL);
    namedWindow("warped2", cv::WINDOW_NORMAL);
    cv::imshow("warped1", image_1_warped);
    cv::imshow("warped2", image_2_warped);
    cv::waitKey(0);

    return EXIT_SUCCESS;
}

int auto_stitching(int argc, char** argv)
{
    const int n_req_params = 2;

    if (argc < n_req_params + 1)
    {
        cout << "Needs parameters" << endl;
        return EXIT_FAILURE;
    }

    const string path_1 = string(argv[1]);
    const string path_2 = string(argv[2]);

    // Load images
    Mat image_1 = imread(path_1, IMREAD_COLOR);
    Mat image_2 = imread(path_2, IMREAD_COLOR);

    vector<Mat> images;
    images.push_back(image_1);
    images.push_back(image_2);

    Mat pano;
    auto stitcher = Stitcher::create(Stitcher::PANORAMA);
    auto status = stitcher->stitch(images, pano);

    if (status != Stitcher::OK)
    {
        cout << "Unable to stitch, error code = " << int(status) << endl;
        return EXIT_FAILURE;
    }

    imwrite("pano.jpg", pano);
    return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    //return feature_match(argc, argv);
    return optical_flow(argc, argv);
    //return stitching(argc, argv);
    //return auto_stitching(argc, argv);
}

// Notes:
// - warping
//   - many kinds of warping; try which is fastest
//     - https://stackoverflow.com/questions/25264009/difference-between-different-warp-methods-in-opencv
