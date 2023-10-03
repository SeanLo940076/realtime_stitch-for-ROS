#include "adjusterimage.h"

AdjusterResult CreateAndAdjust(
    const std::vector<cv::detail::ImageFeatures>& features,
    const std::vector<cv::detail::MatchesInfo>& pairwise_matches,
    const std::string& ba_cost_func,
    std::vector<cv::detail::CameraParams>& cameras,
    double conf_thresh,
    const std::string& ba_refine_mask
) {
    AdjusterResult result;

    cv::Ptr<cv::detail::BundleAdjusterBase> adjuster;
    if (ba_cost_func == "reproj") adjuster = cv::makePtr<cv::detail::BundleAdjusterReproj>();
    else if (ba_cost_func == "ray") adjuster = cv::makePtr<cv::detail::BundleAdjusterRay>();
    else if (ba_cost_func == "affine") adjuster = cv::makePtr<cv::detail::BundleAdjusterAffinePartial>();
    else if (ba_cost_func == "no") adjuster = cv::makePtr<cv::detail::NoBundleAdjuster>();
    else
    {
        std::cout << "Unknown bundle adjustment cost function: '" << ba_cost_func << "'.\n";
    }
    adjuster->setConfThresh(conf_thresh);   // 調整信賴區間閾值
    cv::Mat_<uchar> refine_mask = cv::Mat::zeros(3, 3, CV_8U);
    if (ba_refine_mask[0] == 'x') refine_mask(0,0) = 1;
    if (ba_refine_mask[1] == 'x') refine_mask(0,1) = 1;
    if (ba_refine_mask[2] == 'x') refine_mask(0,2) = 1;
    if (ba_refine_mask[3] == 'x') refine_mask(1,1) = 1;
    if (ba_refine_mask[4] == 'x') refine_mask(1,2) = 1;
    adjuster->setRefinementMask(refine_mask);
    if (!adjuster->operator()(features, pairwise_matches, cameras))
    {    
        std::cout << "Camera parameters adjusting failed.\n";
    }
    result.features = features;
    result.pairwise_matches = pairwise_matches;
    result.cameras = cameras;
    return result;
}
