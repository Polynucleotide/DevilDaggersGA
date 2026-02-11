# Devil Daggers Genetic Algorithm
Genetic Algorithm learns how to play Devil Daggers in C++ with OpenCV.  

## Demo Video
[Watch Demo](https://www.youtube.com/watch?v=WXQQ8S-K4l0)

## Pipeline
1. DXCam captures the game frame
2. The frame is processed by the OpenCV DNN module
3. The DNN detects skulls and outputs their screen positions and bounding boxes
4. Genetic Algorithm receives the data and makes a decision
5. Repeat from Step 1 at 60 FPS

## Important Notice - Interception Driver  
The Interception driver operates at a low level in the operating system (kernel level).  
While generally stable, installing or configuring it incorrectly may lead to system instability or unresponsiveness on some machines.  
**Please proceed with caution and ensure you have a recovery plan.**

## Prerequisites
- Windows OS
- Interception Driver
- Devil Daggers https://store.steampowered.com/app/422970/Devil_Daggers/

## Interception Driver Installation (Windows)
1. Open Command Prompt as Administrator  
2. Navigate to the "interception_installer\" folder  
3. Enter "install-interception.exe /install" in the Command Prompt  
4. You should see "Interception successfully installed. You must reboot for it to take effect."  
4. Reboot your PC  
5. Confirm that "C:\Windows\System32\drivers" contains "keyboard.sys" and "mouse.sys"  

## Interception Driver Uninstallation (Windows)
1. Open Command Prompt as Administrator  
2. Navigate to the "interception_installer\" folder  
3. Enter "install-interception.exe /uninstall" in the Command Prompt  
4. You should see "Interception uninstalled. You must reboot for this to take effect."  
4. Reboot your PC  
5. Confirm that "keyboard.sys" and "mouse.sys" is gone from "C:\Windows\System32\drivers"  

## How to run the Genetic Algorithm
1. Go to your Graphics Settings (Search in Start Menu)  
2. Ensure that the .exe is using Integrated Graphics  
3. Launch Devil Daggers  
4. Devil Daggers must run in windowed mode  
5. Play the game  
6. Press Ctrl + P to toggle the Genetic Algorithm on/off  