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
    if(item->value_ptr == nullptr )
    {return "";}
    
    switch (item->type){
        case ConfigType::TYPE_SUBMENU:
        case ConfigType::TYPE_FUNCTION:
        return "";
        case ConfigType::TYPE_BOOL:{return String((*static_cast<bool*>(item->value_ptr)) ? "On" : "Off");}
        case ConfigType::TYPE_ENUM:{return String(*static_cast<int*>(item->value_ptr));}

        case ConfigType::TYPE_UINT8_T:{return String(*static_cast<uint8_t*>(item->value_ptr));}
        case ConfigType::TYPE_UINT16_T:{return String(*static_cast<uint16_t*>(item->value_ptr));}
        case ConfigType::TYPE_UINT32_T:{return String(*static_cast<uint32_t*>(item->value_ptr));}
        case ConfigType::TYPE_INT8_T:{return String(*static_cast<int8_t*>(item->value_ptr));}
        case ConfigType::TYPE_INT16_T:{return String(*static_cast<int16_t*>(item->value_ptr));}
        case ConfigType::TYPE_INT32_T:{return String(*static_cast<int32_t*>(item->value_ptr));}
        case ConfigType::TYPE_FLOAT:{
            char buf[16];
            float val = *static_cast<float*>(item->value_ptr);
            snprintf(buf, sizeof(buf), "%g", val);
            return buf;
        }
    }
    return "";
}

void M5Config::_incrementValue(ConfigItem* item, int8_t delta){
    if(item->value_ptr == nullptr) return;
    
    switch (item->type){
        case ConfigType::TYPE_SUBMENU:
        case ConfigType::TYPE_FUNCTION:
        return;
        case ConfigType::TYPE_BOOL:{
            bool* value = static_cast<bool*>(item->value_ptr);
            *value = !(*value);
            return;
        }
        case ConfigType::TYPE_ENUM:{
            int* value = static_cast<int*>(item->value_ptr);
            int increment = (int)((uint)(uintptr_t)item->increment) * delta;
            int min = (int)(uintptr_t)item->lower_limit;
            int max = (int)(uintptr_t)item->upper_limit;
            int range = (max - min) + 1;
            int temp = *value + increment;
            temp = min + (((temp - min) % range) + range) % range;
            *value = (int)temp;
            return;}

        case ConfigType::TYPE_UINT8_T:{
            uint8_t* value = static_cast<uint8_t*>(item->value_ptr);
            int increment = (int)((uint8_t)(uintptr_t)item->increment) * delta;
            int min = (uint8_t)(uintptr_t)item->lower_limit;
            int max = (uint8_t)(uintptr_t)item->upper_limit;
            int range = (max - min) + 1;
            int temp = *value + increment;
            temp = min + (((temp - min) % range) + range) % range;
            *value = (uint8_t)temp;
            return;}


        case ConfigType::TYPE_UINT16_T:{
            uint16_t* value = static_cast<uint16_t*>(item->value_ptr);
            int increment = (int)((uint16_t)(uintptr_t)item->increment) * delta;
            int min = (uint16_t)(uintptr_t)item->lower_limit;
            int max = (uint16_t)(uintptr_t)item->upper_limit;
            int range = (max - min) + 1;
            int temp = *value + increment;
            temp = min + (((temp - min) % range) + range) % range;
            *value = (uint16_t)temp;
            return;}
        case ConfigType::TYPE_UINT32_T:{
            uint32_t* value = static_cast<uint32_t*>(item->value_ptr);
            int64_t increment = (int64_t)((uint32_t)(uintptr_t)item->increment) * delta;
            int64_t min = (uint32_t)(uintptr_t)item->lower_limit;
            int64_t max = (uint32_t)(uintptr_t)item->upper_limit;
            int64_t range = (max - min) + 1;
            int64_t temp = *value + increment;
            temp = min + (((temp - min) % range) + range) % range;
            *value = (uint32_t)temp;
            return;}

        case ConfigType::TYPE_INT8_T:
            {
            int8_t* value = static_cast<int8_t*>(item->value_ptr);
            int increment = (int)((int8_t)(uintptr_t)item->increment) * delta;
            int min = (int8_t)(uintptr_t)item->lower_limit;
            int max = (int8_t)(uintptr_t)item->upper_limit;
            int range = (max - min) + 1;
            int temp = *value + increment;
            temp = min + (((temp - min) % range) + range) % range;
            *value = (int8_t)temp;
            return;}

        case ConfigType::TYPE_INT16_T:{
            int16_t* value = static_cast<int16_t*>(item->value_ptr);
            int increment = (int)((int16_t)(uintptr_t)item->increment) * delta;
            int min = (int16_t)(uintptr_t)item->lower_limit;
            int max = (int16_t)(uintptr_t)item->upper_limit;
            int range = (max - min) + 1;
            int temp = *value + increment;
            temp = min + (((temp - min) % range) + range) % range;
            *value = (int16_t)temp;
            return;}

        case ConfigType::TYPE_INT32_T:{
            int32_t* value = static_cast<int32_t*>(item->value_ptr);
            int64_t increment = (int64_t)((int32_t)(uintptr_t)item->increment) * delta;
            int64_t min = (int32_t)(uintptr_t)item->lower_limit;
            int64_t max = (int32_t)(uintptr_t)item->upper_limit;
            int64_t range = (max - min) + 1;
            int64_t temp = *value + increment;
            temp = min + (((temp - min) % range) + range) % range;
            *value = (int32_t)temp;
            return;}
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
        if(item->type == ConfigType::TYPE_BOOL){value_color = (*(bool*)item->value_ptr) ? _theme.bool_true_color : _theme.bool_false_color;}

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
                static_cast<ConfigMenu*>(current_selection.value_ptr);

            goToMenu(menu, true);
            break;
        }
        case ConfigType::TYPE_FUNCTION:{current_selection.function(); ran_function = true; break;}

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
