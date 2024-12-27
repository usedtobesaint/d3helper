#pragma once

#include <opencv2/core.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <Windows.h>


namespace cvf {

	inline std::atomic<bool> exitLoop(false);
	inline std::atomic<int> hotkeyEnch;
	inline std::atomic<int> hotkeyGambling;
	inline std::atomic<int> hotkeyPortaling;

	inline bool isGamblingTab = false;
	inline bool isSalvaging = false;
	inline bool isRiftTab = false;

	inline int selectedItem = 0;
	inline int enchNumberResult = 0;

	inline std::string icon = "icon.ico";

	inline std::string rift = "rift.jpg";

	inline std::string selectEnch = "select_property.jpg";

	inline std::string gambleOneHand = "gamble_1hand.jpg";
	inline std::string gambleTwoHand = "gamble_2hand.jpg";
	inline std::string gambleQuiver = "gamble_quiver.jpg";
	inline std::string gambleOrb = "gamble_orb.jpg";
	inline std::string gambleMojo = "gamble_mojo.jpg";
	inline std::string gamblePhylactery = "gamble_phylactery";
	inline std::string gambleHelm = "gamble_helm.jpg";
	inline std::string gambleGloves = "gamble_gloves.jpg";
	inline std::string gambleBoots = "gamble_boots.jpg";
	inline std::string gambleChest = "gamble_chest.jpg";
	inline std::string gambleBelt = "gamble_belt.jpg";
	inline std::string gambleShoulders = "gamble_shoulders.jpg";
	inline std::string gamblePants = "gamble_pants.jpg";
	inline std::string gambleBracers = "gamble_bracers.jpg";
	inline std::string gambleShield = "gamble_shield.jpg";
	inline std::string gambleRing = "gamble_ring.jpg";
	inline std::string gambleAmulet = "gamble_amulet.jpg";
	inline std::string gamblingTab = "gambling_tab.jpg";
	inline std::string weaponsTab = "weapons_tab.jpg";
	inline std::string armorTab = "armor_tab.jpg";
	inline std::string jeweleryTab = "jewelery_tab.jpg";

	inline std::string riftTab = "rift_tab.jpg";
	inline std::string riftButton = "rift_button.jpg";
	inline std::string closeRift = "close_rift.jpg";
	inline std::string acceptButton = "accept_button.jpg";

	inline std::string orek = "orek.jpg";

	inline std::string tessdata_prefix = "";
	inline LPCSTR windowName = "Diablo III";

	inline std::string exePath;
	inline HWND selectedWindowHandle = nullptr;

	inline std::string desiredEnch;
	inline std::pair<double, std::string> desiredEnchParsed;

	inline RECT enchRegion = { 0,0,0,0 };
	inline POINT templatePos = { 0 ,0 };

	inline const double templateThreshold = 0.95;

	// Function to capture a specific window and display it in a named OpenCV window
	cv::Mat hwnd2mat(LPCSTR windowName);
	cv::Mat hwnd2mat(LPCSTR windowName, RECT region);

	cv::Mat templateMatching(const cv::Mat& src, const cv::Mat& templateImage);
	POINT getTemplatePos(std::string& templateName,int flag);
	void findTemplateImage(std::string& templateName, bool& pointer);
	void checkTemplateImage(std::string& templateName, bool& pointer);

	POINT camShift(std::string& templateName);
	POINT detectPortal();

	std::string getImagePath(std::string imgName);
	std::string getDirectoryPath();
	cv::Mat getClipboardImage();

	std::string OCRFromClipboard();
	std::string OCRFromRegion(LPCSTR windowName, RECT rect);
	std::array<std::string, 3> splitString(std::string& str);
	std::pair<double, std::string> extractHighestNumberAndText(const std::string& str);
	std::pair<double, std::string> extractNumberAndText(const std::string& str);
	void handleOptionClick(RECT& rect, int option);
	void rollingEcnhantments();
	void portalSkipping();

	Pix* matToPix(cv::Mat& mat);

	RECT getSelectedRegion();
	void drawRectangleOnWindow(HWND hwnd, RECT& rect);

	std::string modifyString(std::string& str);

	void sendClick(int x, int y, char c , int number);
	bool isDefaultRegion(const RECT& rect);
	cv::Rect RECT2cvRect(const RECT& rect);
	POINT cvPointToPOINT(const cv::Point& cvPoint);
	POINT cvPoint2fToPOINT(const cv::Point2f& cvPoint);
	void setCursorMid();

	void checkForESCKeyPress();
	void waitForKeyPress1();
	void waitForKeyPress2();
	void waitForKeyPress3();
	std::string getKeyName(int vkCode);

 }
