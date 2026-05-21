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
#define VPTR(x) ((void*)(uintptr_t)(x))
M5Config::ConfigItem configs[] = {
    {"uint8_t",  config.TYPE_UINT8_T,  &item1, VPTR(1), VPTR(0), VPTR(255)},
    {"uint16_t", config.TYPE_UINT16_T, &item2, VPTR(1), VPTR(0), VPTR(65535)},
    {"uint32_t", config.TYPE_UINT32_T, &item3, VPTR(1), VPTR(0), VPTR(4294967295)},

    {"int8_t",   config.TYPE_INT8_T,   &item4, VPTR(1), VPTR(-128), VPTR(127)},
    {"int16_t",  config.TYPE_INT16_T,  &item5, VPTR(1), VPTR(-32768), VPTR(32767)},
    {"int32_t",  config.TYPE_INT32_T,  &item6, VPTR(1), VPTR(-2147483648), VPTR(2147483647)},

    {"bool",config.TYPE_BOOL,&item7,VPTR(1),VPTR(0),VPTR(1)}
};

M5Config::ConfigMenu menu = {configs, sizeof(configs) / sizeof(configs[0])};

void OnSelection(M5Config::ConfigItem*){
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
theme.item_height = 23;
theme.item_window = 4;
theme.font = &fonts::FreeSans12pt7b;



config.begin(&canvas,OnSelection);
//config.setTheme(&theme);
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