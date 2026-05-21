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
        uint16_t value_color = YELLOW;
        uint16_t bool_true_color = GREEN;
        uint16_t bool_false_color = RED;
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
        TYPE_FLOAT,
        TYPE_BOOL,
        TYPE_ENUM,
        TYPE_SUBMENU,
        TYPE_FUNCTION
    };

    union Increment {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        int8_t i8;
        int16_t i16;
        int32_t i32;
        float f;
    };

    union Pointer {
        void* value_ptr;
        void (*function)();
    };

    struct ConfigItem
    {
        const char* name;
        ConfigType type;
        Pointer pointer;
        Increment increment;
        Increment lower_limit;
        Increment upper_limit;
        
    };
    
    struct ConfigMenu{
        ConfigItem* config_items;
        uint16_t size;
    };

    typedef void (*SettingInteracted)(ConfigItem*);

    private:
    M5Canvas* _canvas;
    ExplorerTheme _theme;
    SettingInteracted _callback = nullptr;
    ConfigMenu* _menuStack[MAX_DEPTH];
    uint8_t _width = 0;
    uint8_t _half_width = 0;
    uint8_t _height = 0;
    int8_t _cursor_index = 0; // selection position
    uint16_t _cursor_offset = 0; // real position
    uint16_t _selection = 0; // global pos of selection
    uint16_t _stack_index = 0;

    bool _had_theme = false; // set to true forever when default theme get overwritten
    bool _active = false; 
    void _goBack();
    void _render();
    void _incrementValue(ConfigItem* item, int8_t delta); // delta is the magnitude of the increment, so either -1 or 1
    String _formatValue(ConfigItem* item);

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
    void goToMenu(ConfigMenu* menu, bool append = false);
    void process_input(Input input);
};



#endif