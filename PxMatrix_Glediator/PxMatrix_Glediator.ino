//--------------------------------------PxMatrix-Glediator--------------------------------------
//
//Controll LED-Matrices using the glediator-protocol
//You can use software like Jinx! (http://www.live-leds.de/downloads/) to controll your matrix
//Follow instructions on https://github.com/2dom/PxMatrix to wire up your display
//
//-----------------------------------------by Loewe_111-----------------------------------------

#define PxMATRIX_COLOR_DEPTH 4  //The color-depth to output (RECOMMENDED: 4)

#define MATRIX_HEIGHT 32        //The height of your display in pixels
#define MATRIX_WIDTH 64         //The width of your display in pixels
#define BAUDRATE 500000         //The baudrate data is send (RECOMMENDED: 500000)

#include <PxMatrix.h>

#ifdef ESP32

#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 16
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

#endif

#ifdef ESP8266

#include <Ticker.h>
Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

#endif

PxMATRIX display(MATRIX_WIDTH,MATRIX_HEIGHT,P_LAT, P_OE,P_A,P_B,P_C,P_D);

uint16_t textColor = display.color565(50, 30, 0);
int16_t BLACK = display.color565(0, 0, 0);

#ifdef ESP8266
// ISR for display refresh
void display_updater()
{
  display.display(20);
}
#endif

#ifdef ESP32
void IRAM_ATTR display_updater(){
  portENTER_CRITICAL_ISR(&timerMux);
  display.display(20);
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif


void setup() {
  Serial.begin(BAUDRATE);
  display.begin(16);
  display.flushDisplay();
  display.setTextWrap(true);
  display.fillScreen(BLACK);
  display.setTextColor(textColor);
  display.setCursor(1, 1);
  display.print("Welcome");
  display.showBuffer();
  delay(100);
  #ifdef ESP8266
    display_ticker.attach(0.004, display_updater);
  #endif

  #ifdef ESP32
    timer = timerBegin(0, 40, true);
    timerAttachInterrupt(timer, &display_updater, true);
    timerAlarmWrite(timer, 4000, true);
    timerAlarmEnable(timer);
  #endif
  
}

int serialGlediator () {
  while (!Serial.available()) {}
  return Serial.read();
}

byte leds[(MATRIX_HEIGHT*MATRIX_WIDTH)*3+1];
int prog;

void loop() {
  while (serialGlediator () != 1) {}
  Serial.readBytes((char*)leds, (MATRIX_HEIGHT*MATRIX_WIDTH)*3);
  prog = 0;
  for(int y = 0;y<MATRIX_HEIGHT; y++)
  {
    for(int x = 0;x<MATRIX_WIDTH;x++)
    {
      display.drawPixel(x,y,display.color565(int(leds[prog]), leds[1+prog], leds[2+prog]));
      prog+=3;
    }
  }
}
