#include "focalsdecide.h"
#include <opencv2/stitching/detail/warpers.hpp>
#include "opencv2/stitching/detail/motion_estimators.hpp"
#include "opencv2/stitching/warpers.hpp"
#include <algorithm>

FocalsDecideResult DecideFocalAndCorrectWave(
    std::vector<cv::detail::CameraParams>& cameras,
    bool do_wave_correct,
    cv::detail::WaveCorrectKind wave_correct
) {
    std::vector<double> focals;
    for (const auto& camera : cameras) {
        focals.push_back(camera.focal);
    }

    sort(focals.begin(), focals.end());
    float warped_image_scale;
    if (focals.size() % 2 == 1)
        warped_image_scale = static_cast<float>(focals[focals.size() / 2]);
    else
        warped_image_scale = static_cast<float>(focals[focals.size() / 2 - 1] + focals[focals.size() / 2]) * 0.5f;

    std::vector<cv::detail::CameraParams> updated_cameras = cameras;
    if (do_wave_correct) {
        std::vector<cv::Mat> rmats;
        for (const auto& camera : cameras)
            rmats.push_back(camera.R.clone());
        cv::detail::waveCorrect(rmats, wave_correct);
        for (size_t i = 0; i < cameras.size(); ++i)
            cameras[i].R = rmats[i];
    }
    FocalsDecideResult result;

    result.warped_image_scale = warped_image_scale;
    result.cameras = cameras;
    return result;
}
