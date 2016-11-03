#include "SparkFunMicroOLED.h"  // Include Micro OLED library
#include "math.h"
#include "ClickButton.h"
#include "images.h"

#define PIN_RESET D7  // Connect RST to pin 7 (req. for SPI and I2C)
#define PIN_DC    D6  // Connect DC to pin 6 (required for SPI)
#define PIN_CS    A2 // Connect CS to pin A2 (required for SPI)
#define BUTTON_PIN D0
#define RED_LED D1
#define GREEN_LED D2
#define BRUSHIN_TIME 5

//https://docs.particle.io/support/troubleshooting/mode-switching/photon/#semi-automatic-mode
SYSTEM_MODE(SEMI_AUTOMATIC);

MicroOLED oled(MODE_SPI, PIN_RESET, PIN_DC, PIN_CS);

//ClickButton button1(BUTTON_PIN, LOW, CLICKBTN_PULLUP);

int SCREEN_WIDTH = oled.getLCDWidth();
int SCREEN_HEIGHT = oled.getLCDHeight();

bool brushing = false;

void setup()
{
   attachInterrupt(BUTTON_PIN, click, RISING);

  oled.begin();
  oled.clear(ALL);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

//  button1.debounceTime   = 20;   // Debounce timer in ms
//  button1.multiclickTime = 250;  // Time limit for multi clicks
//  button1.longClickTime  = 1000; // time until "held-down clicks" register

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
}


void click() {
  if (Particle.connected() == false) {
    Particle.connect();
  }
  if (!brushing )
  {
      startBrushing(BRUSHIN_TIME);
  }

}



void loop()
{

}


void startBrushing( int time){
  Particle.publish("brushing","start brushing");
  brushing = true;
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  while (time > 0)
  {
    displayTime(time);
      delay(1000);
      time--;
  }
  showSmiley();

  brushing = false;
  digitalWrite(RED_LED, LOW);
 digitalWrite(GREEN_LED, HIGH);
 delay(2000);
 digitalWrite(GREEN_LED, LOW);
}


void displayTime(int time){
  oled.setFontType(3);
  oled.clear(PAGE);
  oled.setCursor(0, 0);
  //work out how much space so we can centre
  int  digits = floor( log10( time ) ) + 1;

  Particle.publish("time", String(time));

  Particle.publish("digits", String(digits));

  oled.print(" ");
  oled.print (time);
  oled.print("secs");
  oled.display();
}



void showSmiley () {
  oled.clear(PAGE);

  oled.drawBitmap(tiny_teeth);

  oled.display();
  delay(2000);
  oled.clear(PAGE);
}


void printTitle(String title, int font)
{
  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight() / 2;

  oled.clear(PAGE);
  oled.setFontType(font);
  // Try to set the cursor in the middle of the screen
  oled.setCursor(middleX - (oled.getFontWidth() * (title.length()/2)),
                 middleY - (oled.getFontWidth() / 2));
  // Print the title:
  oled.print(title);
  oled.display();
  delay(1500);
  oled.clear(PAGE);
}
