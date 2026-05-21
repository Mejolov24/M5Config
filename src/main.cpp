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


void item10(){
    canvas.clear();
    canvas.drawString("Hi, this is a function,", 0,0,&fonts::Font4);
    canvas.drawString("Menu isnt auto closed", 0,32,&fonts::Font4);
    canvas.drawString("You must call close().", 0,64,&fonts::Font4);
    canvas.pushSprite(0,0);
}


M5Config::ConfigItem configs2[] = {
    {"0-10",  config.TYPE_UINT8_T,  &item8, 1,0,1},
    {"0-100, +10", config.TYPE_UINT8_T, &item9, 10,0,100},
    {"function",   config.TYPE_FUNCTION, item10},
    {"declaration 2",  config.TYPE_UINT8_T,  &item11, item11_increment, item11_min, item11_max}
};
M5Config::ConfigMenu menu2 = {configs2, sizeof(configs2) / sizeof(configs2[0])};

M5Config::ConfigItem configs[] = {
    {"uint8_t",  config.TYPE_UINT8_T,  &item1, 1, 0, 255},
    {"uint16_t", config.TYPE_UINT16_T, &item2, 1, 0, 65535},
    {"uint32_t", config.TYPE_UINT32_T, &item3, 1, 0, 4294967295},

    {"int8_t",   config.TYPE_INT8_T,   &item4, 1, -128, 127},
    {"int16_t",  config.TYPE_INT16_T,  &item5, 1, -32768, 32767},
    {"int32_t",  config.TYPE_INT32_T,  &item6, 1, -2147483648, 2147483647},
    {"float",  config.TYPE_FLOAT,  &item6, 0.1f, -0.1f, 1.5f},

    {"bool",config.TYPE_BOOL,&item7,1,0,1},
    {"sub menu",config.TYPE_SUBMENU,&menu2}
};

M5Config::ConfigMenu menu = {configs, sizeof(configs) / sizeof(configs[0])};


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