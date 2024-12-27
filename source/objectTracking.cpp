#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include "cvf.h"
#include <iostream>

// Defined initial HSV range for blue portal
int blueLowerH = 84, blueLowerS = 101, blueLowerV = 154;
int blueUpperH = 138, blueUpperS = 205, blueUpperV = 255;

// Defined initial HSV range for orange portal
int orangeLowerH = 12, orangeLowerS = 60, orangeLowerV = 209;
int orangeUpperH = 26, orangeUpperS = 223, orangeUpperV = 255;

//for orek
int lowH = 70, lowS = 123, lowV = 121;
int highH = 97, highS = 193, highV = 217;


POINT cvf::camShift(std::string& templateName) 
{
    //createTrackbars();
    bool objectFound =false;

    cv::Point2f objectCenter = { 0,0 };
    POINT POINTcenter = { 0,0 };

    cv::Mat frame;
    // Capture the first frame to initialize the tracking window
    frame = cvf::hwnd2mat(cvf::windowName);

    std::string templatePath = cvf::getImagePath(templateName);
    cv::Mat templateImg = cv::imread(templatePath, cv::IMREAD_COLOR);

    // Define the tracking window using the dimensions of the template image
    cv::Rect trackWindow(0, 0, templateImg.cols, templateImg.rows);

    // Convert the ROI to HSV color space
    cv::Mat tempHsv, hue, mask, hist, backproj;
    cvtColor(templateImg, tempHsv, cv::COLOR_BGR2HSV);

    // Create mask and histogram for the hue channel
    int hbins = 180; // 180 bins for hue channel
    int histSize[] = { hbins };
    float hranges[] = { 0, 180 };
    const float* ranges[] = { hranges };
    int channels[] = { 0 };

    // Calculate histogram and normalize it
    inRange(tempHsv, cv::Scalar(lowH, lowS, lowV), cv::Scalar(highH, highS, highV), mask);
    calcHist(&tempHsv, 1, channels, mask, hist, 1, histSize, ranges);
    normalize(hist, hist, 0, 255, cv::NORM_MINMAX);

    // Termination criteria for CamShift
    cv::TermCriteria termCrit(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 10, 1);


    //while (!objectFound) {

        frame = cvf::hwnd2mat(cvf::windowName);
        cv::Mat hsv;
        cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

        inRange(tempHsv, cv::Scalar(lowH, lowS, lowV), cv::Scalar(highH, highS, highV), mask);
        calcHist(&tempHsv, 1, channels, mask, hist, 1, histSize, ranges);
        normalize(hist, hist, 0, 255, cv::NORM_MINMAX);

        // Backproject the histogram to get a probability image
        calcBackProject(&hsv, 1, channels, hist, backproj, ranges);

        // Apply thresholding to filter out low-probability areas
        cv::threshold(backproj, backproj, 50, 255, cv::THRESH_BINARY);

        // Find contours in the thresholded backprojection image
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(backproj, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        cv::RotatedRect bestTrackBox;
        double bestScore = 9999999999999; // Initialize to a large value

        // Filter contours by size and shape (if needed)
        for (const auto& contour : contours) {
            cv::Rect boundingBox = cv::boundingRect(contour);
            if (boundingBox.area() > 4000) { // Example size filter
                // Calculate the histogram for the current region
                cv::Mat regionHsv = hsv(boundingBox);
                cv::Mat regionHist;
                calcHist(&regionHsv, 1, channels, cv::Mat(), regionHist, 1, histSize, ranges);
                normalize(regionHist, regionHist, 0, 255, cv::NORM_MINMAX);

                // Calculate the Bhattacharyya distance
                double score = compareHist(hist, regionHist, cv::HISTCMP_BHATTACHARYYA);

                // Update the best match if the current region is a better match
                if (score < bestScore) {
                    bestScore = score;
                    bestTrackBox = cv::CamShift(backproj, boundingBox, termCrit);
                    objectFound = true; // Object is found
                }
            }
        }

        // Draw the best tracking result if found
        if (objectFound) {
            ellipse(frame, bestTrackBox, cv::Scalar(0, 0, 255), 2);

            // Get the center of the tracking box
            objectCenter = bestTrackBox.center;

            // Display the object's position
            std::ostringstream positionText;
            positionText << "Position: (" << objectCenter.x << ", " << objectCenter.y << ")";
            cv::putText(frame, positionText.str(), cv::Point(10, 30), cv::FONT_HERSHEY_COMPLEX,
                0.7, cv::Scalar(255, 255, 255), 2);
            SetCursorPos(objectCenter.x, objectCenter.y);
        }
        else {
            std::cout << "Object not found in the frame." << std::endl;
        }     

    POINTcenter = cvf::cvPoint2fToPOINT(objectCenter);
    return POINTcenter;

}



POINT cvf::detectPortal() {
    try {
        // Create masks for blue and orange colors
        cv::Mat blueMask, orangeMask;

        bool isPortalDetected = false;
        cv::Point Pointcenter;
        POINT POINTcenter;

        while (!isPortalDetected) {
            // Update HSV ranges
            cv::Scalar blueLower(blueLowerH, blueLowerS, blueLowerV);
            cv::Scalar blueUpper(blueUpperH, blueUpperS, blueUpperV);
            cv::Scalar orangeLower(orangeLowerH, orangeLowerS, orangeLowerV);
            cv::Scalar orangeUpper(orangeUpperH, orangeUpperS, orangeUpperV);

            cv::Mat frame = cvf::hwnd2mat(cvf::windowName);
            cv::Mat hsv;
            cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

            cv::inRange(hsv, blueLower, blueUpper, blueMask);
            cv::inRange(hsv, orangeLower, orangeUpper, orangeMask);

            // Apply morphological operations to clean up the masks
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
            cv::morphologyEx(blueMask, blueMask, cv::MORPH_CLOSE, kernel);
            cv::morphologyEx(orangeMask, orangeMask, cv::MORPH_CLOSE, kernel);

            // Find contours in the blue mask
            std::vector<std::vector<cv::Point>> blueContours;
            cv::findContours(blueMask, blueContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            // Find contours in the orange mask
            std::vector<std::vector<cv::Point>> orangeContours;
            cv::findContours(orangeMask, orangeContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            // Process blue contours to find the largest one
            if (!blueContours.empty()) {
                auto largestBlueContour = *std::max_element(blueContours.begin(), blueContours.end(),
                    [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
                        return cv::contourArea(a) < cv::contourArea(b);
                    });
                cv::Rect boundingBox = cv::boundingRect(largestBlueContour);
                if (boundingBox.area() > 6500) { // Adjust the size filter as needed
                    cv::rectangle(frame, boundingBox, cv::Scalar(255, 0, 0), 2); // Draw blue rectangle
                    Pointcenter = (boundingBox.tl() + boundingBox.br()) / 2;
                    cv::putText(frame, "Blue Portal", Pointcenter, cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
                    isPortalDetected = true;
                }
            }

            // Process orange contours to find the largest one
            if (!orangeContours.empty()) {
                auto largestOrangeContour = *std::max_element(orangeContours.begin(), orangeContours.end(),
                    [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
                        return cv::contourArea(a) < cv::contourArea(b);
                    });
                cv::Rect boundingBox = cv::boundingRect(largestOrangeContour);
                if (boundingBox.area() > 3500) { // Adjust the size filter as needed
                    cv::rectangle(frame, boundingBox, cv::Scalar(0, 140, 255), 2); // Draw orange rectangle
                    Pointcenter = (boundingBox.tl() + boundingBox.br()) / 2;
                    cv::putText(frame, "Orange Portal", Pointcenter, cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
                    isPortalDetected = true;
                }
            }
            POINTcenter = cvf::cvPointToPOINT(Pointcenter);

            if (isPortalDetected) {
                SetCursorPos(POINTcenter.x, POINTcenter.y);
            }

        }

        return POINTcenter;

    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;  // Handle any exceptions
    };

}


void cvf::portalSkipping()
{
    using namespace cvf;
    cvf::exitLoop.store(false);
    std::cout << "Portal skipping task started." << std::endl << "Flag: " << cvf::exitLoop.load() << std::endl;
    
    while (!cvf::exitLoop.load()) {
        std::cout << "Getting rift button position." << std::endl;
        POINT riftButton = getTemplatePos(cvf::riftButton, 2);
        std::cout << "Rift button position: (" << riftButton.x << ", " << riftButton.y << ")" << std::endl;
        sendClick(riftButton.x, riftButton.y, 'L', 1);

        std::cout << "Getting accept button position." << std::endl;
        POINT acceptButton = getTemplatePos(cvf::acceptButton, 2);
        std::cout << "Accept button position: (" << acceptButton.x << ", " << acceptButton.y << ")" << std::endl;
        sendClick(acceptButton.x, acceptButton.y, 'L', 1);

        setCursorMid();
        std::this_thread::sleep_for(std::chrono::seconds(3));

        std::cout << "Detecting portal." << std::endl;
        POINT portal = detectPortal();
        std::cout << "Portal detected at: (" << portal.x << ", " << portal.y << ")" << std::endl;
        sendClick(portal.x, portal.y, 'L', 2);

        std::this_thread::sleep_for(std::chrono::seconds(3));

        std::cout << "Cam shifting for orek." << std::endl;
        POINT orek = camShift(cvf::orek);
        if (orek.x != 0 && orek.y != 0) {
            std::cout << "Orek found at: (" << orek.x << ", " << orek.y << ")" << std::endl;
            sendClick(orek.x + 10, orek.y, 'L', 1);

            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "Getting close rift option position." << std::endl;
            POINT option = getTemplatePos(closeRift, 2);
            std::cout << "Close rift option position: (" << option.x << ", " << option.y << ")" << std::endl;
            sendClick(option.x, option.y, 'L', 1);

            std::this_thread::sleep_for(std::chrono::seconds(3));
            std::cout << "Getting rift position." << std::endl;
            POINT rift = getTemplatePos(cvf::rift, 2);
            std::cout << "Rift position: (" << rift.x << ", " << rift.y << ")" << std::endl;
            sendClick(rift.x, rift.y, 'L', 2);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        else {
            std::cout << "Finished portal skipping.1" << std::endl;
            break;
        }
    }
}
