#include <Windows.h>
#include "cvf.h"
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <thread>   
#include <chrono>
#include <future>
#include <functional>

std::string cvf::getImagePath(std::string imgName) 
{
    std::string exeDir = cvf::exePath;
    size_t pos = exeDir.find("D3Helper.exe");
    if (pos != std::string::npos) {
        exeDir = exeDir.substr(0, pos);
    }
    std::string imagePath = exeDir + "Templates\\" + imgName;
    return imagePath;
}

std::string cvf::getDirectoryPath() {
    std::string exeDir = cvf::exePath;
    size_t pos = exeDir.find("D3Helper.exe");
    if (pos != std::string::npos) {
        exeDir = exeDir.substr(0, pos);
    }
    return exeDir + "Templates\\";
}


cv::Mat cvf::getClipboardImage() {
    if (!OpenClipboard(nullptr)) {
        MessageBox(
            NULL,  // Parent window, NULL for top-level
            "Clipboard is empty!",  // Message
            "Error",  // Title
            MB_ICONERROR | MB_OK  // Icon and buttons
        );
        return cv::Mat();
    }

    HANDLE hClipboardData = GetClipboardData(CF_BITMAP);
    if (hClipboardData == nullptr) {
        MessageBox(
            NULL,  // Parent window, NULL for top-level
            "Clipboard is empty!",  // Message
            "Error",  // Title
            MB_ICONERROR | MB_OK  // Icon and buttons
        );
        CloseClipboard();
        return cv::Mat();
    }

    HBITMAP hBitmap = static_cast<HBITMAP>(hClipboardData);
    if (hBitmap == nullptr) {
        MessageBox(
            NULL,  // Parent window, NULL for top-level
            "Clipboard is empty!",  // Message
            "Error",  // Title
            MB_ICONERROR | MB_OK  // Icon and buttons
        );
        CloseClipboard();
        return cv::Mat();
    }

    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    int width = bmp.bmWidth;
    int height = bmp.bmHeight;

    cv::Mat src(height, width, CV_8UC4);
    HDC hdc = CreateCompatibleDC(NULL);
    SelectObject(hdc, hBitmap);
    BitBlt(hdc, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    GetDIBits(hdc, hBitmap, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // Convert the image from RGBA to RGB
    cv::cvtColor(src, src, cv::COLOR_RGBA2RGB);

    // Convert to grayscale
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_RGB2GRAY);

    DeleteDC(hdc);
    CloseClipboard();

    return gray;
}

Pix* cvf::matToPix(cv::Mat& mat) {
    if (mat.empty()) {
        return nullptr;
    }

    // Convert to grayscale if necessary
    cv::Mat gray;
    if (mat.channels() == 3) {
        cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
    }
    else {
        gray = mat;
    }

    // Create a Pix object
    Pix* pix = pixCreate(gray.cols, gray.rows, 8);
    if (!pix) {
        std::cerr << "Could not create Pix object" << std::endl;
        return nullptr;
    }

    // Copy data from Mat to Pix
    l_uint32* pixData = pixGetData(pix);
    int wpl = pixGetWpl(pix); // words per line

    for (int y = 0; y < gray.rows; ++y) {
        l_uint32* line = pixData + y * wpl;
        const uint8_t* matLine = gray.ptr(y);
        for (int x = 0; x < gray.cols; ++x) {
            SET_DATA_BYTE(line, x, matLine[x]);
        }
    }

    return pix;
}

RECT cvf::getSelectedRegion() {
    RECT region = { 0, 0, 0, 0 };
    POINT startPoint = { 0, 0 };
    POINT endPoint = { 0, 0 };

    std::cout << "Click and drag to select a region." << std::endl;

    // Loop until the user releases the mouse button
    while (true) {
        if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
            GetCursorPos(&startPoint);
            ScreenToClient(selectedWindowHandle, &startPoint);
            std::cout << "Started at: (" << startPoint.x << ", " << startPoint.y << ")" << std::endl;
            SetCapture(selectedWindowHandle);

            while (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
                GetCursorPos(&endPoint);
                ScreenToClient(selectedWindowHandle, &endPoint);
                std::cout << "Dragging: (" << endPoint.x << ", " << endPoint.y << ")" << "\r";

                // Update the selected region in real-time
                region.left = std::min(startPoint.x, endPoint.x);
                region.top = std::min(startPoint.y, endPoint.y);
                region.right = std::max(startPoint.x, endPoint.x);
                region.bottom = std::max(startPoint.y, endPoint.y);

                // Draw the frame of the selected region with darkening effect
                cvf::drawRectangleOnWindow(selectedWindowHandle, region);
            }

            std::cout << std::endl;
            ReleaseCapture();

            break;
        }
    }

    return region;
}

void cvf::drawRectangleOnWindow(HWND hwnd, RECT& rect) {
    HDC hdc = GetDC(hwnd);

    // Set the pen color to white
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    // Draw the rectangle frame
    MoveToEx(hdc, rect.left, rect.top, NULL);
    LineTo(hdc, rect.right, rect.top);
    LineTo(hdc, rect.right, rect.bottom);
    LineTo(hdc, rect.left, rect.bottom);
    LineTo(hdc, rect.left, rect.top);

    // Cleanup
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    ReleaseDC(hwnd, hdc);
}

std::string cvf::modifyString(std::string& str) {
    size_t pos = str.find('('); // Find the position of '('

    if (pos != std::string::npos) {
        // Erase everything after '(' including '(' itself
        str.erase(pos);
    }
    str.erase(std::remove_if(str.begin(), str.end(), [](char c) {
        return !std::isalpha(static_cast<unsigned char>(c));
        }), str.end());
    return str;
}

// Function to split a string into multiple strings based on a delimiter
std::array<std::string, 3> cvf::splitString(std::string& str) {
    std::array<std::string, 3> tokens;
    size_t start = 0, end = 0;
    std::string delimiter = "\n";
    size_t index = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos && index < 3) {
        tokens[index++] = str.substr(start, end - start);
        start = end + delimiter.length();
    }

    // Add the last token or the entire string if the delimiter is not found
    if (index < 3) {
        tokens[index] = str.substr(start);
    }

    return tokens;
}

// Mock function to simulate mouse click at (x, y) coordinate
void cvf::sendClick(int x, int y, char c, int number) {
    SetCursorPos(x, y);  // Set cursor position
    std::this_thread::sleep_for(std::chrono::milliseconds(20));// Mouse left button down
    if (c == 'L') {
        for (int i = 0; i < number; i++)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));// Mouse left button down
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));// Mouse left button down
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        }
    }
    if (c == 'R'){
        for (int i = 0; i < number; i++)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));// Mouse left button down
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));// Mouse left button down
            mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);   // Mouse left button up
        }
    }
}


bool cvf::isDefaultRegion(const RECT& rect)
{
    return (rect.left == 0 && rect.right == 0 && rect.top == 0 && rect.bottom == 0);
}

cv::Rect cvf::RECT2cvRect(const RECT& rect) {
    return cv::Rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

POINT cvf::cvPointToPOINT(const cv::Point& cvPoint) {
    POINT pt;
    pt.x = cvPoint.x;
    pt.y = cvPoint.y;
    return pt;
}

POINT cvf::cvPoint2fToPOINT(const cv::Point2f& cvPoint) {
    POINT pt;
    pt.x = static_cast<LONG>(cvPoint.x); // Convert x coordinate from float to LONG
    pt.y = static_cast<LONG>(cvPoint.y); // Convert y coordinate from float to LONG
    return pt;
}

void cvf::setCursorMid() {
    // Get the screen resolution
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate the middle point
    int middleX = screenWidth / 2;
    int middleY = screenHeight / 2;

    // Move the cursor to the middle of the screen
    SetCursorPos(middleX, middleY);
}

// Function to check for ESC key press in a separate thread
void  cvf::checkForESCKeyPress() {
    cvf::exitLoop = false;
    while (!cvf::exitLoop.load()) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            cvf::exitLoop.store(true);
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Small delay to avoid high CPU usage
    }
}

void cvf::waitForKeyPress1() {
    while (!cvf::exitLoop.load()) {
        for (int vkCode = 8; vkCode <= 255; ++vkCode) {
            if (GetAsyncKeyState(vkCode) & 0x8000) {
                while (GetAsyncKeyState(vkCode) & 0x8000) {} // Wait for the key to be released
                cvf::exitLoop.store(true);
                cvf::hotkeyGambling = vkCode; // Update the pressed key
                return;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Small delay to avoid high CPU usage
    }
}

void cvf::waitForKeyPress2() {
    while (!cvf::exitLoop.load()) {
        for (int vkCode = 8; vkCode <= 255; ++vkCode) {
            if (GetAsyncKeyState(vkCode) & 0x8000) {
                while (GetAsyncKeyState(vkCode) & 0x8000) {} // Wait for the key to be released
                cvf::hotkeyPortaling = vkCode; // Update the pressed key    
                cvf::exitLoop.store(true);
                return;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Small delay to avoid high CPU usage
    }
}

void cvf::waitForKeyPress3() {
    while (!cvf::exitLoop.load()) {
        for (int vkCode = 8; vkCode <= 255; ++vkCode) {
            if (GetAsyncKeyState(vkCode) & 0x8000) {
                while (GetAsyncKeyState(vkCode) & 0x8000) {} // Wait for the key to be released
                cvf::hotkeyEnch = vkCode; // Update the pressed key            cvf::exitLoop.store(true);
                cvf::exitLoop.store(true);
                return;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Small delay to avoid high CPU usage
    }
}

std::string cvf::getKeyName(int vkCode) {
    std::unordered_map<int, std::string> keyNames = {
    {VK_BACK, "Backspace"}, {VK_TAB, "Tab"}, {VK_RETURN, "Enter"},
    {VK_SHIFT, "Shift"}, {VK_CONTROL, "Ctrl"}, {VK_MENU, "Alt"},
    {VK_CAPITAL, "Caps Lock"}, {VK_ESCAPE, "Escape"}, {VK_SPACE, "Space"},
    {VK_PRIOR, "Page Up"}, {VK_NEXT, "Page Down"}, {VK_END, "End"},
    {VK_HOME, "Home"}, {VK_LEFT, "Left Arrow"}, {VK_UP, "Up Arrow"},
    {VK_RIGHT, "Right Arrow"}, {VK_DOWN, "Down Arrow"},
    {VK_INSERT, "Insert"}, {VK_DELETE, "Delete"}, {VK_F1, "F1"},
    {VK_F2, "F2"}, {VK_F3, "F3"}, {VK_F4, "F4"}, {VK_F5, "F5"},
    {VK_F6, "F6"}, {VK_F7, "F7"}, {VK_F8, "F8"}, {VK_F9, "F9"},
    {VK_F10, "F10"}, {VK_F11, "F11"}, {VK_F12, "F12"},

    {VK_LSHIFT, "Left Shift"}, {VK_RSHIFT, "Right Shift"},
    {VK_LCONTROL, "Left Ctrl"}, {VK_RCONTROL, "Right Ctrl"},
    {VK_LMENU, "Left Alt"}, {VK_RMENU, "Right Alt"},

    {VK_NUMPAD0, "Numpad 0"}, {VK_NUMPAD1, "Numpad 1"},
    {VK_NUMPAD2, "Numpad 2"}, {VK_NUMPAD3, "Numpad 3"},
    {VK_NUMPAD4, "Numpad 4"}, {VK_NUMPAD5, "Numpad 5"},
    {VK_NUMPAD6, "Numpad 6"}, {VK_NUMPAD7, "Numpad 7"},
    {VK_NUMPAD8, "Numpad 8"}, {VK_NUMPAD9, "Numpad 9"},
    {VK_MULTIPLY, "Numpad *"}, {VK_ADD, "Numpad +"},
    {VK_SEPARATOR, "Numpad Separator"}, {VK_SUBTRACT, "Numpad -"},
    {VK_DECIMAL, "Numpad ."}, {VK_DIVIDE, "Numpad /"},

    {VK_OEM_1, "OEM ;:"}, {VK_OEM_PLUS, "OEM +"},
    {VK_OEM_COMMA, "OEM ,"}, {VK_OEM_MINUS, "OEM -"},
    {VK_OEM_PERIOD, "OEM ."}, {VK_OEM_2, "OEM /?"},
    {VK_OEM_3, "OEM `~"}, {VK_OEM_4, "OEM [{ "},
    {VK_OEM_5, "OEM \\|"}, {VK_OEM_6, "OEM ]}"},
    {VK_OEM_7, "OEM '\""},

    {VK_BROWSER_BACK, "Browser Back"}, {VK_BROWSER_FORWARD, "Browser Forward"},
    {VK_BROWSER_REFRESH, "Browser Refresh"}, {VK_BROWSER_STOP, "Browser Stop"},
    {VK_BROWSER_SEARCH, "Browser Search"}, {VK_BROWSER_FAVORITES, "Browser Favorites"},
    {VK_BROWSER_HOME, "Browser Home"},

    {VK_VOLUME_MUTE, "Volume Mute"}, {VK_VOLUME_DOWN, "Volume Down"},
    {VK_VOLUME_UP, "Volume Up"}, {VK_MEDIA_NEXT_TRACK, "Next Track"},
    {VK_MEDIA_PREV_TRACK, "Previous Track"}, {VK_MEDIA_STOP, "Stop Media"},
    {VK_MEDIA_PLAY_PAUSE, "Play/Pause Media"}, {VK_LAUNCH_MAIL, "Launch Mail"},
    {VK_LAUNCH_MEDIA_SELECT, "Launch Media Select"}, {VK_LAUNCH_APP1, "Launch App 1"},
    {VK_LAUNCH_APP2, "Launch App 2"},

    {0x30, "0"}, {0x31, "1"}, {0x32, "2"}, {0x33, "3"},
    {0x34, "4"}, {0x35, "5"}, {0x36, "6"}, {0x37, "7"},
    {0x38, "8"}, {0x39, "9"},

    {0x41, "A"}, {0x42, "B"}, {0x43, "C"}, {0x44, "D"},
    {0x45, "E"}, {0x46, "F"}, {0x47, "G"}, {0x48, "H"},
    {0x49, "I"}, {0x4A, "J"}, {0x4B, "K"}, {0x4C, "L"},
    {0x4D, "M"}, {0x4E, "N"}, {0x4F, "O"}, {0x50, "P"},
    {0x51, "Q"}, {0x52, "R"}, {0x53, "S"}, {0x54, "T"},
    {0x55, "U"}, {0x56, "V"}, {0x57, "W"}, {0x58, "X"},
    {0x59, "Y"}, {0x5A, "Z"}
    };

    if (keyNames.find(vkCode) != keyNames.end()) {
        return keyNames[vkCode];
    }
    return "Unknown";
}
