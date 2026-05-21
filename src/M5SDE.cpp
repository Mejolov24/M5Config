#include "M5SDE.h"
#include <SD.h>
String M5SDE::_buildPath() {
    String path = "";
    uint16_t stack_size = _pathStack.size();
    if (stack_size == 0) return "/";
    for (size_t i = 0; i < stack_size; i++) {
        path += "/" + _pathStack[i];
    }
    return path;
}

void M5SDE::_goToDir(String dir_name){
    _cursor_offset = 0;
    _cursor_index = 0;
    _pathStack.push_back(dir_name);
    _updateDirectoryList();
    return;
}

void M5SDE::goToAbsoluteDir(const char* path) {
    _cursor_offset = 0;
    _cursor_index = 0;
    _pathStack.clear();

    if (!path || path[0] == '\0') return;

    const char* start = path;

    if (*start == '/') start++;

    const char* ptr = start;

    while (*ptr) {
        if (*ptr == '/') {
            if (ptr > start) {
                _pathStack.push_back(String(start).substring(0, ptr - start));
            }
            start = ptr + 1;
        }
        ptr++;
    }

    if (ptr > start) {
        _pathStack.push_back(String(start).substring(0, ptr - start));
    }

    _updateDirectoryList();
}

void M5SDE::_goBack(){
    if (_pathStack.size() == 0) return;
    _cursor_offset = 0;
    _cursor_index = 0;
    _pathStack.pop_back();
    _updateDirectoryList();
    return;
}

void M5SDE::_updateDirectoryList() {
    String path = _buildPath();
    _dirList.clear();
    _isDirectory.clear();

    File root = SD.open(path);
    if (!root) {
        _has_dirs = false;
        return;
    }
    else _has_dirs = true;

    File file = root.openNextFile();
    while(file){
        if (file.isDirectory()){
            _isDirectory.push_back(true);
        }
        else{
            _isDirectory.push_back(false);
        }
        _dirList.push_back(String(file.name()));
        file = root.openNextFile();
    }
    root.close();
}


void M5SDE::_render(){
    if (_canvas == nullptr) return;
    if (!_active) return;
    _canvas->fillScreen(_theme.background_color);
    _canvas->drawRect(0, 0, _width, _theme.item_height, _theme.directory_color);
    _canvas->setTextColor(_theme.text_color);
    _canvas->setTextDatum(TR_DATUM);
    if (_pathStack.size() == 0) {_canvas->drawString(_buildPath(), _half_width , 0, _theme.font);}
    else{_canvas->drawString(_buildPath(), _width, 0, _theme.font);}
    _canvas->setTextDatum(TL_DATUM);

    uint16_t files_amount = _dirList.size();
    uint16_t draw_offset = _theme.item_height;
    uint16_t selection_cursor = _cursor_offset + _cursor_index;
    uint8_t window_offset = _cursor_offset + _theme.item_window; // until where we are seeing in the vector
    if (files_amount < window_offset){
        window_offset = files_amount;
        if (files_amount != 0) window_offset --;}
    
    
    for (uint16_t i = _cursor_offset; i <= window_offset; i++){
        if (files_amount == 0) break;
        String current_item_name = _dirList[i];
        bool is_dir = _isDirectory[i];

        if(is_dir) current_item_name = "/" + current_item_name;
        if(i == selection_cursor and files_amount != 0){
            _canvas->fillRect(0, draw_offset, _width, _theme.item_height, _theme.selection_color);
        }
    
        _canvas->drawRect(0, draw_offset, _width, _theme.item_height, _theme.border_color);
        _canvas->drawString(current_item_name, 0, draw_offset, _theme.font);
        draw_offset += _theme.item_height;

    }


    _canvas->pushSprite(0,0);

    return;
}

void M5SDE::begin(M5Canvas* targetCanvas, SelectionCallback callback){
    _canvas = targetCanvas;
    _callback = callback;
    _width = _canvas->width();
    _height = _canvas->height();
    _half_width = int(_width / 2);
    return;
}

void M5SDE::open(){
    if(!_has_dirs) _updateDirectoryList();
    if (!_has_dirs) return;

    _active = true;
    _render();

    return;
}

void M5SDE::setTheme(ExplorerTheme* theme){
    if (theme == nullptr) {
        if (!_had_theme) _theme = ExplorerTheme();}
    else {_theme = *theme; _had_theme = true;}
    return;
}

void M5SDE::close(){
    _canvas->clear();
    _active = false;
    return;
}

void M5SDE::process_input(Input input){
    if (!_active) return;
    uint16_t files_amount = (int16_t)_dirList.size();
    String callback_path = "";

    switch (input)
    {
    case Input::up :
        if (files_amount == 0) break;
        _cursor_index--;
        // If we move off the top of the current window
        if (_cursor_index < 0) {
            if (_cursor_offset > 0) {
                // Scroll the window up
                _cursor_index = 0;
                _cursor_offset--;
            } else {
                // Wrap around to the very end of the list
                _cursor_offset = (files_amount > _theme.item_window) ? (files_amount - (_theme.item_window + 1)) : 0;
                _cursor_index = (files_amount > _theme.item_window) ? _theme.item_window : (files_amount - 1);
            }
        }
        break;

    case Input::down :
        if (files_amount == 0) break;
        _cursor_index++;
        // If we move off the bottom of the current window
        if (_cursor_index > _theme.item_window or (_cursor_offset + _cursor_index) >= files_amount) {
            if ((_cursor_offset + _theme.item_window + 1) < files_amount) {
                // Scroll the window down
                _cursor_index = _theme.item_window;
                _cursor_offset++;
            } else {
                // Wrap around to the very beginning
                _cursor_offset = 0;
                _cursor_index = 0;
            }
        }
        break;

    case (Input::select):
        {String current_selection = _dirList[_selection];
        if (files_amount == 0) break;
        if(!current_selection) break;
        if(_isDirectory[_selection]){
            _goToDir(current_selection);
        }
        else{
            if (_pathStack.size() == 0){callback_path = "/" + current_selection;} // at root dir
            else{callback_path = _buildPath() + "/" + current_selection;}
            }
        break;}
    
    case Input::back:
        _goBack();
        break;

    default:
    break;
    }
    _selection = _cursor_offset + _cursor_index;
    _render();
    if (!_callback or callback_path.isEmpty()) return;
    char char_path[callback_path.length() + 1];
    strcpy(char_path, callback_path.c_str());
    _callback(char_path);
}
