#include "findseam.h"
#include <iostream>

SeamFinderResult FindSeams(
    std::vector<cv::UMat>& images_warped_f,
    std::vector<cv::Point>& corners,
    std::vector<cv::UMat>& masks_warped,
    const std::string& seam_find_type
) {

    cv::Ptr<cv::detail::SeamFinder> seam_finder;

    if (seam_find_type == "no")
        seam_finder = cv::makePtr<cv::detail::NoSeamFinder>();
    else if (seam_find_type == "voronoi")
        seam_finder = cv::makePtr<cv::detail::VoronoiSeamFinder>();
    else if (seam_find_type == "gc_color")
    {
        seam_finder = cv::makePtr<cv::detail::GraphCutSeamFinder>(cv::detail::GraphCutSeamFinderBase::COST_COLOR);
    }
    else if (seam_find_type == "gc_colorgrad")
    {
        seam_finder = cv::makePtr<cv::detail::GraphCutSeamFinder>(cv::detail::GraphCutSeamFinderBase::COST_COLOR_GRAD);
    }
    else if (seam_find_type == "dp_color")
        seam_finder = cv::makePtr<cv::detail::DpSeamFinder>(cv::detail::DpSeamFinder::COLOR);
    else if (seam_find_type == "dp_colorgrad")
        seam_finder = cv::makePtr<cv::detail::DpSeamFinder>(cv::detail::DpSeamFinder::COLOR_GRAD);
    
    if (!seam_finder)
    {
        std::cout << "Can't create the following seam finder '" << seam_find_type << "'\n";
    }

    seam_finder->find(images_warped_f, corners, masks_warped);

    images_warped_f.clear();

    SeamFinderResult result;
    result.corners = corners;
    result.masks_warped = masks_warped;

    return result;
}
