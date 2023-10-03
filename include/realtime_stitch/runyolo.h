#ifndef RUNYOLO_H
#define RUNYOLO_H

#include "inference.h"
#include <iostream>
#include <vector>
#include <getopt.h>
#include <opencv2/opencv.hpp>

struct RunYoloResult {
    cv::Mat yoloresult;
};

RunYoloResult runyolo(
    Inference inf,
    cv::Mat imputimage
);

#endif // RUNYOLO_H
