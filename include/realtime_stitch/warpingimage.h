#ifndef WARPINGIMAGE_H
#define WARPINGIMAGE_H

#include <opencv2/opencv.hpp>
#include <vector>

struct WarpingResult {
    std::vector<cv::Point> corners;
    std::vector<cv::UMat> masks_warped;
    std::vector<cv::UMat> images_warped;
    std::vector<cv::Size> sizes;
    std::vector<cv::UMat> images_warped_f;
    float warped_image_scale;
    int num_images;
    cv::Ptr<cv::detail::RotationWarper> warper;
    cv::Ptr<cv::WarperCreator> warper_creator;
};

WarpingResult WarpImages(
    std::vector<cv::Mat>& images,
    std::vector<cv::detail::CameraParams>& cameras,
    float warped_image_scale,
    double seam_work_aspect,
    const std::string& warp_type,
    bool try_cuda,
    int num_images
);

#endif // WARPINGIMAGE_H