#include "gui.h"
#include "cvf.h"
#include <thread>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

using namespace gui;
int __stdcall  wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR arguments, int nCmdShow) {
	// Buffer to store the path
	char executablePath[MAX_PATH];

	// Get the path of the executable
	GetModuleFileName(NULL, executablePath, MAX_PATH);

	// Convert to std::string
	cvf::exePath = executablePath;

	std::string iconPath = cvf::getImagePath(cvf::icon);

	cvf::tessdata_prefix = cvf::getDirectoryPath();

	// create gui
	gui::CreateHWindow("D3Helper", "Helper");
	gui::CreateDevice();
	gui::CreateImGui();

	while (gui::isRunning)
	{
		gui::BeginRender();
		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ 500, 300 });
		ImGui::Begin(
			"D3Helper",
			&isRunning,
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse
		);

		// Kadala gambling
		if (GetForegroundWindow() == cvf::selectedWindowHandle)
		{
			if (GetAsyncKeyState(cvf::hotkeyGambling) & 0x8000){

				cvf::selectedWindowHandle = FindWindow(NULL, cvf::windowName);
				cvf::isGamblingTab = false;
				cvf::checkTemplateImage(cvf::gamblingTab, cvf::isGamblingTab);
				if (cvf::isGamblingTab)
				{
					if (cvf::selectedItem != 0) {
						std::cout << "F2 key pressed" << std::endl;
						std::cout << "Selected item " << cvf::selectedItem << std::endl;

						POINT tabPos;
						int tabNumber = cvf::selectedItem / 10;

						switch (tabNumber) {
						case 0:
							tabPos = cvf::getTemplatePos(cvf::weaponsTab,1);
							break;
						case 1:
							tabPos = cvf::getTemplatePos(cvf::armorTab,1);
							break;
						case 2:
							tabPos = cvf::getTemplatePos(cvf::jeweleryTab,1);
							break;
						}
						cvf::sendClick(tabPos.x, tabPos.y, 'L', 2);

						switch (cvf::selectedItem)
						{
						case 1:
							cvf::templatePos = cvf::getTemplatePos(cvf::gambleOneHand,1);
							break;
						case 2:
							cvf::templatePos = cvf::getTemplatePos(cvf::gambleQuiver, 1);
							break;
						case 3:
							cvf::templatePos = cvf::getTemplatePos(cvf::gambleMojo, 1);
							break;
						case 4:
							cvf::templatePos = cvf::getTemplatePos(cvf::gambleTwoHand,  1);
							break;
						case 5:
							cvf::templatePos = cvf::getTemplatePos(cvf::gambleOrb, 1);
							break;
						case 6:
							cvf::templatePos = cvf::getTemplatePos(cvf::gamblePhylactery,  1);
							break;
						case 11:
							cvf::templatePos = cvf::getTemplatePos(cvf::gambleHelm,  1);
							break;
						case 12:
							cvf::templatePos = cvf::getTemplatePos(cvf::gambleBoots, 1);
							break;
						case 13:
							cvf::templatePos = cvf::getTemplatePos(cvf::gambleBelt,  1);
							break;
						case 14:
							cvf::templatePos = cvf::getTemplatePos(cvf::gamblePants,  1);
							break;
						case 15:
							cvf::templatePos = cvf::getTemplatePos(cvf::gambleShield,  1);
							break;
						case 16:
							cvf::templatePos = cvf::getTemplatePos(cvf::gambleGloves,  1);
							break;
						case 17:
							cvf::templatePos = cvf::getTemplatePos(cvf::gambleChest,  1);
							break;
						case 18:
							cvf::templatePos = cvf::getTemplatePos(cvf::gambleShoulders,  1);
							break;
						case 19:
							cvf::templatePos = cvf::getTemplatePos(cvf::gambleBracers,  1);
							break;
						case 21:
							cvf::templatePos = cvf::getTemplatePos(cvf::gambleRing,  1);
							break;
						case 22:
							cvf::templatePos = cvf::getTemplatePos(cvf::gambleAmulet, 1);
							break;
						}
						cvf::sendClick(cvf::templatePos.x, cvf::templatePos.y, 'R', 60);
					}
					else {
						MessageBox(
							NULL,  // Parent window, NULL for top-level
							"Please select item to gamble from Kadala",  // Message
							"Error",  // Title
							MB_ICONERROR | MB_OK  // Icon and buttons
						);
					}
				}

				while (GetAsyncKeyState(cvf::hotkeyGambling) & 0x8000) {
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
				}
			}
		}

		//portal skipping
		if (GetForegroundWindow() == cvf::selectedWindowHandle)
		{
			try {
				if (GetAsyncKeyState(cvf::hotkeyPortaling) & 0x8000) {

					cvf::selectedWindowHandle = FindWindow(NULL, cvf::windowName);
					cvf::isRiftTab = false;
					cvf::checkTemplateImage(cvf::riftTab, cvf::isRiftTab);
					if (cvf::isRiftTab) {
						// Start a separate thread to handle portal skipping
						std::thread portalThread(cvf::portalSkipping);
						portalThread.detach();
						while (!cvf::exitLoop.load()) {
							if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
								cvf::exitLoop.store(true);
								std::cout << "ESC key pressed. Stopping portal skipping..." << std::endl;
								break;
							}
							std::this_thread::sleep_for(std::chrono::milliseconds(20));
						}

					}

					while (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
						std::this_thread::sleep_for(std::chrono::milliseconds(20));
					}
				}
			}catch (const std::exception& ex) {
				std::cout << ex.what() << std::endl;  // Handle any exceptions
			}
		}

			
		//enchantment rolling
		if (GetForegroundWindow() == cvf::selectedWindowHandle)
		{
			if (GetAsyncKeyState(cvf::hotkeyEnch) & 0x8000) {

				cvf::selectedWindowHandle = FindWindow(NULL, cvf::windowName);
				cvf::enchNumberResult = 0;
				cvf::desiredEnch = cvf::OCRFromClipboard();
				cvf::desiredEnchParsed = cvf::extractHighestNumberAndText(cvf::desiredEnch);


				if (cvf::desiredEnchParsed.first!=-1 && !cvf::isDefaultRegion(cvf::enchRegion))
				{
					// Start a separate thread to check for ESC key press
					cvf::exitLoop = false;
					std::thread escThread(cvf::checkForESCKeyPress);
					escThread.detach();
					cvf::templatePos = cvf::getTemplatePos(cvf::selectEnch, 1);
					while (cvf::enchNumberResult != -1 && !cvf::exitLoop.load())
					{				

						cvf::rollingEcnhantments();
						cvf::handleOptionClick(cvf::enchRegion, cvf::enchNumberResult);
						cvf::sendClick(cvf::templatePos.x, cvf::templatePos.y, 'L', 2);
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
						cvf::sendClick(cvf::templatePos.x, cvf::templatePos.y, 'L', 2);

						std::this_thread::sleep_for(std::chrono::milliseconds(1500));
						
					}	
					
				}
				else {
					MessageBox(
						NULL,  // Parent window, NULL for top-level
						"Cannot roll enchantments! Please recapture desired enchantment or select region",  // Message
						"Error",  // Title
						MB_ICONERROR | MB_OK  // Icon and buttons
					);
				}
				
				
				while (GetAsyncKeyState(cvf::hotkeyEnch) & 0x8000) {
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
				}
			}
		}


		if (ImGui::BeginTabBar("tabs"))
		{
			if (ImGui::BeginTabItem("Options and Hotkeys"))
			{
				ImGui::Spacing(2);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Set hotkey for item : ");
				ImGui::SameLine();
				if (ImGui::Button("Gambling")) {
					cvf::selectedWindowHandle = FindWindow(NULL, cvf::windowName);
					cvf::exitLoop.store(false);
					std::thread hotkeyThread1(cvf::waitForKeyPress1);
					std::cout << "Key pressed: " << cvf::hotkeyGambling<< std::endl;
					hotkeyThread1.detach();
				}
				ImGui::SameLine();
				std::string gamblingHK = cvf::getKeyName(cvf::hotkeyGambling);
				ImGui::Text("%s", gamblingHK);

				ImGui::Spacing(2);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Set hotkey for portal : ");
				ImGui::SameLine();
				if (ImGui::Button("Skipping")) {
					cvf::selectedWindowHandle = FindWindow(NULL, cvf::windowName);
					cvf::exitLoop.store(false);
					std::thread hotkeyThread2(cvf::waitForKeyPress2);
					std::cout << "Key pressed: " << cvf::hotkeyPortaling << std::endl;
					hotkeyThread2.detach();
				}
				ImGui::SameLine();
				std::string portHK = cvf::getKeyName(cvf::hotkeyPortaling);
				ImGui::Text("%s", portHK);

				ImGui::Spacing(2);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Set hotkey for enchantment : ");
				ImGui::SameLine();
				if (ImGui::Button("Rolling")) {
					cvf::selectedWindowHandle = FindWindow(NULL, cvf::windowName);
					cvf::exitLoop.store(false);
					std::thread hotkeyThread3(cvf::waitForKeyPress3);
					std::cout << "Key pressed: " << cvf::hotkeyEnch << std::endl;
					hotkeyThread3.detach();
				}
				ImGui::SameLine();
				std::string enchHK = cvf::getKeyName(cvf::hotkeyEnch);
				ImGui::Text("%s", enchHK);

				ImGui::Spacing(2);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Select enchantment region");
				ImGui::SameLine();
				if (ImGui::Button("Select")) {
					cvf::enchRegion = cvf::getSelectedRegion();
				}
				ImGui::SameLine();
				if (!cvf::isDefaultRegion(cvf::enchRegion))
				{
					ImGui::Text("Region selected");
				}

				
				ImGui::EndTabItem();
			}
			
			if (ImGui::BeginTabItem("Kadala gambling"))
			{
				float columnWidth = ImGui::GetContentRegionAvail().x / 2;
				
				// Block 1
				ImGui::BeginChild("Block1", ImVec2(columnWidth, 0), false);
								
				ImGui::RadioButton("Gamble Onehand Weapon", &cvf::selectedItem, 1);
				ImGui::RadioButton("Gamble Quiver", &cvf::selectedItem,2);
				ImGui::RadioButton("Gamble Mojo", &cvf::selectedItem,3);
				ImGui::Separator();
				ImGui::RadioButton("Gamble Helm", &cvf::selectedItem,11);
				ImGui::RadioButton("Gamble Boots", &cvf::selectedItem,12);
				ImGui::RadioButton("Gamble Belt", &cvf::selectedItem,13);
				ImGui::RadioButton("Gamble Pants", &cvf::selectedItem,14);
				ImGui::RadioButton("Gamble Shield", &cvf::selectedItem,15);
				ImGui::Separator();
				ImGui::RadioButton("Gamble Ring", &cvf::selectedItem,21);
				ImGui::EndChild();

				ImGui::SameLine();

				// Block 2
				ImGui::BeginChild("Block2", ImVec2(columnWidth, 0), false);
				ImGui::RadioButton("Gamble Twohand Weapon", &cvf::selectedItem,4);
				ImGui::RadioButton("Gamble Orb", &cvf::selectedItem,5);
				ImGui::RadioButton("Gamble Phylactery", &cvf::selectedItem,6);
				ImGui::Separator();
				ImGui::RadioButton("Gamble Gloves", &cvf::selectedItem,16);
				ImGui::RadioButton("Gamble Chest", &cvf::selectedItem,17);
				ImGui::RadioButton("Gamble Shoulders", &cvf::selectedItem,18);
				ImGui::RadioButton("Gamble Bracers", &cvf::selectedItem,19);
				ImGui::Spacing(6);
				ImGui::Separator();
				ImGui::RadioButton("Gamble Amulet", &cvf::selectedItem,22);								

				ImGui::EndChild();

				ImGui::EndTabItem();
			}


			ImGui::EndTabBar();
		}

		ImGui::End();
		gui::EndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return EXIT_SUCCESS;
}