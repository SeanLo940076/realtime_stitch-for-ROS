#include "findfeature.h"

ExtractedFeatures ExtractFeatures(const std::vector<std::string>& img_names, const std::string& features_type, double work_megapix, double seam_megapix) {
    int num_images = static_cast<int>(img_names.size());

    cv::Ptr<cv::Feature2D> finder;
    if (features_type == "orb")
    {
        finder = cv::ORB::create();
    }
    else if (features_type == "akaze")
    {
        finder = cv::AKAZE::create();
    }
    else if (features_type == "surf")
    {
        finder = cv::xfeatures2d::SURF::create();
        std::cout << "surf" << std::endl;
    }
    else if (features_type == "sift")
    {
        finder = cv::SIFT::create();
        std::cout << "sift" << std::endl;
    }
    else
    {
        std::cout << "Unknown 2D features type: '" << features_type << std::endl;
    }

    cv::Mat full_img, img;
    std::vector<cv::detail::ImageFeatures> features(num_images);
    std::vector<cv::Mat> images(num_images);
    std::vector<cv::Size> full_img_sizes(num_images);
    double seam_work_aspect = 1;

    // work_scale seam_scale compose_scale
    double work_scale = 1, seam_scale = 1, compose_scale = 1;
    bool is_work_scale_set = false, is_seam_scale_set = false, is_compose_scale_set = false;

    for (int i = 0; i < num_images; ++i)
    {
        full_img = cv::imread(cv::samples::findFile(img_names[i]));
        full_img_sizes[i] = full_img.size();

        if (full_img.empty())
        {
            std::cout << "Can't open image " << img_names[i] << std::endl;
        }
        if (work_megapix < 0)       // work_megapix = 1 
        {
            img = full_img;
            work_scale = 1;
            is_work_scale_set = true;
        }
        else
        {
            if (!is_work_scale_set) // --> here
            {
                work_scale = std::min(1.0, sqrt(work_megapix * 1e6 / full_img.size().area())); // work_megapix = 1
                is_work_scale_set = true;
            }
            // 將 full_img 縮放 work_scale 倍，並將結果存儲在 img 中，縮放時使用的是線性插值法
            cv::resize(full_img, img, cv::Size(), work_scale, work_scale, cv::INTER_LINEAR_EXACT);
        }
        if (!is_seam_scale_set) // --> no in here
        {
            seam_scale = std::min(1.0, sqrt(seam_megapix * 1e6 / full_img.size().area()));
            seam_work_aspect = seam_scale / work_scale;
            is_seam_scale_set = true;
        }
        // 計算圖像特徵
        cv::detail::computeImageFeatures(finder, img, features[i]);
        features[i].img_idx = i;
        std::cout << "Features in image #" << i+1 << ": " << features[i].keypoints.size() << std::endl;
        // 將 full_img 縮放 seam_scale 倍，並將結果存儲在 img 中，縮放時使用的是線性插值法
        cv::resize(full_img, img, cv::Size(), seam_scale, seam_scale, cv::INTER_LINEAR_EXACT);
        // resize(full_img, img, Size(), 1, 1, INTER_LINEAR_EXACT);     // 還原原始大小 只有用於顯示匹配
        images[i] = img.clone();
    }

    full_img.release();
    img.release();

    ExtractedFeatures result;
    result.features = features;
    result.images = images;
    result.seam_scale = seam_scale;
    result.seam_work_aspect = seam_work_aspect;
    result.work_scale = work_scale;
    result.is_work_scale_set = is_work_scale_set;
    result.is_seam_scale_set = is_seam_scale_set;
    result.full_img_sizes = full_img_sizes;
    
    return result;
}
