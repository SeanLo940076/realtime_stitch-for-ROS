#ifndef FILTERIMAGE_H
#define FILTERIMAGE_H

#include <vector>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/stitching/detail/matchers.hpp>
#include "opencv2/stitching/detail/motion_estimators.hpp"

struct FilteredImageResult {
    std::vector<int> indices;
    std::vector<cv::Mat> images;
    std::vector<cv::Size> full_img_sizes;
    int num_images;
};

FilteredImageResult FilterImages(
    std::vector<cv::detail::ImageFeatures>& features, 
    std::vector<cv::detail::MatchesInfo>& pairwise_matches, 
    float conf_thresh, 
    const std::vector<std::string>& img_names, 
    std::vector<cv::Mat>& images, 
    std::vector<cv::Size>& full_img_sizes
);

#endif // FILTERIMAGE_H
