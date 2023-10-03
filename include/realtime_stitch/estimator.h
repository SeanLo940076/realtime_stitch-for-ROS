#ifndef ESTIMATOR_H
#define ESTIMATOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/stitching/detail/matchers.hpp>
#include <opencv2/stitching/detail/motion_estimators.hpp>
#include <vector>
#include <string>

struct EstimatorResult {
    std::vector<cv::detail::CameraParams> cameras;
    std::vector<cv::detail::ImageFeatures> features;
    std::vector<cv::detail::MatchesInfo> pairwise_matches;
};

EstimatorResult EstimateCameras(
    const std::vector<cv::detail::ImageFeatures>& features,
    const std::vector<cv::detail::MatchesInfo>& pairwise_matches,
    const std::string& estimator_type
);

#endif // ESTIMATOR_H
