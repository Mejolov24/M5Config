#ifndef M5CONFIG_H
#define M5CONFIG_H

#ifndef MAX_DEPTH
#define MAX_DEPTH 8
#endif

#include <stdint.h>
#include <type_traits>
#include <Arduino.h>
#include <M5GFX.h>

extern M5Canvas canvas;

class M5Config
{
public:

    struct ConfigMenu;
    struct ConfigItem;
    using SettingInteracted = void (*)(ConfigItem*);

    struct ExplorerTheme {
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

    enum class Input {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        BACK,
        SELECT
    };

    // lower level stuff from now on:

    enum class ValueType {
        TYPE_UINT8_T,
        TYPE_UINT16_T,
        TYPE_UINT32_T,
        TYPE_INT8_T,
        TYPE_INT16_T,
        TYPE_INT32_T,
        TYPE_BOOL,
        TYPE_SUBMENU,
        TYPE_FUNCTION
    };

    enum class ScrollType {
        TYPE_CYCLIC,// range 0-100 | 100 + 10 = 10  (modulo)
        TYPE_WRAP,  // range 0-100 | 100 + 10 = 0   (teleport)
        TYPE_CLAMP  // range 0-100 | 100 + 10 = 100 (lock)
    };

    union IncrementType {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        int8_t i8;
        int16_t i16;
        int32_t i32;
        float f;
    };

    union Pointer {
        void* data;
        void (*function)();
    };

    struct ConfigItem
    {
        const char* name;
        ValueType type;

        Pointer pointer;
        IncrementType increment;
        IncrementType lower_limit;
        IncrementType upper_limit;

        bool include_max;

        // Generic constructor
        template<typename T, typename A, typename B, typename C>
        ConfigItem(
            const char* n,
            T* ptr,
            A inc,
            B min,
            C max,
            bool incl = true // TODO : replace incl for implementation of proper enum of Scroll type
        )
            : name(n),
              type(ValueType::TYPE_UINT8_T), // temporary, fixed below
              include_max(incl)
        {
            pointer.data = ptr;

            if constexpr (std::is_same<T, uint8_t>::value) {
                type = ValueType::TYPE_UINT8_T;
                increment.u8 = inc;
                lower_limit.u8 = min;
                upper_limit.u8 = max;
            }
            else if constexpr (std::is_same<T, uint16_t>::value) {
                type = ValueType::TYPE_UINT16_T;
                increment.u16 = inc;
                lower_limit.u16 = min;
                upper_limit.u16 = max;
            }
            else if constexpr (std::is_same<T, uint32_t>::value) {
                type = ValueType::TYPE_UINT32_T;
                increment.u32 = inc;
                lower_limit.u32 = min;
                upper_limit.u32 = max;
            }
            else if constexpr (std::is_same<T, int8_t>::value) {
                type = ValueType::TYPE_INT8_T;
                increment.i8 = inc;
                lower_limit.i8 = min;
                upper_limit.i8 = max;
            }
            else if constexpr (std::is_same<T, int16_t>::value) {
                type = ValueType::TYPE_INT16_T;
                increment.i16 = inc;
                lower_limit.i16 = min;
                upper_limit.i16 = max;
            }
            else if constexpr (std::is_same<T, int32_t>::value) {
                type = ValueType::TYPE_INT32_T;
                increment.i32 = inc;
                lower_limit.i32 = min;
                upper_limit.i32 = max;
            }
            else {
                static_assert(!std::is_same<T, T>::value,
                              "Unsupported ConfigItem type");
            }
        }

        // function constructor

        ConfigItem(const char* n, void (*func)())
            : name(n),
              type(ValueType::TYPE_FUNCTION),
              include_max(true)
        {
            pointer.function = func;
        }

        // bool constructor

        ConfigItem(const char* n, bool* ptr)
            : name(n),
              type(ValueType::TYPE_BOOL),
              include_max(true)
        {
            pointer.data = ptr;
            increment.u8 = 1;
            lower_limit.u8 = 0;
            upper_limit.u8 = 1;
        }

        //submenu constructor
        ConfigItem(const char* n, ConfigMenu* submenu)
            : name(n),
              type(ValueType::TYPE_SUBMENU),
              include_max(true)
        {
            pointer.data = submenu;
        }
    };

    struct ConfigMenu {
        ConfigItem* config_items;
        uint16_t size;
    };


private:
    // internal engine stuff
    M5Canvas* _canvas;

    ExplorerTheme _theme;
    SettingInteracted _callback = nullptr;

    ConfigMenu* _menuStack[MAX_DEPTH];

    uint8_t _width = 0;
    uint8_t _half_width = 0;
    uint8_t _height = 0;

    int8_t _cursor_index = 0;
    uint16_t _cursor_offset = 0;
    uint16_t _selection = 0;
    uint16_t _stack_index = 0;

    bool _had_theme = false;
    bool _active = false;

    void _goBack();
    void _render();
    void _incrementValue(ConfigItem* item, int8_t delta);
    String _formatValue(ConfigItem* item);

    public:

    void begin(M5Canvas* targetCanvas, SettingInteracted callback = nullptr);
    void setTheme(ExplorerTheme* theme = nullptr);
    void open();
    void close();
    void goToMenu(ConfigMenu* menu, bool append = false); // if not append, override menu stack.
    void process_input(Input input);
};

#endif