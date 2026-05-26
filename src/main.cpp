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

uint8_t item12 = 0;
uint8_t item13 = 0;


void item10(){
    canvas.clear();
    canvas.drawString("Hi, this is a function,", 0,0,&fonts::Font4);
    canvas.drawString("Menu isnt auto closed", 0,32,&fonts::Font4);
    canvas.drawString("You must call close().", 0,64,&fonts::Font4);
    canvas.pushSprite(0,0);
}


M5Config::ConfigItem configs2[] = {
    {
        "0-10", // name
        &item8, // pointer to variable
        1, // increment
        0, // minimum
        10 // maximum
    },
    {
        "0-100, +10, clamp", 
        &item9,
        10,
        0,
        100,
        M5Config::ScrollType::TYPE_CLAMP
    },
        {
        "0-100, +10, wrap", 
        &item12,
        10,
        0,
        100,
        M5Config::ScrollType::TYPE_WRAP
    },
        {
        "0-100, +10, cyclic", 
        &item13,
        10,
        0,
        100,
        M5Config::ScrollType::TYPE_CYCLIC
    },
    {
        "function",
        item10 // function pointer
    },
    {
        "declaration 2",
        &item11, 
        item11_increment,
        item11_min,
        item11_max
    }
};

M5Config::ConfigMenu menu2 = {
    .config_items = configs2, 
    .size = sizeof(configs2) / sizeof(configs2[0])
};

M5Config::ConfigItem configs[] = {
    {
        "uint8_t",
        &item1,
        1,
        0,
        UINT8_MAX
    },
    {
        "uint16_t",
        &item2,
        1,
        0,
        UINT16_MAX
    },
    {
        "uint32_t",
        &item3,
        1,
        0,
        UINT32_MAX
    },
    {
        "int8_t",
        &item4,
        1,
        INT8_MIN,
        INT8_MAX
    },
    {
        "int16_t",
        &item5,
        1,
        INT16_MIN,
        INT16_MAX
    },
    {
        "int32_t",
        &item6,
        1,
        INT32_MIN,
        INT32_MAX
    },
    {
        "bool",
        &item7
    },
    {
        "sub menu",
        &menu2
    }
};

M5Config::ConfigMenu menu = {
    .config_items = configs, 
    .size = sizeof(configs) / sizeof(configs[0])
};

void OnUsage(M5Config::ConfigItem* item){/* commented for smoother browsing, not usually needed but it exists.
    canvas.clear();
    canvas.drawString("Interacted with item,", 0,0,&fonts::Font4);
    canvas.drawString("name = " + String(item->name), 0,32,&fonts::Font4);
    canvas.drawString("Type = " + String(static_cast<int>(item->type)), 0,64,&fonts::Font4);
    canvas.pushSprite(0,0);
    return;
    // you cant modify here because its pointless, the prupose of the lib is to modify the value for you.
    // but if for some reason you wanted to access the value on the callback, then do something similar as _formatValue() inside M5Config.cpp for the casting.
    */ 
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
    if (M5Cardputer.Keyboard.isKeyPressed(';')){config.process_input(M5Config::Input::UP);}
    if (M5Cardputer.Keyboard.isKeyPressed('.')){config.process_input(M5Config::Input::DOWN);}
    if (M5Cardputer.Keyboard.isKeyPressed(',')){config.process_input(M5Config::Input::LEFT);}
    if (M5Cardputer.Keyboard.isKeyPressed('/')){config.process_input(M5Config::Input::RIGHT);}
    if (keys.enter){config.process_input(M5Config::Input::SELECT);}
    if (keys.del){config.process_input(M5Config::Input::BACK);}
    
    if (M5Cardputer.Keyboard.isKeyPressed('=')){config.open();}
    if (M5Cardputer.Keyboard.isKeyPressed('-')){
        config.close();
        canvas.setTextColor(YELLOW);
        canvas.drawString("Closed",0,0,&fonts::FreeMonoBold24pt7b);
        canvas.pushSprite(0,0);
    }

    }
}