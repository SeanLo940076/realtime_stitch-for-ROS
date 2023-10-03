#include "runyolo.h"

RunYoloResult runyolo(
    Inference inf,
    cv::Mat imputimage
) {
    std::vector<Detection> output = inf.runInference(imputimage);
    int detections = output.size();
    // std::cout << "Number of detections:" << detections << std::endl;
    for (auto detection:output)
    {
        cv::Rect box = detection.box;
        cv::Scalar color = detection.color;

        // Detection box
        cv::rectangle(imputimage, box, color, 2);

        // Detection box text
        std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
        cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
        cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

        cv::rectangle(imputimage, textBox, color, cv::FILLED);
        cv::putText(imputimage, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
        // std::cout << "物件：" << detection.className << " 信心度：" << std::to_string(detection.confidence) 
        //             << " 左上右下位置： (" << box.x << ", " << box.y << "), (" << (box.x + box.width) << ", " << (box.y + box.height) << ")" << std::endl;
    }
    // for (int i = 0; i < detections; ++i)
    // {
    //     Detection detection = output[i];

    //     cv::Rect box = detection.box;
    //     cv::Scalar color = detection.color;

    //     // Detection box
    //     cv::rectangle(imputimage, box, color, 2);

    //     // Detection box text
    //     std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
    //     cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
    //     cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

    //     cv::rectangle(imputimage, textBox, color, cv::FILLED);
    //     cv::putText(imputimage, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
    //     // std::cout << "物件：" << detection.className << " 信心度：" << std::to_string(detection.confidence) 
    //     //           << " 左上右下位置： (" << box.x << ", " << box.y << "), (" << (box.x + box.width) << ", " << (box.y + box.height) << ")" << std::endl;
    // }

    RunYoloResult result;
    result.yoloresult = imputimage;

    return result;
}
