#ifndef FINDFEATURE_H
#define FINDFEATURE_H

#include <vector>
#include <opencv2/opencv.hpp>
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/features2d.hpp"

struct ExtractedFeatures {
    std::vector<cv::detail::ImageFeatures> features;
    std::vector<cv::Mat> images;
    double seam_scale;
    double seam_work_aspect;
    double work_scale;
    bool is_work_scale_set;
    bool is_seam_scale_set;
    std::vector<cv::Size> full_img_sizes;
};

ExtractedFeatures ExtractFeatures(const std::vector<std::string>& img_names, const std::string& features_type, double work_megapix, double seam_megapix);

#endif // FINDFEATURE_H
