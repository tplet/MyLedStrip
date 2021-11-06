# MyLedStrip
LedStrip library with MySensor

# Dependencies

Please make sure that theses libraries has been installed.

* MySensors: https://github.com/mysensors/MySensors
* Adafruit_NeoPixel: https://github.com/adafruit/Adafruit_NeoPixel
* ArduinoProperty: https://github.com/tplet/ArduinoProperty

# Using

```cpp
#include <MySensors.h>  
#include <MyLedStrip.h>

// Led strip
// Pin to control led
#define LED_PIN 6
// Total number of led
#define LED_COUNT 150

//
// Node
//
// Node childs id (local to node)
#define CHILD_ID_MODE 0
#define CHILD_ID_ENABLE 1
#define CHILD_ID_SIZE 2
#define CHILD_ID_LOOP 3
#define CHILD_ID_COLOR 4
#define CHILD_ID_RAINBOW 5
#define CHILD_ID_SPEED 6
#define CHILD_ID_LUMINOSITY 7
#define CHILD_ID_DIRECTION 8
#define CHILD_ID_POSITION 9


MyLedStrip ledStrip(
    CHILD_ID_MODE,
    CHILD_ID_ENABLE,
    CHILD_ID_SIZE,
    CHILD_ID_LOOP,
    CHILD_ID_COLOR,
    CHILD_ID_RAINBOW,
    CHILD_ID_SPEED,
    CHILD_ID_LUMINOSITY,
    CHILD_ID_DIRECTION,
    CHILD_ID_POSITION,
    LED_COUNT,
    LED_PIN
);

/**
 * Init node to gateway
 */
void presentation()  
{ 
    // Send the sketch version information to the gateway
    sendSketchInfo("LedStrip", "1.0.0");
  
    // LedStrip
    ledStrip.presentation();
}

/**
 * Setup node
 */
void setup()
{
    // LedStrip
    ledStrip.setup();
}

/**
 * Loop
 */
void loop()      
{
    // LedStrip
    ledStrip.loop();
    
    // Wait for a while, and keep listening to receive some message
    wait(ledStrip.getSpeedToMs());
}

/**
 * Receive message
 */
void receive(const MyMessage &message)
{    
    // LedStrip
    ledStrip.receive(message);  
}
```