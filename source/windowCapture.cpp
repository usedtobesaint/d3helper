#include <Windows.h>
#include "cvf.h"
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;


cv::Mat cvf::hwnd2mat(LPCSTR windowName)
{
    HDC hwindowDC, hwindowCompatibleDC;
    int height, width, srcHeight, srcWidth;
    HBITMAP hbwindow;
    Mat src;
    BITMAPINFOHEADER bi;

    HWND hwnd = FindWindow(NULL, windowName);
    if (hwnd == NULL) {
        MessageBox(
            NULL,  // Parent window, NULL for top-level
            "No such window found!",  // Message
            "Error",  // Title
            MB_ICONERROR | MB_OK  // Icon and buttons
        );
        return src;
    }
    else {
        hwindowDC = GetDC(hwnd);
        hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
        SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

        RECT windowsize;    // get the height and width of the screen
        GetClientRect(hwnd, &windowsize);

        srcHeight = windowsize.bottom;
        srcWidth = windowsize.right;
        height = srcHeight;
        width = srcWidth;

        src.create(height, width, CV_8UC4);

        // create a bitmap
        hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
        bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
        bi.biWidth = width;
        bi.biHeight = -height;  //this is the line that makes it draw upside down or not
        bi.biPlanes = 1;
        bi.biBitCount = 32;
        bi.biCompression = BI_RGB;
        bi.biSizeImage = 0;
        bi.biXPelsPerMeter = 0;
        bi.biYPelsPerMeter = 0;
        bi.biClrUsed = 0;
        bi.biClrImportant = 0;

        // use the previously created device context with the bitmap
        SelectObject(hwindowCompatibleDC, hbwindow);
        // copy from the window device context to the bitmap device context
        StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcWidth, srcHeight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
        GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

        // Convert the image from RGBA to RGB
        cvtColor(src, src, cv::COLOR_RGBA2RGB);

        // avoid memory leak
        DeleteObject(hbwindow);
        DeleteDC(hwindowCompatibleDC);
        ReleaseDC(hwnd, hwindowDC);
    }
    return src;
}

cv::Mat cvf::hwnd2mat(LPCSTR windowName, RECT region) {
    HDC hwindowDC, hwindowCompatibleDC;
    int height, width, srcHeight, srcWidth;
    HBITMAP hbwindow;
    Mat src;
    BITMAPINFOHEADER bi;

    HWND hwnd = FindWindow(NULL, windowName);
    if (hwnd == NULL || IsRectEmpty(&region)) {
        MessageBox(
            NULL,  // Parent window, NULL for top-level
            "No window found or region selected!",  // Message
            "Error",  // Title
            MB_ICONERROR | MB_OK  // Icon and buttons
        );
        return src;
    }
    else {
        hwindowDC = GetDC(hwnd);
        hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
        SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

        // Set the coordinates and dimensions of the region to capture
        srcHeight = region.bottom - region.top;
        srcWidth = region.right - region.left;
        height = srcHeight;
        width = srcWidth;

        src.create(height, width, CV_8UC4);

        // create a bitmap
        hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = width;
        bi.biHeight = -height;  //this is the line that makes it draw upside down or not
        bi.biPlanes = 1;
        bi.biBitCount = 32;
        bi.biCompression = BI_RGB;
        bi.biSizeImage = 0;
        bi.biXPelsPerMeter = 0;
        bi.biYPelsPerMeter = 0;
        bi.biClrUsed = 0;
        bi.biClrImportant = 0;

        // use the previously created device context with the bitmap
        SelectObject(hwindowCompatibleDC, hbwindow);
        // copy from the window device context to the bitmap device context
        StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, region.left, region.top, srcWidth, srcHeight, SRCCOPY);
        GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

        // Convert the image from RGBA to RGB
        cvtColor(src, src, cv::COLOR_RGBA2RGB);

        // avoid memory leak
        DeleteObject(hbwindow);
        DeleteDC(hwindowCompatibleDC);
        ReleaseDC(hwnd, hwindowDC);
    }
    return src;
}
