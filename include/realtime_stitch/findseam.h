#ifndef FINDSEAM_H
#define FINDSEAM_H

#include <opencv2/core/core.hpp>
#include <opencv2/stitching/detail/seam_finders.hpp>
#include <vector>
#include <iostream>

struct SeamFinderResult {
    std::vector<cv::Point> corners;
    std::vector<cv::UMat> masks_warped;    
};

SeamFinderResult FindSeams(
    std::vector<cv::UMat>& images_warped_f,
    std::vector<cv::Point>& corners,
    std::vector<cv::UMat>& masks_warped,
    const std::string& seam_find_type
);

#endif // FINDSEAM_H
