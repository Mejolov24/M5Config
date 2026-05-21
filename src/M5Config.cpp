#include "M5Config.h"
#include <SD.h>
void M5Config::_goBack(){
    if (_stack_index == 0) return;
    _cursor_offset = 0;
    _cursor_index = 0;
    _stack_index --;
    return;}

void M5Config::goToMenu(ConfigMenu* menu, bool append){
    if(append){
        _stack_index ++;
        _menuStack[_stack_index] = menu;
    }
    else{
        _stack_index = 0;
        _menuStack[0] = menu;
    }
    _render();
    return;
}

String M5Config::_formatValue(ConfigItem* item){
    if(item->value_ptr == nullptr )
    {return "";}
    
    switch (item->type){
        case ConfigType::TYPE_SUBMENU:
        case ConfigType::TYPE_FUNCTION:
        return "";
        case ConfigType::TYPE_BOOL:{return String((*static_cast<bool*>(item->value_ptr)) ? "ON" : "OFF");}
        case ConfigType::TYPE_ENUM:{return String(*static_cast<int*>(item->value_ptr));}

        case ConfigType::TYPE_UINT8_T:{return String(*static_cast<uint8_t*>(item->value_ptr));}
        case ConfigType::TYPE_UINT16_T:{return String(*static_cast<uint16_t*>(item->value_ptr));}
        case ConfigType::TYPE_UINT32_T:{return String(*static_cast<uint32_t*>(item->value_ptr));}
        case ConfigType::TYPE_INT8_T:{return String(*static_cast<int8_t*>(item->value_ptr));}
        case ConfigType::TYPE_INT16_T:{return String(*static_cast<int16_t*>(item->value_ptr));}
        case ConfigType::TYPE_INT32_T:{return String(*static_cast<int32_t*>(item->value_ptr));}
    }
    return "";
}

void M5Config::_SetValue(ConfigItem* item, void* value){
    if(item->value_ptr == nullptr )
    {return;}
    
    switch (item->type){
        case ConfigType::TYPE_SUBMENU:
        case ConfigType::TYPE_FUNCTION:
        return;
        case ConfigType::TYPE_BOOL:{*static_cast<bool*>(item->value_ptr) = *static_cast<bool*>(value); return;}
        case ConfigType::TYPE_ENUM:{*static_cast<int*>(item->value_ptr) = *static_cast<bool*>(value); return;}

        case ConfigType::TYPE_UINT8_T:{*static_cast<uint8_t*>(item->value_ptr) = *static_cast<bool*>(value); return;}
        case ConfigType::TYPE_UINT16_T:{*static_cast<uint16_t*>(item->value_ptr) = *static_cast<bool*>(value); return;}
        case ConfigType::TYPE_UINT32_T:{*static_cast<uint32_t*>(item->value_ptr) = *static_cast<bool*>(value); return;}
        case ConfigType::TYPE_INT8_T:{*static_cast<int8_t*>(item->value_ptr) = *static_cast<bool*>(value); return;}
        case ConfigType::TYPE_INT16_T:{*static_cast<int16_t*>(item->value_ptr) = *static_cast<bool*>(value); return;}
        case ConfigType::TYPE_INT32_T:{*static_cast<int32_t*>(item->value_ptr) = *static_cast<bool*>(value); return;}
    }
}

void M5Config::_render(){
    if (_canvas == nullptr) return;
    if (!_active) return;
    _canvas->fillScreen(_theme.background_color);
    _canvas->setTextColor(_theme.text_color);
    uint16_t menu_size = _menuStack[_stack_index]->size;
    uint16_t draw_offset = 0;
    uint16_t selection_cursor = _cursor_offset + _cursor_index;
    uint8_t window_offset = _cursor_offset + _theme.item_window; // until where we are seeing in the vector
    if (menu_size < window_offset){
        window_offset = menu_size;
        if (menu_size != 0) window_offset --;}
    
    for (uint16_t i = _cursor_offset; i <= window_offset; i++){
        if (menu_size == 0) break;
        ConfigItem* item = &_menuStack[_stack_index]->config_items[i];
        String current_item_name = item->name;
        String current_item_value = _formatValue(item);

        if(i == selection_cursor and menu_size != 0){
            _canvas->fillRect(0, draw_offset, _width, _theme.item_height, _theme.selection_color);
        }
    
        _canvas->drawRect(0, draw_offset, _width, _theme.item_height, _theme.border_color);
        _canvas->setTextDatum(TL_DATUM);
        _canvas->drawString(current_item_name, 0, draw_offset, _theme.font);
        canvas.setTextDatum(TR_DATUM);
        _canvas->drawString(current_item_value,_width,draw_offset,_theme.font);
        draw_offset += _theme.item_height;

    }


    _canvas->pushSprite(0,0);

    return;
}

void M5Config::begin(M5Canvas* targetCanvas, SettingInteracted callback){
    _canvas = targetCanvas;
    _callback = callback;
    _width = _canvas->width();
    _height = _canvas->height();
    _half_width = int(_width / 2);
    return;
}

void M5Config::open(){

    _active = true;
    _render();

    return;
}

void M5Config::setTheme(ExplorerTheme* theme){
    if (theme == nullptr) {
        if (!_had_theme) _theme = ExplorerTheme();}
    else {_theme = *theme; _had_theme = true;}
    return;
}

void M5Config::close(){
    _canvas->clear();
    _active = false;
    return;
}

void M5Config::process_input(Input input){
    if (!_active) return;
    uint16_t menu_size = _menuStack[_stack_index]->size;
    String callback_path = "";

    switch (input)
    {
    case UP:
        if (menu_size == 0) break;
        _cursor_index--;
        // If we move off the top of the current window
        if (_cursor_index < 0) {
            if (_cursor_offset > 0) {
                // Scroll the window up
                _cursor_index = 0;
                _cursor_offset--;
            } else {
                // Wrap around to the very end of the list
                _cursor_offset = (menu_size > _theme.item_window) ? (menu_size - (_theme.item_window + 1)) : 0;
                _cursor_index = (menu_size > _theme.item_window) ? _theme.item_window : (menu_size - 1);
            }
        }
        break;

    case DOWN:
        if (menu_size == 0) break;
        _cursor_index++;
        // If we move off the bottom of the current window
        if (_cursor_index > _theme.item_window or (_cursor_offset + _cursor_index) >= menu_size) {
            if ((_cursor_offset + _theme.item_window + 1) < menu_size) {
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

    case (SELECT):
        {ConfigItem current_selection = _menuStack[_stack_index]->config_items[_selection];
        if (menu_size == 0) break;
        
        switch (current_selection.type)
        {
        case ConfigType::TYPE_BOOL:{break;}
        case ConfigType::TYPE_SUBMENU:{break;}
        case ConfigType::TYPE_FUNCTION:{break;}

        default:{break;}
        }

        break;}
    
    case BACK:
        _goBack();
        break;

    default:
    break;
    }
    _selection = _cursor_offset + _cursor_index;
    _render();
    if (!_callback) return;
    //_callback(ConfigItem current_selection = _menuStack[_stack_index]->config_items[_selection]);
}
