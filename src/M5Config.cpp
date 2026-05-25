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
    _cursor_offset = 0;
    _cursor_index = 0;
    _render();
    return;
}

String M5Config::_formatValue(ConfigItem* item){
    if(item->pointer.data == nullptr )
    {return "";}
    
    switch (item->type){
        case ConfigType::TYPE_SUBMENU:
        case ConfigType::TYPE_FUNCTION:
        return "";
        case ConfigType::TYPE_BOOL:{return String((*static_cast<bool*>(item->pointer.data)) ? "On" : "Off");}
        case ConfigType::TYPE_UINT8_T:{return String(*static_cast<uint8_t*>(item->pointer.data));}
        case ConfigType::TYPE_UINT16_T:{return String(*static_cast<uint16_t*>(item->pointer.data));}
        case ConfigType::TYPE_UINT32_T:{return String(*static_cast<uint32_t*>(item->pointer.data));}
        case ConfigType::TYPE_INT8_T:{return String(*static_cast<int8_t*>(item->pointer.data));}
        case ConfigType::TYPE_INT16_T:{return String(*static_cast<int16_t*>(item->pointer.data));}
        case ConfigType::TYPE_INT32_T:{return String(*static_cast<int32_t*>(item->pointer.data));}
    }
    return "";
}

void M5Config::_incrementValue(ConfigItem* item, int8_t delta){
    if(item->pointer.data == nullptr) return;
    
    switch (item->type){
        case ConfigType::TYPE_SUBMENU:
        case ConfigType::TYPE_FUNCTION:
        return;
        case ConfigType::TYPE_BOOL:{
            bool* value = static_cast<bool*>(item->pointer.data);
            *value = !(*value);
            return;
        }

        case ConfigType::TYPE_UINT8_T:{
            uint8_t* value = static_cast<uint8_t*>(item->pointer.data);
            int16_t increment = item->increment.u8 * delta;
            uint8_t min = item->lower_limit.u8;
            uint8_t max = item->upper_limit.u8;
            int16_t temp = *value + increment;
            int16_t range = max - min;
            if (range == 0) return; // Prevent division by zero
            if (temp > max) {
                int16_t overshoot = temp - max;
                // If overshoot is larger than the range, modulo drops it into a single loop
                temp = min + (overshoot % range);
            }
            else if (temp < min) {
                int16_t undershoot = min - temp;
                temp = max - (undershoot % range);
            }

            *value = (uint8_t)temp;
            return;
        }

        case ConfigType::TYPE_UINT16_T:{
            uint16_t* value = static_cast<uint16_t*>(item->pointer.data);
            int32_t increment = item->increment.u16 * delta;
            uint16_t min = item->lower_limit.u16;
            uint16_t max = item->upper_limit.u16;
            int32_t temp = *value + increment;
            int32_t range = max - min;
            if (range == 0) return; 
            if (temp > max) {
                int32_t overshoot = temp - max;
                temp = min + (overshoot % range);
            }
            else if (temp < min) {
                int32_t undershoot = min - temp;
                temp = max - (undershoot % range);
            }
            *value = (uint16_t)temp;
            return;
        }

        case ConfigType::TYPE_UINT32_T:{
            uint32_t* value = static_cast<uint32_t*>(item->pointer.data);
            int64_t increment = item->increment.u32 * delta;
            uint32_t min = item->lower_limit.u32;
            uint32_t max = item->upper_limit.u32;
            int64_t temp = *value + increment;
            int64_t range = max - min;
            if (range == 0) return; 
            if (temp > max) {
                int64_t overshoot = temp - max;
                temp = min + (overshoot % range);
            }
            else if (temp < min) {
                int64_t undershoot = min - temp;
                temp = max - (undershoot % range);
            }
            *value = (uint32_t)temp;
            return;
        }
        case ConfigType::TYPE_INT8_T:{
            int8_t* value = static_cast<int8_t*>(item->pointer.data);
            int16_t increment = item->increment.i8 * delta;
            int8_t min = item->lower_limit.i8;
            int8_t max = item->upper_limit.i8;
            int16_t temp = *value + increment;
            int16_t range = max - min;
            if (range == 0) return; 
            if (temp > max) {
                int16_t overshoot = temp - max;
                temp = min + (overshoot % range);
            }
            else if (temp < min) {
                int16_t undershoot = min - temp;
                temp = max - (undershoot % range);
            }
            *value = (int8_t)temp;
            return;
        }
        case ConfigType::TYPE_INT16_T:{
            int16_t* value = static_cast<int16_t*>(item->pointer.data);
            int32_t increment = item->increment.i16 * delta;
            int16_t min = item->lower_limit.i16;
            int16_t max = item->upper_limit.i16;
            int32_t temp = *value + increment;
            int32_t range = max - min;
            if (range == 0) return; 
            if (temp > max) {
                int32_t overshoot = temp - max;
                temp = min + (overshoot % range);
            }
            else if (temp < min) {
                int32_t undershoot = min - temp;
                temp = max - (undershoot % range);
            }
            *value = (int16_t)temp;
            return;
        }
        case ConfigType::TYPE_INT32_T:{
            int32_t* value = static_cast<int32_t*>(item->pointer.data);
            int64_t increment = item->increment.i16 * delta;
            int32_t min = item->lower_limit.i16;
            int32_t max = item->upper_limit.i16;
            int64_t temp = *value + increment;
            int64_t range = max - min;
            if (range == 0) return; 
            if (temp > max) {
                int64_t overshoot = temp - max;
                temp = min + (overshoot % range);
            }
            else if (temp < min) {
                int64_t undershoot = min - temp;
                temp = max - (undershoot % range);
            }
            *value = (int32_t)temp;
            return;
        }
        }
    }
void M5Config::_render(){
    if (_canvas == nullptr) return;
    if (!_active) return;
    _canvas->fillScreen(_theme.background_color);
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
        uint16_t value_color = _theme.value_color;
        if(item->type == ConfigType::TYPE_BOOL){value_color = (*(bool*)item->pointer.data) ? _theme.bool_true_color : _theme.bool_false_color;}

        if(i == selection_cursor and menu_size != 0){
            _canvas->fillRect(0, draw_offset, _width, _theme.item_height, _theme.selection_color);
        }
    
        _canvas->drawRect(0, draw_offset, _width, _theme.item_height, _theme.border_color);
        _canvas->setTextDatum(TL_DATUM);
        _canvas->setTextColor(_theme.text_color);
        _canvas->drawString(current_item_name, 0, draw_offset, _theme.font);
        canvas.setTextDatum(TR_DATUM);
        _canvas->setTextColor(value_color);
        _canvas->drawString(current_item_value,_width,draw_offset,_theme.font);
        _canvas->setTextDatum(TL_DATUM);
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
    ConfigItem current_selection = _menuStack[_stack_index]->config_items[_selection];
    bool ran_function = false;
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
    
    case(LEFT):{
        _incrementValue(&current_selection,-1);
        break;
    }
    case(RIGHT):{
        _incrementValue(&current_selection,1);
        break;
    }

    case (SELECT):
        {
        if (menu_size == 0) break;
        
        switch (current_selection.type)
        {
        case ConfigType::TYPE_BOOL:{_incrementValue(&current_selection,1); break;}
        case ConfigType::TYPE_SUBMENU:{
            ConfigMenu* menu =
                static_cast<ConfigMenu*>(current_selection.pointer.data);

            goToMenu(menu, true);
            break;
        }
        case ConfigType::TYPE_FUNCTION:{current_selection.pointer.function(); ran_function = true; break;}

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
    if (not ran_function) _render();
    if (!_callback) return;
    _callback(&current_selection);
}
