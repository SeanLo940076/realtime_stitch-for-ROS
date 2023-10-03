#ifndef STITCHINGLOOP_H
#define STITCHINGLOOP_H

#include <iostream>
#include <fstream>
#include <string>
#include "opencv2/opencv_modules.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching/detail/autocalib.hpp"
#include "opencv2/stitching/detail/blenders.hpp"
#include "opencv2/stitching/detail/timelapsers.hpp"
#include "opencv2/stitching/detail/camera.hpp"
#include "opencv2/stitching/detail/exposure_compensate.hpp"
#include "opencv2/stitching/detail/matchers.hpp"
#include "opencv2/stitching/detail/motion_estimators.hpp"
#include "opencv2/stitching/detail/seam_finders.hpp"
#include "opencv2/stitching/detail/warpers.hpp"
#include "opencv2/stitching/warpers.hpp"

#include "imagescaler.h"
#include "imageblender.h"
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <mutex>

#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudafilters.hpp>



struct StitchingLoopResult {
    cv::Mat stitchedimage;
};

StitchingLoopResult processor(
    int num_images,
    cv::Mat frame[6],
    const std::vector<cv::detail::CameraParams>& cameras,
    cv::Ptr<cv::detail::RotationWarper> warper,
    cv::Mat& mask_warped,
    cv::Mat& mask,
    cv::Ptr<cv::detail::ExposureCompensator> compensator,
    const std::vector<cv::Point>& corners,
    cv::Mat& img_warped,
    cv::Mat& img_warped_s,
    const std::vector<cv::UMat>& masks_warped,
    cv::Mat& dilated_mask,
    cv::Mat& seam_mask,
    cv::Ptr<cv::detail::Blender> blender
);

#endif // STITCHINGLOOP_H
