#ifndef IMAGESCALER_H
#define IMAGESCALER_H

#include <opencv2/core/core.hpp>
#include <opencv2/stitching/detail/camera.hpp>
#include "opencv2/stitching/warpers.hpp"
#include "opencv2/stitching/detail/motion_estimators.hpp"

struct ImageScalerResult {
    cv::Ptr<cv::detail::RotationWarper> warper;
    double compose_scale;
    std::vector<cv::detail::CameraParams> cameras;
    std::vector<cv::Point> corners;
    std::vector<cv::Size> sizes;
};

ImageScalerResult AdjustImageScalesAndCameras(
    cv::Ptr<cv::detail::RotationWarper> warper,
    double compose_megapix,
    cv::Mat full_img,
    double work_scale,
    float warped_image_scale,
    int num_images,
    cv::Ptr<cv::WarperCreator>& warper_creator,
    std::vector<cv::Size>& full_img_sizes,
    std::vector<cv::Point> corners,
    std::vector<cv::detail::CameraParams>& cameras,
    std::vector<cv::Size> sizes
);

#endif // IMAGESCALER_H
