#ifndef EXPOSCOMPENSATION_H
#define EXPOSCOMPENSATION_H

#include <opencv2/core/core.hpp>
#include <opencv2/stitching/detail/exposure_compensate.hpp>


struct ExposureCompensation {
    cv::Ptr<cv::detail::ExposureCompensator> compensator;
    std::vector<cv::Point> corners;
    std::vector<cv::UMat> images_warped;
    std::vector<cv::UMat> masks_warped;
};

ExposureCompensation ApplyExposureCompensation(
    std::vector<cv::Point> corners,
    std::vector<cv::UMat> images_warped,
    std::vector<cv::UMat> masks_warped,
    int expos_comp_type,
    int expos_comp_nr_feeds,
    int expos_comp_nr_filtering,
    int expos_comp_block_size
);

#endif // EXPOSCOMPENSATION_H
