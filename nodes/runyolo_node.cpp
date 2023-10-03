#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "runyolo.h"

// Global publisher for the result (if you need to publish the result)
image_transport::Publisher pub;

void imageCallback(const sensor_msgs::ImageConstPtr& msg) {
    // Convert the image message to an OpenCV format
    cv_bridge::CvImagePtr cv_ptr;
    try {
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    } catch (cv_bridge::Exception& e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    // Now, you can use cv_ptr->image to get the OpenCV image
    cv::Mat image = cv_ptr->image;

    // Run YOLO on this image
    // Assuming the runYolo function takes an OpenCV image and returns the processed image.
    RunYoloResult runyolo(inf, imputimage)
)

    // Publish the result (if you want to)
    sensor_msgs::ImagePtr output_msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", result).toImageMsg();
    pub.publish(output_msg);
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "Yolov8_node");
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);

    // Subscribing to the input image topic
    image_transport::Subscriber sub = it.subscribe("/panoramicimage", 1, imageCallback);
    
    // Publisher for the result
    pub = it.advertise("/panoramic_yolo/image_raw", 1);

    ros::spin();
    return 0;
}
