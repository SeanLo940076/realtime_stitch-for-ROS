#include "exposcompensation.h"

ExposureCompensation ApplyExposureCompensation(
    std::vector<cv::Point> corners,
    std::vector<cv::UMat> images_warped,
    std::vector<cv::UMat> masks_warped,
    int expos_comp_type,
    int expos_comp_nr_feeds,
    int expos_comp_nr_filtering,
    int expos_comp_block_size
) {    
    cv::Ptr<cv::detail::ExposureCompensator> compensator = cv::detail::ExposureCompensator::createDefault(expos_comp_type);

    if (cv::detail::GainCompensator* gcompensator = dynamic_cast<cv::detail::GainCompensator*>(compensator.get())) {
        gcompensator->setNrFeeds(expos_comp_nr_feeds);
    }

    if (cv::detail::ChannelsCompensator* ccompensator = dynamic_cast<cv::detail::ChannelsCompensator*>(compensator.get())) {
        ccompensator->setNrFeeds(expos_comp_nr_feeds);
    }

    if (cv::detail::BlocksCompensator* bcompensator = dynamic_cast<cv::detail::BlocksCompensator*>(compensator.get())) {
        bcompensator->setNrFeeds(expos_comp_nr_feeds);
        bcompensator->setNrGainsFilteringIterations(expos_comp_nr_filtering);
        bcompensator->setBlockSize(expos_comp_block_size, expos_comp_block_size);
    }

    compensator->feed(corners, images_warped, masks_warped);

    ExposureCompensation result;
    result.compensator = compensator;
    result.corners = corners;
    result.images_warped = images_warped;
    result.masks_warped = masks_warped;

    return result;
}
