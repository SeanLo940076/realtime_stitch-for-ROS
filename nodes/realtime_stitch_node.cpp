// ROS include
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/Image.h>

// main include
#include "inference.h"
#include "imagecropper.h"
#include "findfeature.h"
#include "pairwisematches.h"
#include "filterimage.h"
#include "estimator.h"
#include "adjusterimage.h"
#include "focalsdecide.h"
#include "warpingimage.h"
#include "exposcompensation.h"
#include "findseam.h"
#include "imagescaler.h"
#include "imageblender.h"
#include "runyolo.h"
#include "stitchingloop.h"

// External libraries
#include <yaml-cpp/yaml.h>
#include "pugixml.hpp"
#include <numeric>

// parallel libraries
#include "tbb/tbb.h"

// configure
// Load the YAML configuration file
YAML::Node config = YAML::LoadFile("/home/sean/catkin_ws/src/realtime_stitch/config/config.yaml");

// Read each parameter from the YAML file
std::vector<std::string> img_names = config["img_names"].as<std::vector<std::string>>();
bool try_cuda = config["try_cuda"].as<bool>();
double work_megapix = config["work_megapix"].as<double>();
double seam_megapix = config["seam_megapix"].as<double>();
double compose_megapix = config["compose_megapix"].as<double>();
float conf_thresh = config["conf_thresh"].as<float>();
std::string features_type = config["features_type"].as<std::string>();
float match_conf = config["match_conf"].as<float>();
std::string matcher_type = config["matcher_type"].as<std::string>();
std::string estimator_type = config["estimator_type"].as<std::string>();
std::string ba_cost_func = config["ba_cost_func"].as<std::string>();
std::string ba_refine_mask = config["ba_refine_mask"].as<std::string>();
bool do_wave_correct = config["do_wave_correct"].as<bool>();
std::string save_graph_to = config["save_graph_to"].as<std::string>();
std::string warp_type = config["warp_type"].as<std::string>();
int expos_comp_nr_feeds = config["expos_comp_nr_feeds"].as<int>();
int expos_comp_nr_filtering = config["expos_comp_nr_filtering"].as<int>();
int expos_comp_block_size = config["expos_comp_block_size"].as<int>();
std::string seam_find_type = config["seam_find_type"].as<std::string>();
float blend_strength = config["blend_strength"].as<float>();
int range_width = config["range_width"].as<int>();
std::string save_show_root = config["save_show_root"].as<std::string>();
std::string yolo_path = config["yolo_path"].as<std::string>();
std::string image_model_path = config["image_model_path"].as<std::string>();
std::string image_model_number = config["image_model_number"].as<std::string>();
std::string video_path = config["video_path"].as<std::string>();

cv::detail::WaveCorrectKind wave_correct = cv::detail::WAVE_CORRECT_HORIZ;
int expos_comp_type = cv::detail::ExposureCompensator::NO;  // ExposureCompensator::NO, ExposureCompensator::GAIN_BLOCKS
int blend_type = cv::detail::Blender::MULTI_BAND;           // Blender::MULTI_BAND, Blender::NO

cv::Mat frames[6];
// bool new_frame_received[6] = {false, false, false, false, false, false};

void imageCallback1(const sensor_msgs::ImageConstPtr& msg)
{
    try
    {
        frames[0] = cv_bridge::toCvShare(msg, "bgr8")->image;
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}

void imageCallback2(const sensor_msgs::ImageConstPtr& msg)
{
    try
    {
        frames[1]= cv_bridge::toCvShare(msg, "bgr8")->image;
        // TODO: Add your image processing logic here for camera 1
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}

void imageCallback3(const sensor_msgs::ImageConstPtr& msg)
{
    try
    {
        frames[2] = cv_bridge::toCvShare(msg, "bgr8")->image;
        // TODO: Add your image processing logic here for camera 1
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}

void imageCallback4(const sensor_msgs::ImageConstPtr& msg)
{
    try
    {
        frames[3] = cv_bridge::toCvShare(msg, "bgr8")->image;
        // TODO: Add your image processing logic here for camera 1
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}

void imageCallback5(const sensor_msgs::ImageConstPtr& msg)
{
    try
    {
        frames[4] = cv_bridge::toCvShare(msg, "bgr8")->image;
        // TODO: Add your image processing logic here for camera 1
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}

void imageCallback6(const sensor_msgs::ImageConstPtr& msg)
{
    try
    {
        frames[5] = cv_bridge::toCvShare(msg, "bgr8")->image;
        // TODO: Add your image processing logic here for camera 1
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "realtime_stitch_node");
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);

    image_transport::Subscriber sub1 = it.subscribe("usb_cam1/image_raw", 1, imageCallback1);
    image_transport::Subscriber sub2 = it.subscribe("usb_cam2/image_raw", 1, imageCallback2);
    image_transport::Subscriber sub3 = it.subscribe("usb_cam3/image_raw", 1, imageCallback3);
    image_transport::Subscriber sub4 = it.subscribe("usb_cam4/image_raw", 1, imageCallback4);
    image_transport::Subscriber sub5 = it.subscribe("usb_cam5/image_raw", 1, imageCallback5);
    image_transport::Subscriber sub6 = it.subscribe("usb_cam6/image_raw", 1, imageCallback6);
    image_transport::Publisher pub_result = it.advertise("stitched_image/image_raw", 1);

    // Stitching template images
    img_names.push_back(image_model_path + "cam1_" + image_model_number + ".jpg");
    img_names.push_back(image_model_path + "cam2_" + image_model_number + ".jpg");
    img_names.push_back(image_model_path + "cam3_" + image_model_number + ".jpg");
    img_names.push_back(image_model_path + "cam4_" + image_model_number + ".jpg");
    img_names.push_back(image_model_path + "cam5_" + image_model_number + ".jpg");
    img_names.push_back(image_model_path + "cam6_" + image_model_number + ".jpg");

    int num_images = static_cast<int>(img_names.size());

    std::cout << "Finding features..." << std::endl;
    ExtractedFeatures extracted = ExtractFeatures(img_names, features_type, work_megapix, seam_megapix);
    // 使用結構化綁定 >> 不用重新宣告
    auto &[features, images, seam_scale, seam_work_aspect, work_scale, is_work_scale_set, is_seam_scale_set, full_img_sizes] = extracted;

    std::cout << "Pairwise matching..." << std::endl;
    std::vector<cv::detail::MatchesInfo> pairwise_matches; // features, matcher_type, try_cuda, match_conf, range_width, save_graph, img_names, conf_thresh
    PairwiseMatchResults pairwise = PairwiseMatches(features, matcher_type, try_cuda, match_conf, range_width, save_graph_to, img_names, conf_thresh);
    features = pairwise.features;
    pairwise_matches = pairwise.pairwise_matches;

    std::cout << "Filter Image to panorama..." << std::endl;
    std::vector<int> indices;
    FilteredImageResult filtered = FilterImages(features, pairwise_matches, conf_thresh, img_names, images, full_img_sizes);
    indices = filtered.indices;
    images = filtered.images;
    full_img_sizes = filtered.full_img_sizes;
    num_images = filtered.num_images;

    std::cout << "Estimator Image K, R..." << std::endl;
    EstimatorResult estimatorResult = EstimateCameras(features, pairwise_matches, estimator_type);
    std::vector<cv::detail::CameraParams> cameras = estimatorResult.cameras;
    features = estimatorResult.features;
    pairwise_matches = estimatorResult.pairwise_matches;

    std::cout << "Bundle Adjustment..." << std::endl;
    AdjusterResult adjusterResult = CreateAndAdjust(features, pairwise_matches, ba_cost_func, cameras, conf_thresh, ba_refine_mask);
    cameras = adjusterResult.cameras;
    features = adjusterResult.features;
    pairwise_matches = adjusterResult.pairwise_matches;

    std::cout << "Find median focal length and wave Correction..." << std::endl;
    FocalsDecideResult focalsdecide = DecideFocalAndCorrectWave(cameras, do_wave_correct, wave_correct);
    float warped_image_scale = focalsdecide.warped_image_scale;
    cameras = focalsdecide.cameras;

    std::cout << "Warping images (auxiliary)..." << std::endl;
    WarpingResult warpingResult = WarpImages(images, cameras, warped_image_scale, seam_work_aspect, warp_type, try_cuda, num_images);
    std::vector<cv::Point> corners = warpingResult.corners;
    std::vector<cv::UMat> masks_warped = warpingResult.masks_warped;
    std::vector<cv::UMat> images_warped = warpingResult.images_warped;
    std::vector<cv::Size> sizes = warpingResult.sizes;
    std::vector<cv::UMat> images_warped_f = warpingResult.images_warped_f;
    warped_image_scale = warpingResult.warped_image_scale;
    num_images = warpingResult.num_images;
    cv::Ptr<cv::detail::RotationWarper> warper = warpingResult.warper;
    cv::Ptr<cv::WarperCreator> warper_creator = warpingResult.warper_creator;

    std::cout << "Compensating exposure..." << std::endl;
    ExposureCompensation exposcomp = ApplyExposureCompensation(corners, images_warped, masks_warped, expos_comp_type, expos_comp_nr_feeds, expos_comp_nr_filtering, expos_comp_block_size);
    corners = exposcomp.corners;
    // images_warped = exposcomp.images_warped;
    masks_warped = exposcomp.masks_warped;
    cv::Ptr<cv::detail::ExposureCompensator> compensator = exposcomp.compensator;

    std::cout << "Finding seams..." << std::endl;
    SeamFinderResult seamfindresult = FindSeams(images_warped_f, corners, masks_warped, seam_find_type);
    corners = seamfindresult.corners;
    masks_warped = seamfindresult.masks_warped;


    ros::Rate rate(30); // or your preferred rate
    while (ros::ok())
    {
        ros::spinOnce();
        // 創建混和器Blender
        cv::Mat img_warped, img_warped_s;
        cv::Mat dilated_mask, seam_mask, mask, mask_warped;
        static cv::Ptr<cv::detail::Blender> blender;
        static bool is_compose_scale_set = false;
        double compose_scale;
        // std::cout << "Compositing image #" << indices[img_idx]+1);

        if (!is_compose_scale_set)
        {
            ImageScalerResult imagescaler = AdjustImageScalesAndCameras(warper, compose_megapix, frames[0], work_scale, warped_image_scale, num_images, warper_creator, full_img_sizes, corners, cameras, sizes);
            warper = imagescaler.warper;
            compose_scale = imagescaler.compose_scale;
            cameras = imagescaler.cameras;
            corners = imagescaler.corners;
            sizes = imagescaler.sizes;

            is_compose_scale_set = true;
        }

        if (!blender)
        {
            BlenderImagesResult imageblender = SetupImageBlender(blender, blend_type, try_cuda, corners, sizes, blend_strength);
            blender = imageblender.blender;
        }

        StitchingLoopResult stitching = processor(num_images, frames, cameras, warper, mask_warped, mask, compensator, corners, img_warped, img_warped_s, masks_warped, dilated_mask, seam_mask, blender);
        cv::Mat result = stitching.stitchedimage;

        static bool cropped = false;
        static std::vector<cv::Point> nearest_points_ok;
        if (!cropped)
        {
            nearest_points_ok = CropImageUsingContours(result);
            cropped = true;
        }
        cv::Rect roi(nearest_points_ok[0], nearest_points_ok[3]);
        result = result(roi);
        
        sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", result).toImageMsg();
        pub_result.publish(msg);

        // ros::spin();  // Keep the node running and process callbacks
        rate.sleep();
    }
    return 0;
}