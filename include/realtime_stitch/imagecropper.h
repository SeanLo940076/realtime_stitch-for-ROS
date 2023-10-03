#ifndef IMAGE_CROPPER_H
#define IMAGE_CROPPER_H

#include <opencv2/opencv.hpp>
#include <vector>

std::vector<cv::Point> CropImageUsingContours(cv::Mat &result);

#endif // IMAGE_CROPPER_H
