#include "imagecropper.h"
#include <algorithm> // for sort
#include <limits>    // for numeric_limits

std::vector<cv::Point> CropImageUsingContours(cv::Mat &result)
{
    cv::Mat cut_black = result.clone();
    cv::cvtColor(cut_black, cut_black, cv::COLOR_BGR2GRAY);
    cv::Mat thresh;
    cv::threshold(cut_black, thresh, 1, 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::sort(contours.begin(), contours.end(), [](const std::vector<cv::Point> &c1, const std::vector<cv::Point> &c2) {
        return cv::contourArea(c1, false) > cv::contourArea(c2, false);
    });

    std::vector<cv::Point> hull;
    cv::convexHull(contours[0], hull);

    std::vector<cv::Point> corners = {
        cv::Point(0, 0),
        cv::Point(0, cut_black.rows - 1),
        cv::Point(cut_black.cols - 1, 0),
        cv::Point(cut_black.cols - 1, cut_black.rows - 1)
    };

    std::vector<cv::Point> nearest_points;
    for (auto &corner : corners)
    {
        double min_dist = std::numeric_limits<double>::max();
        cv::Point nearest;
        for (auto &h : hull)
        {
            double dist = cv::norm(corner - h);
            if (dist < min_dist)
            {
                min_dist = dist;
                nearest = h;
            }
        }
        nearest_points.push_back(nearest);
    }

    int c = 71;
    int d = c - 1;
    cv::Point adjust_left_top = cv::Point(c, 0);
    cv::Point adjust_right_bottom = cv::Point(-d, 0);

    nearest_points[0] = nearest_points[0] + adjust_left_top;
    nearest_points[3] = nearest_points[3] + adjust_right_bottom;

    cut_black.release();
    thresh.release();

    return nearest_points;
}
