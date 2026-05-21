#include <M5Cardputer.h>
M5Canvas canvas(&M5.Lcd);
#include <M5Config.h>
M5Config config;

uint8_t item1 = 0;
uint16_t item2 = 0;
uint32_t item3 = 0;
int8_t item4 = 0;
int16_t item5 = 0;
int32_t item6 = 0;
bool item7 = false;
uint8_t item8 = 0;
uint8_t item9 = 0;

uint8_t item11 = 0; // can also be declared like this
uint8_t item11_increment = 1;
uint8_t item11_min = 0;
uint8_t item11_max = 5;

float item12 = 0.0f;

void item10(){
    canvas.clear();
    canvas.drawString("Hi, this is a function,", 0,0,&fonts::Font4);
    canvas.drawString("Menu isnt auto closed", 0,32,&fonts::Font4);
    canvas.drawString("You must call close().", 0,64,&fonts::Font4);
    canvas.pushSprite(0,0);
}


M5Config::ConfigItem configs2[] = {
    {
        .name = "0-10", 
        .type = config.TYPE_UINT8_T, 
        .pointer = {.data = &item8}, 
        .increment = {.u8 = 1}, .lower_limit = {.u8 = 0}, .upper_limit = {.u8 = 10}
    },
    {
        .name = "0-100, +10", 
        .type = config.TYPE_UINT8_T, 
        .pointer = {.data = &item9}, 
        .increment = {.u8 = 10}, .lower_limit = {.u8 = 0}, .upper_limit = {.u8 = 100}
    },
    {
        .name = "function",   
        .type = config.TYPE_FUNCTION, 
        .pointer = {.function = item10} // Sets the function pointer slot
    },
    {
        .name = "declaration 2", 
        .type = config.TYPE_UINT8_T, 
        .pointer = {.data = &item11}, 
        .increment = {.u8 = item11_increment}, .lower_limit = {.u8 = item11_min}, .upper_limit = {.u8 = item11_max}
    }
};

M5Config::ConfigMenu menu2 = {
    .config_items = configs2, 
    .size = sizeof(configs2) / sizeof(configs2[0])
};

M5Config::ConfigItem configs[] = {
    {
        .name = "uint8_t",  
        .type = config.TYPE_UINT8_T,  
        .pointer = {.data = &item1}, 
        .increment = {.u8 = 1}, .lower_limit = {.u8 = 0}, .upper_limit = {.u8 = 255}
    },
    {
        .name = "uint16_t", 
        .type = config.TYPE_UINT16_T, 
        .pointer = {.data = &item2}, 
        .increment = {.u16 = 1}, .lower_limit = {.u16 = 0}, .upper_limit = {.u16 = 65535}
    },
    {
        .name = "uint32_t", 
        .type = config.TYPE_UINT32_T, 
        .pointer = {.data = &item3}, 
        .increment = {.u32 = 1}, .lower_limit = {.u32 = 0}, .upper_limit = {.u32 = 4294967295}
    },
    {
        .name = "int8_t",   
        .type = config.TYPE_INT8_T,   
        .pointer = {.data = &item4}, 
        .increment = {.i8 = 1}, .lower_limit = {.i8 = -128}, .upper_limit = {.i8 = 127}
    },
    {
        .name = "int16_t",  
        .type = config.TYPE_INT16_T,  
        .pointer = {.data = &item5}, 
        .increment = {.i16 = 1}, .lower_limit = {.i16 = -32768}, .upper_limit = {.i16 = 32767}
    },
    {
        .name = "int32_t",  
        .type = config.TYPE_INT32_T,  
        .pointer = {.data = &item6}, 
        .increment = {.i32 = 1}, .lower_limit = {.i32 = -2147483648}, .upper_limit = {.i32 = 2147483647}
    },
    {
        .name = "bool",     
        .type = config.TYPE_BOOL,     
        .pointer = {.data = &item7}, 
        .increment = {.u8 = 1}, .lower_limit = {.u8 = 0}, .upper_limit = {.u8 = 1}
    },
    {
        .name = "sub menu", 
        .type = config.TYPE_SUBMENU,  
        .pointer = {.data = &menu2}
    }
};

M5Config::ConfigMenu menu = {
    .config_items = configs, 
    .size = sizeof(configs) / sizeof(configs[0])
};

void OnUsage(M5Config::ConfigItem*){
    return;
}

void setup(){
    auto cfg = M5.config();
    M5Cardputer.begin(cfg);
    canvas.createSprite(240, 135);
    bool sd_active = false;

// custom theme setup:
M5Config::ExplorerTheme theme;
theme.background_color = 0x211a;
theme.border_color = 0x2c9f;
theme.selection_color = 0x06e0;
theme.value_color = WHITE;
theme.item_height = 23;
theme.item_window = 5;
theme.font = &fonts::FreeSans12pt7b;



config.begin(&canvas,OnUsage);
config.setTheme(&theme);
config.goToMenu(&menu);
config.open(); // if an argument is not providen, it will use defualts or latest config.
}

void loop(){
  M5Cardputer.update();
    if (M5Cardputer.Keyboard.isChange()) {
    auto keys = M5Cardputer.Keyboard.keysState();
    if (M5Cardputer.Keyboard.isKeyPressed(';')){config.process_input(config.UP);}
    if (M5Cardputer.Keyboard.isKeyPressed('.')){config.process_input(config.DOWN);}
    if (M5Cardputer.Keyboard.isKeyPressed(',')){config.process_input(config.LEFT);}
    if (M5Cardputer.Keyboard.isKeyPressed('/')){config.process_input(config.RIGHT);}
    if (keys.enter){config.process_input(config.SELECT);}
    if (keys.del){config.process_input(config.BACK);}
    
    if (M5Cardputer.Keyboard.isKeyPressed('=')){config.open();}
    if (M5Cardputer.Keyboard.isKeyPressed('-')){
        config.close();
        canvas.setTextColor(YELLOW);
        canvas.drawString("Closed",0,0,&fonts::FreeMonoBold24pt7b);
        canvas.pushSprite(0,0);
    }

    }
}