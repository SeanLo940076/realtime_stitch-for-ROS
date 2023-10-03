#include "stitchingloop.h"
#include "imagescaler.h"
#include "imageblender.h"

std::mutex warperMutex;
std::mutex compensatorMutex;
std::mutex blenderMutex;  // If the blender is not thread-safe
std::mutex mtx;

StitchingLoopResult processor(
    int num_images,
    cv::Mat frame[6],
    const std::vector<cv::detail::CameraParams>& cameras,
    cv::Ptr<cv::detail::RotationWarper> warper,
    cv::Mat& mask_warped,
    cv::Mat& mask,
    cv::Ptr<cv::detail::ExposureCompensator> compensator,
    const std::vector<cv::Point>& corners,
    cv::Mat& img_warped,
    cv::Mat& img_warped_s,
    const std::vector<cv::UMat>& masks_warped,
    cv::Mat& dilated_mask,
    cv::Mat& seam_mask,
    cv::Ptr<cv::detail::Blender> blender
) {

    for (int img_idx = 0; img_idx < num_images; ++img_idx)
    {
        
        // std::cout << "Compositing image #" << indices[img_idx]+1 << std::endl

        cv::Mat img = frame[img_idx];
        cv::Size img_size = img.size();

        cv::Mat K;
        cameras[img_idx].K().convertTo(K, CV_32F);

        warper->warp(img, K, cameras[img_idx].R, cv::INTER_LINEAR, cv::BORDER_REFLECT, img_warped);

        mask.create(img_size, CV_8U);
        mask.setTo(cv::Scalar::all(255));
        warper->warp(mask, K, cameras[img_idx].R, cv::INTER_NEAREST, cv::BORDER_CONSTANT, mask_warped);

        compensator->apply(img_idx, corners[img_idx], img_warped, mask_warped);

        img_warped.convertTo(img_warped_s, CV_16S);
        img_warped.release();
        img.release();
        mask.release();

        cv::dilate(masks_warped[img_idx], dilated_mask, cv::Mat());
        cv::resize(dilated_mask, seam_mask, mask_warped.size(), 0, 0, cv::INTER_LINEAR_EXACT);
        mask_warped = seam_mask & mask_warped;

        blender->feed(img_warped_s, mask_warped, corners[img_idx]);
    }

    std::scoped_lock lock(mtx);
    cv::Mat result, result_mask;
    blender->blend(result, result_mask);
    result.convertTo(result, CV_8UC3);

    StitchingLoopResult output;
    output.stitchedimage = result;

    return output;
}