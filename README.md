# D3Helper

D3Helper is a desktop application designed to automate various aspects of gameplay in Diablo 3, such as:
- Purchasing items
- Locating portals
- Enchanting items

The application leverages advanced technologies like computer vision and OCR to analyze game scenes and perform actions accurately. The user interface is built using **ImGui**, ensuring a lightweight and customizable experience.

## Key Features

- **Portal Detection**: Automatically identifies and distinguishes between blue and orange portals.
- **Item Purchasing**: Simplifies bulk item purchases from vendors.
- **Enchanting**: Streamlines the enchantment process for gear.
- **Ease of Installation**: An installer created with **Inno Setup** packages all dependencies and reference images, ensuring a seamless installation experience.

## Technologies Used

- **OpenCV**: For computer vision tasks such as image processing and feature detection.
- **Tesseract OCR**: For text recognition in game scenes.
- **ImGui**: For an intuitive and customizable user interface.
- **Inno Setup**: To bundle all dependencies, images, and resources into a convenient installer.

## Installation

1. Download the installer from the [release page](#).
2. Run the installer and follow the on-screen instructions.
3. Launch the application from the desktop shortcut or start menu.

## Usage

### Detecting Portals
- **Blue Portal Detection**
  ![Blue Portal Detection]([https://imgur.com/gallery/blue-portal-y3mdyWG](https://i.imgur.com/y3mdyWG_lq.mp4))

- **Orange Portal Detection**
  ![Orange Portal Detection](#)

### Purchasing Items
Automates the item purchasing process from vendors.
![Item Purchasing](#)

### Enchanting Gear
Simplifies enchantment by automating repetitive steps.
![Enchanting Gear](#)

### NPC Detection
Uses SIFT to identify NPCs like Orrek.
![NPC Detection](#)

## System Requirements

- **OS**: Windows 10/11
- **CPU**: Intel Core i5 or equivalent
- **RAM**: 8 GB or more
- **Disk Space**: 100 MB free
- **DirectX9** or higher

## Dependencies

The application includes the following libraries and dependencies:
- OpenCV
- Tesseract OCR
- ImGui
- Required image reference files

All dependencies are packaged within the installer for user convenience.

## Known Issues

- The accuracy of portal detection may vary with low-resolution game settings.
- Some portal detection actions may require manual intervention in rare cases.

## License

This project is licensed under the [MIT License](LICENSE).


---

Enjoy automating your Diablo 3 experience with D3Helper!
