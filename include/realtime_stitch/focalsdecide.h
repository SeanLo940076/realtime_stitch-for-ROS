#ifndef FOCALSDECIDE_H
#define FOCALSDECIDE_H

#include <opencv2/core/core.hpp>
#include <opencv2/stitching/detail/camera.hpp>
#include "opencv2/stitching/detail/warpers.hpp"
#include "opencv2/stitching/detail/motion_estimators.hpp"
#include "opencv2/stitching/warpers.hpp"

struct FocalsDecideResult {
    float warped_image_scale;
    std::vector<cv::detail::CameraParams> cameras;
};

FocalsDecideResult DecideFocalAndCorrectWave(
    std::vector<cv::detail::CameraParams>& cameras,
    bool do_wave_correct,
    cv::detail::WaveCorrectKind wave_correct
);

#endif // FOCALSDECIDE_H
