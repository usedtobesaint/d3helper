#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>
#include <regex>
#include "cvf.h"
#include <limits>
#include <stdexcept>
#include <thread>
#include <chrono>


std::string cvf::OCRFromClipboard() {
    // Set the TESSDATA_PREFIX environment variable in code
    _putenv_s("TESSDATA_PREFIX", cvf::tessdata_prefix.c_str());

    std::string result;

    // Initialize Tesseract API
    tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
    if (ocr->Init(NULL, "eng")) {
        std::cout << "Could not initialize tesseract." << std::endl;
        delete ocr;
        return result;
    }

    cv::Mat img = cvf::getClipboardImage();
    if (img.empty()) {
        std::cout << "Could not retrieve the image from clipboard" << std::endl;
        return result;
    }

    Pix* pixImage = cvf::matToPix(img);
    if (!pixImage) {
        return result;
    }

    // Set the image for Tesseract to process
    ocr->SetImage(pixImage);

    // Perform OCR
    char* text = ocr->GetUTF8Text();
    if (text) {
        result = std::string(text);
        std::cout << "OCR output:\n" << text << std::endl;
        delete[] text;
    }
    else {
        std::cerr << "Could not extract text." << std::endl;
    }

    // Clean up
    pixDestroy(&pixImage);
    ocr->End();
    delete ocr;
    return result;
}

std::string cvf::OCRFromRegion(LPCSTR windowName, RECT rect) {
    // Set the TESSDATA_PREFIX environment variable in code
    _putenv_s("TESSDATA_PREFIX", cvf::tessdata_prefix.c_str());

    std::string result;

    // Initialize Tesseract API
    tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
    if (ocr->Init(NULL, "eng")) {
        std::cout << "Could not initialize tesseract." << std::endl;
        delete ocr;
        return result;
    }

    cv::Mat capturedImage = cvf::hwnd2mat(windowName, rect);
    if (capturedImage.empty()) {
        std::cerr << "Failed to capture the window area." << std::endl;
        return result;
    }

    Pix* pixImage = cvf::matToPix(capturedImage);
    if (!pixImage) {
        std::cerr << "Failed to convert Mat to Pix." << std::endl;
        return result;
    }

    ocr->SetImage(pixImage);
    char* outText = ocr->GetUTF8Text();
    if (outText) {
        result = std::string(outText);
        std::cout << "OCR Result: " << std::endl << result << std::endl;
        delete[] outText;
    }
    else {
        std::cerr << "Could not extract text from image." << std::endl;
    }

    // Clean up
    pixDestroy(&pixImage);
    ocr->End();
    delete ocr;
    return result;
}


std::pair<double, std::string> cvf::extractHighestNumberAndText(const std::string& str) {

    double highestNumber = -1;
    std::string highestText;

    std::regex rgx("\\[\\s*(\\d+\\.?\\d*)\\s*-\\s*(\\d+\\.?\\d*)\\s*\\]");
    std::smatch match;

    if (std::regex_search(str, match, rgx)) {
        double number1 = std::stod(match[1]); // Extract the first number from the match
        double number2 = std::stod(match[2]); // Extract the second number from the match

        // Choose the highest number
        highestNumber = number2;

        // Find the position of the match in the input string
        size_t startPos = str.find('[');
        size_t endPos = str.find(']', startPos);

        // Construct the highestText by combining the text before and after the brackets
        highestText = str.substr(0, startPos);
        highestText += str.substr(endPos + 1); // +1 to skip the closing bracket
    }

    // Remove leading and trailing whitespace from the highest text
    highestText.erase(0, highestText.find_first_not_of(" \t\n\v\f\r"));
    highestText.erase(highestText.find_last_not_of(" \t\n\v\f\r") + 1);

    return { highestNumber, highestText };
}

std::pair<double, std::string> cvf::extractNumberAndText(const std::string& str) {
    double number = -1;
    std::string text = str;

    // Regex to match a number
    std::regex rgx("(\\d+\\.?\\d*)");
    std::smatch match;

    // Perform regex search to find the number
    if (std::regex_search(str, match, rgx)) {
        number = std::stod(match[1].str()); // Extract the number from the match

        // Remove the number from the text
        text = str.substr(0, match.position()) + str.substr(match.position() + match.length());
    }

    // Remove leading and trailing whitespace from the text
    text.erase(0, text.find_first_not_of(" \t\n\v\f\r"));
    text.erase(text.find_last_not_of(" \t\n\v\f\r") + 1);

    return { number, text };
}

std::vector<std::string> splitIntoLines(const std::string& input) {
    std::vector<std::string> lines;
    std::istringstream stream(input);
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    return lines;
}

bool containsNumber(const std::string& line) {
    std::regex numberRegex("\\d+");
    return std::regex_search(line, numberRegex);
}

std::string filterLines( std::string& input) {
    std::vector<std::string> lines = splitIntoLines(input);
    std::vector<std::string> filteredLines;

    for (const std::string& line : lines) {
        if (!line.empty() && containsNumber(line)) {
            filteredLines.push_back(line);
        }
    }

    // Join the filtered lines back into a single string
    std::ostringstream output;
    for (const std::string& line : filteredLines) {
        output << line << "\n";
    }
    return output.str();
}

void cvf::rollingEcnhantments()
{
    try {
        std::array<std::string, 3> rolledEnchSeparated;

        std::string rolledEnch = cvf::OCRFromRegion(cvf::windowName, cvf::enchRegion);

        rolledEnch = filterLines(rolledEnch);

        rolledEnchSeparated = cvf::splitString(rolledEnch);

        std::array<std::pair<double, std::string>, 3> rolledEnchParsed;

        cvf::desiredEnchParsed.second = cvf::modifyString(cvf::desiredEnchParsed.second);

        std::string currEnch = "";
        std::string wantedEnch = "";

        std::cout << std::endl << "Desired: ";
        std::cout << "Value:" << cvf::desiredEnchParsed.first << " . Name:";
        for (char c : cvf::desiredEnchParsed.second) {
            std::cout << c;
            wantedEnch += c;
        }
        std::cout << std::endl;

        for (int i = 0; i < 3; i++)
        {
            currEnch = "";
            rolledEnchParsed[i] = cvf::extractNumberAndText(rolledEnchSeparated[i]);
            rolledEnchParsed[i].second = cvf::modifyString(rolledEnchParsed[i].second);
            std::cout << std::endl << "Rolled " << i << ": ";
            std::cout << "Value:" << rolledEnchParsed[i].first << " . Name:";
            for (char c : rolledEnchParsed[i].second) {
                std::cout << c;
                currEnch += c;
            }
            std::cout << std::endl;

            if (currEnch == wantedEnch)
            {
                if (rolledEnchParsed[i].first >= rolledEnchParsed[0].first ||
                    rolledEnchParsed[i].second!=rolledEnchParsed[0].second)
                    cvf::enchNumberResult = i;
                if (rolledEnchParsed[i].first == cvf::desiredEnchParsed.first)
                    cvf::enchNumberResult = -1;
            }

        }

        std::cout << "Best enchant after this roll is at pos " << cvf::enchNumberResult << std::endl;

    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;  // Handle any exceptions
    };   
}

// Function to handle the click based on the option
void cvf::handleOptionClick(RECT& rect, int option) {
    if (option == -1) {
        std::cout << "Option is -1" << std::endl;
        return;
    }

    if (option < 0 || option > 2) {
        std::cerr << "Invalid option. Must be 0, 1, or 2." << std::endl;
        return;
    }

    // Calculate the width and height of the rectangle
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    // Calculate the width of each option
    int optionHeight = height / 3;

    // Calculate the center x-coordinate of the chosen option
    int clickX = rect.left + width / 2;
    int clickY = rect.top + optionHeight * (option+1) - optionHeight/2;

    // Perform the click at the calculated position
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    cvf::sendClick(clickX, clickY, 'L', 1);
}