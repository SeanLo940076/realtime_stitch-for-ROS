#ifndef IMAGEBLENDER_H
#define IMAGEBLENDER_H

#include <opencv2/core/core.hpp>
#include <opencv2/stitching/detail/blenders.hpp>

struct BlenderImagesResult {
    cv::Ptr<cv::detail::Blender> blender;
};

BlenderImagesResult SetupImageBlender(
    cv::Ptr<cv::detail::Blender> blender,
    int blend_type,
    bool try_cuda,
    std::vector<cv::Point> corners,
    std::vector<cv::Size> sizes,
    float blend_strength
);

#endif // IMAGEBLENDER_H
