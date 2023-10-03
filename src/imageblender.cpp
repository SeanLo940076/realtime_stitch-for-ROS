#include "imageblender.h"
#include <string>
#include <iostream>
#include <fstream>
#include "opencv2/stitching/detail/blenders.hpp"
#include "opencv2/stitching/detail/timelapsers.hpp"
#include <opencv2/core/utility.hpp>
#include <opencv2/stitching/detail/util.hpp>

BlenderImagesResult SetupImageBlender(
    cv::Ptr<cv::detail::Blender> blender,
    int blend_type,
    bool try_cuda,
    std::vector<cv::Point> corners,
    std::vector<cv::Size> sizes,
    float blend_strength
) {
    
    if (!blender) // 如果 blender 尚未被創建
    {
        blender = cv::detail::Blender::createDefault(blend_type, try_cuda);
        cv::Size dst_sz = cv::detail::resultRoi(corners, sizes).size();
        float blend_width = sqrt(static_cast<float>(dst_sz.area())) * blend_strength / 100.f;
        if (blend_width < 1.f)
            blender = cv::detail::Blender::createDefault(cv::detail::Blender::NO, try_cuda);
        else if (blend_type == cv::detail::Blender::MULTI_BAND)
        {
            cv::detail::MultiBandBlender* mb = dynamic_cast<cv::detail::MultiBandBlender*>(blender.get());
            mb->setNumBands(static_cast<int>(ceil(log(blend_width)/log(2.)) - 1.));
            std::cout << "Multi-band blender, number of bands: " << mb->numBands() << std::endl;
        }
        else if (blend_type == cv::detail::Blender::FEATHER)
        {
            cv::detail::FeatherBlender* fb = dynamic_cast<cv::detail::FeatherBlender*>(blender.get());
            fb->setSharpness(1.f/blend_width);
            std::cout << "Feather blender, sharpness: " << fb->sharpness() << std::endl;
        }
        blender->prepare(corners, sizes);
    }

    BlenderImagesResult result;
    result.blender = blender;

    return result;
}