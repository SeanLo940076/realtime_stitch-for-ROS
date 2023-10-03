#include "filterimage.h"
#include <iostream>
#include <opencv2/stitching/detail/util.hpp>
#include "opencv2/stitching/detail/motion_estimators.hpp"

FilteredImageResult FilterImages(
    std::vector<cv::detail::ImageFeatures>& features, 
    std::vector<cv::detail::MatchesInfo>& pairwise_matches, 
    float conf_thresh, 
    const std::vector<std::string>& img_names, 
    std::vector<cv::Mat>& images, 
    std::vector<cv::Size>& full_img_sizes
) {
    // Leave only images we are sure are from the same panorama 保留數量最多的一組成為全景圖
    std::vector<int> indices = cv::detail::leaveBiggestComponent(features, pairwise_matches, conf_thresh);
    std::vector<cv::Mat> img_subset;
    std::vector<std::string> img_names_subset;
    std::vector<cv::Size> full_img_sizes_subset;
    int num_images;
    for (size_t i = 0; i < indices.size(); ++i)
    {
        img_names_subset.push_back(img_names[indices[i]]);
        img_subset.push_back(images[indices[i]]);
        full_img_sizes_subset.push_back(full_img_sizes[indices[i]]);
    }

    num_images = static_cast<int>(img_names.size());

    // Check if we still have enough images
    if (num_images < 2)
    {
        std::cout << "Need more images" << std::endl; 
    }

    FilteredImageResult result;

    result.indices = indices;
    result.images = img_subset;
    result.full_img_sizes = full_img_sizes_subset;
    result.num_images = num_images;
    return result;
}
