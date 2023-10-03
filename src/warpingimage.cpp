#include "warpingimage.h"

WarpingResult WarpImages(
    std::vector<cv::Mat>& images,
    std::vector<cv::detail::CameraParams>& cameras,
    float warped_image_scale,
    double seam_work_aspect,
    const std::string& warp_type,
    bool try_cuda,
    int num_images
) {
    std::vector<cv::Point> corners(num_images);
    std::vector<cv::UMat> masks_warped(num_images);
    std::vector<cv::UMat> images_warped(num_images);
    std::vector<cv::Size> sizes(num_images);
    std::vector<cv::UMat> masks(num_images);

    // Prepare images masks
    for (int i = 0; i < num_images; ++i)
    {
        masks[i].create(images[i].size(), CV_8U);
        masks[i].setTo(cv::Scalar::all(255));
    }

    // Warp images and their masks

    cv::Ptr<cv::WarperCreator> warper_creator;

#ifdef HAVE_OPENCV_CUDAWARPING
    if (try_cuda && cv::cuda::getCudaEnabledDeviceCount() > 0)
    {
        if (warp_type == "plane")
            warper_creator = cv::makePtr<cv::PlaneWarperGpu>();
        else if (warp_type == "cylindrical")
            warper_creator = cv::makePtr<cv::CylindricalWarperGpu>();
        else if (warp_type == "spherical")
            warper_creator = cv::makePtr<cv::SphericalWarperGpu>();
    }
    else
#endif
    {
        if (warp_type == "plane")
            warper_creator = cv::makePtr<cv::PlaneWarper>();
        else if (warp_type == "affine")
            warper_creator = cv::makePtr<cv::AffineWarper>();
        else if (warp_type == "cylindrical")
            warper_creator = cv::makePtr<cv::CylindricalWarper>();
        else if (warp_type == "spherical")
            warper_creator = cv::makePtr<cv::SphericalWarper>();
        else if (warp_type == "fisheye")
            warper_creator = cv::makePtr<cv::FisheyeWarper>();
        else if (warp_type == "stereographic")
            warper_creator = cv::makePtr<cv::StereographicWarper>();
        else if (warp_type == "compressedPlaneA2B1")
            warper_creator = cv::makePtr<cv::CompressedRectilinearWarper>(2.0f, 1.0f);
        else if (warp_type == "compressedPlaneA1.5B1")
            warper_creator = cv::makePtr<cv::CompressedRectilinearWarper>(1.5f, 1.0f);
        else if (warp_type == "compressedPlanePortraitA2B1")
            warper_creator = cv::makePtr<cv::CompressedRectilinearPortraitWarper>(2.0f, 1.0f);
        else if (warp_type == "compressedPlanePortraitA1.5B1")
            warper_creator = cv::makePtr<cv::CompressedRectilinearPortraitWarper>(1.5f, 1.0f);
        else if (warp_type == "paniniA2B1")
            warper_creator = cv::makePtr<cv::PaniniWarper>(2.0f, 1.0f);
        else if (warp_type == "paniniA1.5B1")
            warper_creator = cv::makePtr<cv::PaniniWarper>(1.5f, 1.0f);
        else if (warp_type == "paniniPortraitA2B1")
            warper_creator = cv::makePtr<cv::PaniniPortraitWarper>(2.0f, 1.0f);
        else if (warp_type == "paniniPortraitA1.5B1")
            warper_creator = cv::makePtr<cv::PaniniPortraitWarper>(1.5f, 1.0f);
        else if (warp_type == "mercator")
            warper_creator = cv::makePtr<cv::MercatorWarper>();
        else if (warp_type == "transverseMercator")
            warper_creator = cv::makePtr<cv::TransverseMercatorWarper>();
    }

    if (!warper_creator)
    {
        std::cout << "Can't create the following warper '" << warp_type << "'" << std::endl;
    }
    cv::Ptr<cv::detail::RotationWarper> warper = warper_creator->create(static_cast<float>(warped_image_scale * seam_work_aspect));

    for (int i = 0; i < num_images; ++i)
    {
        cv::Mat_<float> K;
        cameras[i].K().convertTo(K, CV_32F);
        float swa = static_cast<float>(seam_work_aspect);
        K(0,0) *= swa; K(0,2) *= swa;
        K(1,1) *= swa; K(1,2) *= swa;

        corners[i] = warper->warp(images[i], K, cameras[i].R, cv::INTER_LINEAR, cv::BORDER_REFLECT, images_warped[i]);
        sizes[i] = images_warped[i].size();

        warper->warp(masks[i], K, cameras[i].R, cv::INTER_NEAREST, cv::BORDER_CONSTANT, masks_warped[i]);
    }

    std::vector<cv::UMat> images_warped_f(num_images);
    for (int i = 0; i < num_images; ++i)
        images_warped[i].convertTo(images_warped_f[i], CV_32F);

    masks.clear();

    WarpingResult result;
    result.corners = corners;
    result.masks_warped = masks_warped;
    result.images_warped = images_warped;
    result.sizes = sizes;
    result.images_warped_f = images_warped_f;
    result.warped_image_scale = warped_image_scale;
    result.num_images = num_images;
    result.warper = warper;
    result.warper_creator = warper_creator;

    return result;
}
