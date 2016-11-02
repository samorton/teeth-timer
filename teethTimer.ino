#include "SparkFunMicroOLED.h"  // Include Micro OLED library
#include "math.h"
#include "ClickButton.h"


#define PIN_RESET D7  // Connect RST to pin 7 (req. for SPI and I2C)
#define PIN_DC    D6  // Connect DC to pin 6 (required for SPI)
#define PIN_CS    A2 // Connect CS to pin A2 (required for SPI)
#define BUTTON_PIN D0
#define RED_LED D1
#define GREEN_LED D2

//////////////////////////////////
// MicroOLED Object Declaration //
//////////////////////////////////
// Declare a MicroOLED object. The parameters include:
// 1 - Mode: Should be either MODE_SPI or MODE_I2C
// 2 - Reset pin: Any digital pin
// 3 - D/C pin: Any digital pin (SPI mode only)
// 4 - CS pin: Any digital pin (SPI mode only, 10 recommended)
MicroOLED oled(MODE_SPI, PIN_RESET, PIN_DC, PIN_CS);
//MicroOLED uOLED(MODE_I2C, PIN_RESET); // Example I2C declaration

// I2C is great, but will result in a much slower update rate. The
// slower framerate may be a worthwhile tradeoff, if you need more
// pins, though.
ClickButton button1(BUTTON_PIN, LOW, CLICKBTN_PULLUP);

////////////////////
// Cube Variables //
////////////////////
#define SHAPE_SIZE 600
#define ROTATION_SPEED 0 // ms delay between cube draws

int SCREEN_WIDTH = oled.getLCDWidth();
int SCREEN_HEIGHT = oled.getLCDHeight();

float d = 3;
float px[] = { -d,  d,  d, -d, -d,  d,  d, -d };
float py[] = { -d, -d,  d,  d, -d, -d,  d,  d };
float pz[] = { -d, -d, -d, -d,  d,  d,  d,  d };

float p2x[] = {0,0,0,0,0,0,0,0};
float p2y[] = {0,0,0,0,0,0,0,0};

float r[] = {0,0,0};

bool brushing = false;

void setup()
{
  oled.begin();
  oled.clear(ALL);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  button1.debounceTime   = 20;   // Debounce timer in ms
  button1.multiclickTime = 250;  // Time limit for multi clicks
  button1.longClickTime  = 1000; // time until "held-down clicks" register

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

}
void loop()
{

  button1.Update();

  if(button1.clicks != 0)
  {
    //decodeFunction(button1.clicks);
    if (!brushing )
    {
        startBrushing(120);
    }
  }
  //drawCube();
	//delay(ROTATION_SPEED);
  delay(1);
}


void startBrushing( int time){
  Particle.publish("brushing","start brushing");
  brushing = true;
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  while (time--> 0)
  {
    displayTime(time);
      delay(1000);
  }

  //displayStopwatch();

  brushing = false;
  digitalWrite(RED_LED, LOW);
 digitalWrite(GREEN_LED, HIGH);
 delay(2000);
 digitalWrite(GREEN_LED, LOW);
}
void decodeFunction(int function) {
  if(function == 1)
  {
    Particle.publish("button-press","SINGLE click");
    Serial.println("SINGLE click");
  }

  if(function == 2){
    Particle.publish("button-press","DOUBLE click");
    Serial.println("DOUBLE click");
  }

if(function == 3)
{
  Particle.publish("button-press","TRIPLE click");
  Serial.println("TRIPLE click");
}

  if(function == -1) {
    Particle.publish("button-press","SINGLE LONG click");
    Serial.println("SINGLE LONG click");
  }

  if(function == -2) {
    Particle.publish("button-press","DOUBLE LONG click");
    Serial.println("DOUBLE LONG click");
  }

  if(function == -3)
  {
    Particle.publish("button-press","TRIPLE LONG click");
    Serial.println("TRIPLE LONG click");
  }

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

void displayStopwatch() {
  // Demonstrate font 3. 12x48. Stopwatch demo.
  oled.setFontType(3);  // Use the biggest font
  int ms = 0;
  int s = 0;
  while (s <= 50)
  {
    oled.clear(PAGE);     // Clear the display
    oled.setCursor(0, 0); // Set cursor to top-left
    if (s < 10)
      oled.print("00");   // Print "00" if s is 1 digit
    else if (s < 100)
      oled.print("0");    // Print "0" if s is 2 digits
    oled.print(s);        // Print s's value
    oled.print(":");      // Print ":"
    oled.print(ms);       // Print ms value
    oled.display();       // Draw on the screen
    ms++;         // Increment ms
    if (ms >= 10) // If ms is >= 10
    {
      ms = 0;     // Set ms back to 0
      s++;        // and increment s
    }
    delay(1);
  }
}




void drawCube()
{
  r[0]=r[0]+M_PI/180.0; // Add a degree
	r[1]=r[1]+M_PI/180.0; // Add a degree
	r[2]=r[2]+M_PI/180.0; // Add a degree
	if (r[0] >= 360.0*M_PI/180.0) r[0] = 0;
	if (r[1] >= 360.0*M_PI/180.0) r[1] = 0;
	if (r[2] >= 360.0*M_PI/180.0) r[2] = 0;

	for (int i=0;i<8;i++)
	{
		float px2 = px[i];
		float py2 = cos(r[0])*py[i] - sin(r[0])*pz[i];
		float pz2 = sin(r[0])*py[i] + cos(r[0])*pz[i];

		float px3 = cos(r[1])*px2 + sin(r[1])*pz2;
		float py3 = py2;
		float pz3 = -sin(r[1])*px2 + cos(r[1])*pz2;

		float ax = cos(r[2])*px3 - sin(r[2])*py3;
		float ay = sin(r[2])*px3 + cos(r[2])*py3;
		float az = pz3-150;

		p2x[i] = SCREEN_WIDTH/2+ax*SHAPE_SIZE/az;
		p2y[i] = SCREEN_HEIGHT/2+ay*SHAPE_SIZE/az;
	}

	oled.clear(PAGE);
	for (int i=0;i<3;i++)
	{
		oled.line(p2x[i],p2y[i],p2x[i+1],p2y[i+1]);
		oled.line(p2x[i+4],p2y[i+4],p2x[i+5],p2y[i+5]);
		oled.line(p2x[i],p2y[i],p2x[i+4],p2y[i+4]);
	}
	oled.line(p2x[3],p2y[3],p2x[0],p2y[0]);
	oled.line(p2x[7],p2y[7],p2x[4],p2y[4]);
	oled.line(p2x[3],p2y[3],p2x[7],p2y[7]);
	oled.display();
}
