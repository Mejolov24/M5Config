# M5SDE
high-level SD card explorer for M5Stack devices.

![thumbnail](https://github.com/Mejolov24/M5SDE/blob/main/thumbnail.jpeg?raw=true)

## More organized files for Everyone
 #### Do you have thousands of folders on your M5Stack device’s SD card?

I do too, and managing them can become messy quickly.
That’s why I propose a simple convention: store application data inside a shared /AppData directory.

This approach keeps SD cards cleaner, makes projects easier to manage, and helps applications coexist without cluttering the root directory.

 - When making a proyect, store its files under /AppData

 - If you have common files you share across or ar commonly used, store them at /AppData/Common, for example /AppData/Common/Music

## Roadmap
Version 1.0.0 covers all current production requirements for my specific workflow. While I have designed a broader feature set, those updates will be worked on as the library gains traction.

Want more features? If you are using this library and need a feature, please let me know by opening a GitHub Issue. I am happy to accelerate the update schedule for active users.

- [x] Basic directory listing
- [x] Recursive folder navigation
- [x] Custom color themes and fonts
- [ ] Search/Filter functionality
- [ ] Multiple selection
- [ ] Folder selection boolean

## Features
* Recursive Directory Navigation: Smoothly browse folders and subfolders.

* Customizable Callbacks: Trigger specific code when a file is selected 

* Customizable Theme and Settings: Use bigger or smaller text to fit your needs.
## Installation

### PlatformIO
Add the following to your `platformio.ini`:
```ini
lib_deps = 
    https://github.com/Mejolov24/M5SDE
```
## Usage
### Pre requisites:
 - Before using M5SDE, the SD card must be initialized.

### begin()
#### Arguments:
targetCanvas : pointer to a canvas object
callback : funtion to call after picking a file

### setTheme()
## Arguments:
theme : pointer of a theme struct

### open()
opens the file picker

### close()
closes the file picker

### goToAbsoluteDir(const char* path);
jumps to specific path at sd card, for example /Appdata/Cardstudio

### process_input();
handle inputs such as up, down, select and back

## Usage Example:
```cpp
#include #include <M5Unified.h>
#include <SD.h>
#include <M5SDE.h>

M5Canvas canvas(&M5.Lcd);
M5SDE sdex;
using input = M5SDE::Input;

void OnSelection(const char* path){// returns the absolute path of selected item

}

void setup(){
    canvas.createSprite();
    SPI.begin();
    // TODO : SD arguments of your device and sprite size

// Custom theme and settings:
M5SDE::ExplorerTheme theme;
theme.background_color = 0x211a;
theme.border_color = 0x2c9f;
theme.selection_color = 0x06e0;
theme.item_height = 23;
theme.item_window = 4;
theme.font = &fonts::FreeSans12pt7b;

sdex.begin(&canvas,OnSelection);
sdex.setTheme(&theme); // if an argument is not providen, it will use defualts or latest config.
sdex.goToAbsoluteDir("/MyDir");
sdex.open();
}

void loop(){
// TODO : send input via sdex.process_input(input::)
}
```
## License
This project is licensed under the MIT License.
