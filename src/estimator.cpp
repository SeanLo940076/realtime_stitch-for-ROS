#include "estimator.h"

EstimatorResult EstimateCameras(
    const std::vector<cv::detail::ImageFeatures>& features,
    const std::vector<cv::detail::MatchesInfo>& pairwise_matches,
    const std::string& estimator_type
) {
    EstimatorResult result;
    cv::Ptr<cv::detail::Estimator> estimator;

    if (estimator_type == "affine")
        estimator = cv::makePtr<cv::detail::AffineBasedEstimator>();
    else
        estimator = cv::makePtr<cv::detail::HomographyBasedEstimator>();

    if (!estimator->operator()(features, pairwise_matches, result.cameras)) {
        std::cout << "Homography estimation failed.\n";
    }

    for (size_t i = 0; i < result.cameras.size(); ++i) {
        cv::Mat R;
        result.cameras[i].R.convertTo(R, CV_32F);
        result.cameras[i].R = R;
        std::cout << "Initial camera intrinsics:\nK:\n" << result.cameras[i].K() << "\nR:\n" << result.cameras[i].R << std::endl;
    }
    result.features = features;
    result.pairwise_matches = pairwise_matches;
    return result;
}
