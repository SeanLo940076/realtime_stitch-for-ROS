#ifndef ADJUSTER_H
#define ADJUSTER_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/stitching/detail/camera.hpp>
#include <opencv2/stitching/detail/matchers.hpp>

struct AdjusterResult {
    std::vector<cv::detail::ImageFeatures> features;
    std::vector<cv::detail::MatchesInfo> pairwise_matches;
    std::vector<cv::detail::CameraParams> cameras;
};

AdjusterResult CreateAndAdjust(
    const std::vector<cv::detail::ImageFeatures>& features,
    const std::vector<cv::detail::MatchesInfo>& pairwise_matches,
    const std::string& ba_cost_func,
    std::vector<cv::detail::CameraParams>& cameras,
    double conf_thresh,
    const std::string& ba_refine_mask
);

#endif // ADJUSTER_H
