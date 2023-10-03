#include "imagescaler.h"

ImageScalerResult AdjustImageScalesAndCameras(
    cv::Ptr<cv::detail::RotationWarper> warper,
    double compose_megapix,
    cv::Mat full_img,
    double work_scale,
    float warped_image_scale,
    int num_images,
    cv::Ptr<cv::WarperCreator>& warper_creator,
    std::vector<cv::Size>& full_img_sizes,
    std::vector<cv::Point> corners,
    std::vector<cv::detail::CameraParams>& cameras,
    std::vector<cv::Size> sizes
) {
    double compose_scale = 1.0;
    double compose_work_aspect = 1.0;
    
    if (compose_megapix > 0)  // compose_megapix df=-1  --> F
        compose_scale = std::min(1.0, std::sqrt(compose_megapix * 1e6 / full_img.size().area()));
    // Compute relative scales // 計算相對比例
    //compose_seam_aspect = compose_scale / seam_scale;
    compose_work_aspect = compose_scale / work_scale;       // compose_scale=1 // work_scale = min(1.0, sqrt(work_megapix * 1e6 / full_img.size().area()));
        
    // Update warped image scale // 更新變形的圖像比例，對圖像進行變換
    warped_image_scale *= static_cast<float>(compose_work_aspect);
    warper = warper_creator->create(warped_image_scale);

    // Update corners and sizes
    for (int i = 0; i < num_images; ++i)
    {
        // Update intrinsics
        cameras[i].focal *= compose_work_aspect;
        cameras[i].ppx *= compose_work_aspect;
        cameras[i].ppy *= compose_work_aspect;

        // Update corner and size
        cv::Size sz = full_img_sizes[i];
        if (std::abs(compose_scale - 1) > 1e-1)     // compose_scale = 1
        {
            sz.width = cvRound(full_img_sizes[i].width * compose_scale);    // don't in here
            sz.height = cvRound(full_img_sizes[i].height * compose_scale);  // don't in here
        }

        // 根據每一張圖片的鏡頭參數和旋轉矩陣對圖片進行變換。
        cv::Mat K;
        cameras[i].K().convertTo(K, CV_32F);        // 獲取內參矩陣，並將其轉換為浮點數型式
        cv::Rect roi = warper->warpRoi(sz, K, cameras[i].R); 
        // 存儲了計算得到的 ROI 的左上角點（top-left corner）和尺寸（大小）
        corners[i] = roi.tl();
        sizes[i] = roi.size();
    }

    ImageScalerResult result;
    result.warper = warper;
    result.compose_scale = compose_scale;
    result.cameras = cameras;
    result.corners = corners;
    result.sizes = sizes;

    return result;
}
