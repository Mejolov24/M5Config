#ifndef M5CONFIG_H
#define M5CONFIG_H
#ifndef MAX_DEPTH
    #define MAX_DEPTH 8
#endif
#include <stdint.h>
#include <Arduino.h>
#include <M5GFX.h>
extern M5Canvas canvas;
class M5Config{
    public:
    struct ExplorerTheme{
        uint16_t background_color = BLACK;
        uint16_t border_color = WHITE;
        uint16_t selection_color = BLUE;
        uint16_t text_color = WHITE;
        uint16_t item_height = 16;
        uint16_t item_window = 6;
        const lgfx::v1::IFont* font = &fonts::Font2;
    };

    enum ConfigType{
        TYPE_UINT8_T,
        TYPE_UINT16_T,
        TYPE_UINT32_T,
        TYPE_INT8_T,
        TYPE_INT16_T,
        TYPE_INT32_T,
        TYPE_BOOL,
        TYPE_ENUM,
        TYPE_STRING,
        TYPE_SUBMENU,
        TYPE_FUNCTION
    };

    struct ConfigItem
    {
        ConfigType type;
        const char* name;
        union {
                void* value_ptr;
                void (*function)();
            };
        int increment;
        int upper_limit;
        int lower_limit;
    };
    
    typedef void (*SettingInteracted)(ConfigItem*, int delta); // delta is the absolute increment of the config item, will be 0 if its a callback

    private:
    M5Canvas* _canvas;
    ExplorerTheme _theme;
    SettingInteracted _callback = nullptr;
    uint8_t _width = 0;
    uint8_t _half_width = 0;
    uint8_t _height = 0;
    int8_t _cursor_index = 0; // selection position
    uint16_t _cursor_offset = 0; // real position
    uint16_t _selection = 0; // global pos of selection
    
    ConfigItem* _pageStack[MAX_DEPTH];

    bool _had_theme = false; // set to true forever when default theme get overwritten
    bool _active = false; 
    void _goBack();
    void _render();

    public:

    enum Input{
        UP,
        DOWN,
        LEFT,
        RIGHT,
        BACK,
        SELECT
    };

    void begin(M5Canvas* targetCanvas, SettingInteracted callback);
    void setTheme(ExplorerTheme* theme = nullptr);
    void open(); // opens the file picker, wrapper or _active
    void close(); // closes the file picker
    void goToAbsoluteDir(const char* path);
    void process_input(Input input);
};



#endif