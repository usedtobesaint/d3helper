#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <Windows.h>
#include "cvf.h"
#include <stdexcept>
#include <iostream>
#include <string>

cv::Mat cvf::templateMatching(const cv::Mat& src, const cv::Mat& templateImage) {
    cv::Mat result;
    cv::matchTemplate(src, templateImage, result, cv::TM_CCORR_NORMED);

    return result;
}

POINT cvf::getTemplatePos(std::string& templateName, int flag)
{
    bool lookingForTemplate = true;
    POINT templatePos;
    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::Mat srcRGB;
    cv::Mat result;
    std::string templatePath = cvf::getImagePath(templateName);
    std::cout << "Template path: " << templatePath << std::endl;
    cv::Mat templateImg = cv::imread(templatePath, cv::IMREAD_COLOR);

    if (flag == 1)
    {
        while (lookingForTemplate)
        {
            srcRGB = cvf::hwnd2mat(cvf::windowName);
            result = cvf::templateMatching(srcRGB, templateImg);
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

            if (maxVal >= cvf::templateThreshold)
            {
                templatePos.x = maxLoc.x + templateImg.cols / 2;
                templatePos.y = maxLoc.y + templateImg.rows / 2;
                std::cout << "Template pos: " << templatePos.x << " " << templatePos.y << std::endl;
                lookingForTemplate = false;
            }
        }
    }
    if (flag == 2) {
        srcRGB = cvf::hwnd2mat(cvf::windowName);
        result = cvf::templateMatching(srcRGB, templateImg);
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

        if (maxVal >= cvf::templateThreshold)
        {
            templatePos.x = maxLoc.x + templateImg.cols / 2;
            templatePos.y = maxLoc.y + templateImg.rows / 2;
            std::cout << "Template pos: " << templatePos.x << " " << templatePos.y << std::endl;
            lookingForTemplate = false;
        }
    }
    return templatePos;
}

void cvf::findTemplateImage(std::string& templateName, bool& pointer) {
    try {
        cv::Mat srcGray;
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;

        std::string templatePath = cvf::getImagePath(templateName);
        std::cout << "Template path: " << templatePath << std::endl;
        cv::Mat templateImg = cv::imread(templatePath, cv::IMREAD_GRAYSCALE);

        while (!pointer) {
            std::cout << "Looking for template..." << std::endl;
            cv::Mat srcRGB = cvf::hwnd2mat(cvf::windowName);
            cv::cvtColor(srcRGB, srcGray, cv::COLOR_RGB2GRAY);

            cv::Mat result = cvf::templateMatching(srcGray, templateImg);
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

            if (maxVal >= cvf::templateThreshold) {
                pointer = true;
                std::cout << "Template found. Finished searching." << std::endl;
                break;
            }
        }
    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;  // Handle any exceptions
    }
}

void cvf::checkTemplateImage(std::string& templateName, bool& pointer) {
    try {
        pointer = false;
        cv::Mat srcGray;
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;

        std::string templatePath = cvf::getImagePath(templateName);
        std::cout << "Template path: " << templatePath << std::endl;
        cv::Mat templateImg = cv::imread(templatePath, cv::IMREAD_GRAYSCALE);

        cv::Mat srcRGB = cvf::hwnd2mat(cvf::windowName);
        cv::cvtColor(srcRGB, srcGray, cv::COLOR_RGB2GRAY);

        cv::Mat result = cvf::templateMatching(srcGray, templateImg);
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

        if (maxVal >= cvf::templateThreshold) {
            pointer = true;
            std::cout << "Template found. Finished searching." << std::endl;
        }
        
    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;  // Handle any exceptions
    }
}