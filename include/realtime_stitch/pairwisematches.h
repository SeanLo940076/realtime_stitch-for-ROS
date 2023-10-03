
#ifndef PAIRWISEMATCHES_H
#define PAIRWISEMATCHES_H

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/stitching/detail/matchers.hpp>
#include "opencv2/stitching/detail/motion_estimators.hpp"
#include <fstream>
#include <iostream>

struct PairwiseMatchResults {
    std::vector<cv::detail::ImageFeatures> features;
    std::vector<cv::detail::MatchesInfo> pairwise_matches;
};

PairwiseMatchResults PairwiseMatches(
    const std::vector<cv::detail::ImageFeatures>& features,
    const std::string& matcher_type,
    bool try_cuda,
    double match_conf, 
    int range_width, 
    std::string save_graph_to, 
    std::vector<std::string>& img_names, 
    float conf_thresh
);

#endif // PAIRWISEMATCHES_H