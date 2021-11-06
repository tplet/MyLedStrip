//
// Created by Thibault PLET on 19/05/2020.
//

#ifndef COM_OSTERES_AUTOMATION_ARDUINO_COMPONENT_MYSENSOR_MYLEDSTRIP_H
#define COM_OSTERES_AUTOMATION_ARDUINO_COMPONENT_MYSENSOR_MYLEDSTRIP_H

#include <MySensors.h>
#include <ArduinoProperty.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Serpent: Une trainée de points lumineux se déplace sur le ruban
#define MODE_SNAKE 0
// Sapin: Toutes les lumières sont allumées
#define MODE_FIR 1

class MyLedStrip {
public:
    /**
     * Constructor
     * 
     * @param childMode Child mode identifier
     * @param childEnable Child enable identifier
     * @param childSize Child size identifier
     * @param childLoop Child loop identifier
     * @param childRainbow Child rainbow identifier
     * @param childSpeed Child speed identifier
     * @param childLuminosity Child luminosity identifier
     * @param childDirectio Child direction property
     * @param childPosition Child position property
     * @param ledCount  How many led are attached to the arduino
     * @param ledPin    Which pin on arduino is connected to control led
     */
    MyLedStrip(
      uint8_t childMode,
      uint8_t childEnable,
      uint8_t childSize,
      uint8_t childLoop,
      uint8_t childColor,
      uint8_t childRainbow,
      uint8_t childSpeed,
      uint8_t childLuminosity,
      uint8_t childDirection,
      uint8_t childPosition,
      uint16_t ledCount, 
      uint16_t ledPin
    ) {
        // Childs identifier
        this->childMode = childMode;
        this->childEnable = childEnable;
        this->childSize = childSize;
        this->childLoop = childLoop,
        this->childColor = childColor;
        this->childRainbow = childRainbow;
        this->childSpeed = childSpeed;
        this->childLuminosity = childLuminosity;
        this->childDirection = childDirection;
        this->childPosition = childPosition;
      
        this->trigger = new DataBuffer(this->convertSpeedToDelay(this->speed));
        this->strip = new Adafruit_NeoPixel(ledCount, ledPin, NEO_GRB + NEO_KHZ800);
        // Argument 1 = Number of pixels in NeoPixel strip
        // Argument 2 = Arduino pin number (most are valid)
        // Argument 3 = Pixel type flags, add together as needed:
        //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
        //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
        //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
        //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
        //   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
    }

    /**
     * Destructor
     */
    ~MyLedStrip()
    {
        delete this->trigger;
        delete this->strip;
    }

    /**
     * Presentation (for MySensor)
     */
    void presentation()
    {
        present(this->childMode, S_CUSTOM, "Mode");
        present(this->childEnable, S_BINARY, "Enable");
        present(this->childSize, S_CUSTOM, "Size");
        present(this->childLoop, S_BINARY, "Loop");
        present(this->childColor, S_CUSTOM, "Color");
        present(this->childRainbow, S_BINARY, "Rainbow");
        present(this->childSpeed, S_CUSTOM, "Speed");
        present(this->childLuminosity, S_DIMMER, "Luminosity");
        present(this->childDirection, S_CUSTOM, "Direction");
        present(this->childPosition, S_CUSTOM, "Position");
    }

    /**
     * Setup MySensor
     */
    void setup()
    {
        // Request all child
        request(this->childMode, V_CUSTOM);
        request(this->childEnable, V_STATUS);
        request(this->childSize, V_CUSTOM);
        request(this->childLoop, V_STATUS);
        request(this->childColor, V_CUSTOM);
        request(this->childRainbow, V_STATUS);
        request(this->childSpeed, V_CUSTOM);
        request(this->childLuminosity, V_PERCENTAGE);
        request(this->childDirection, V_CUSTOM);
        request(this->childPosition, V_CUSTOM);
      
        // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
        // Any other board, you can remove this part (but no harm leaving it):
        #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
        clock_prescale_set(clock_div_1);
        #endif
        // END of Trinket-specific code.
      
        strip->begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
        strip->show();            // Turn OFF all pixels ASAP
        this->setLuminosity(this->getLuminosity()); // Force re-apply luminosity        
    }

    /**
     * Receive (for MySensor)
     */
    void receive(const MyMessage &message)
    {
        if (!message.isEcho()) {
            #ifdef MY_DEBUG
            String logMsg = "R: ";
            #endif
            // Mode
            if (message.sensor == this->childMode) {
                this->setMode(message.getByte());
                #ifdef MY_DEBUG
                logMsg.concat("mode ");
                logMsg.concat(this->getMode());
                #endif
            }
            // Enable
            else if (message.sensor == this->childEnable) {
                this->setEnable(message.getBool());
                #ifdef MY_DEBUG
                logMsg.concat("enable " + String(message.getBool() ? "1" : "0") + " - " + String(this->isEnable() ? "1" : "0"));
                #endif
            }
            // Size
            else if (message.sensor == this->childSize) {
                this->setSize(message.getUInt());
                #ifdef MY_DEBUG
                logMsg.concat("size " + String(message.getUInt()) + " - " + String(this->getSize()));
                #endif
            }
            // Loop
            else if (message.sensor == this->childLoop) {
                this->setLoop(message.getBool());
                #ifdef MY_DEBUG
                logMsg.concat("loop " + String(message.getBool() ? "1" : "0") + " - " + String(this->isLoop() ? "1" : "0"));
                #endif
            }
            // Color
            else if (message.sensor == this->childColor) {
                this->setColor(message.getULong());
                #ifdef MY_DEBUG
                logMsg.concat("color " + String(message.getULong()) + " - " + String(this->getColor()));
                #endif
            }
            // Rainbow
            else if (message.sensor == this->childRainbow) {
                this->setRainbow(message.getBool());
                #ifdef MY_DEBUG
                logMsg.concat("rainbow " + String(message.getBool() ? "1" : "0") + " - " + String(this->isRainbow() ? "1" : "0"));
                #endif
            }
            // Speed
            else if (message.sensor == this->childSpeed) {
                this->setSpeed(message.getFloat());
                #ifdef MY_DEBUG
                logMsg.concat("speed " + String(message.getFloat()) + " - " + String(this->getSpeed()));
                #endif
            }
            // Luminosity
            else if (message.sensor == this->childLuminosity) {
                this->setLuminosity(message.getByte());
                #ifdef MY_DEBUG
                logMsg.concat("luminosity " + String(message.getByte()) + " - " + String(this->getLuminosity()));
                #endif
            }
            // Direction
            else if (message.sensor == this->childDirection) {
                this->setDirection((int8_t)message.getInt());
                #ifdef MY_DEBUG
                logMsg.concat("direction " + String(message.getInt()) + " - " + String(this->getDirection()));
                #endif
            }
            // Position
            else if (message.sensor == this->childPosition) {
                this->setPosition(message.getInt());
                #ifdef MY_DEBUG
                logMsg.concat("position " + String(message.getInt()) + " - " + String(this->getPosition()));
                #endif
            }
            #ifdef MY_DEBUG
            this->sendLog(logMsg.c_str());
            #endif
        }
    }

    /**
     * Loop MySensor
     */
    void loop()
    {
        if (this->isEnable() && this->trigger->isOutdated()) {

            this->trigger->reset();
            
            if (this->mode == MODE_SNAKE) {
                this->snakeProcess();
            }
            else if (this->mode == MODE_FIR) {
                this->firProcess();
            }
        }
    }

    /**
     * Set luminosity
     */
    void setLuminosity(uint8_t value)
    {
        this->luminosity = max(0, min(100, value));

        strip->setBrightness(this->convertLuminosityToBrightness(this->luminosity)); // Set BRIGHTNESS to about 1/5 (max = 255)
    }

    /**
     * Get luminosity
     */
    uint8_t getLuminosity()
    {
        return this->luminosity;
    }

    /**
     * Set mode
     */
    void setMode(uint8_t mode)
    {
        if (this->isAllowedMode(mode)) {
            this->mode = mode;
        }
    }

    /**
     * Get mode
     */
    uint8_t getMode()
    {
        return this->mode;
    }

    /**
     * Is led strip enable
     */
    bool isEnable()
    {
        return this->enable;
    }

    /**
     * Enable or disable led strip
     */
    void setEnable(bool enable)
    {
        this->enable = enable;

        if (!this->enable) {
            this->off();
        }
    }

    /**
     * Set speed
     */
    void setSpeed(float speed)
    {
        // Cannot be negative or zero
        if (speed > 0) {
            this->speed = speed;
    
            // Convert speed to trigger intervale
            this->trigger->setBufferDelay(this->convertSpeedToDelay(this->speed));
        }
    }

    /**
     * Get speed
     */
    float getSpeed()
    {
        return this->speed;
    }

    /**
     * Get speed converted to milliseconds
     */
    unsigned long getSpeedToMs()
    {
        return this->convertSpeedToDelay(this->speed);
    }

    /**
     * Is loop enable
     */
    bool isLoop()
    {
        return this->loopMode;
    }

    /**
     * Enable/Disable loop
     */
    void setLoop(bool loop)
    {
        this->loopMode = loop;
    }

    /**
     * Set direction (only -1, 0 and 1 values allowed)
     */
    void setDirection(int8_t direction)
    {
        this->direction = max(-1, min(1, direction));
    }

    /**
     * Get direction
     */
    int8_t getDirection()
    {
        return this->direction;
    }

    /**
     * Set position
     */
    void setPosition(int16_t position)
    {
        this->position = this->convertPosition(position);
    }

    /**
     * Get position
     */
    uint16_t getPosition()
    {
        return this->position;
    }

    /**
     * Set size
     */
    void setSize(uint16_t size)
    {
        this->size = size;
    }

    /**
     * Get size
     */
    uint16_t getSize()
    {
        return this->size;
    }

    /**
     * Set rainbow
     */
    void setRainbow(bool rainbow)
    {
        this->rainbow = rainbow;
    }

    /**
     * Is rainbow enable
     */
    bool isRainbow()
    {
        return this->rainbow;
    }

    /**
     * Set color
     */
    void setColor(uint32_t color)
    {
        this->color = color;
    }

    /**
     * Get color
     */
    uint32_t getColor()
    {
        return this->color;
    }

protected:

    /**
     * Snake process
     */
    void snakeProcess()
    {
        // Reset all led
        this->off(false);

        float fakeDirection = this->direction > 0 ? (float)this->direction : -1.0; // Use -1 for 0 and -1 values for some case

        // From back position (computed with 'size') to position 
        int16_t pos;
        float lumDelta = (float)this->getLuminosity() / (float)(this->size+1); // Use size+1 to allow to see last point (otherwise, will be luminosity 0)
        float lum = this->direction > 0 ? lumDelta : (float)this->getLuminosity();
        int16_t posStart = (int16_t)this->position - (int16_t)(this->direction > 0 ? this->size : 0);
        for (
            int16_t i = posStart; 
            i < posStart + this->size;
            i++
        ) {
            pos = this->convertPosition(i);

            if (pos >= 0) {
                this->strip->setPixelColor(pos, this->getNextColor((uint8_t)lum));
            }
            lum += lumDelta * fakeDirection;
        }
        this->strip->show();

        // Move position used for next time
        this->move();
    }

    /**
     * Fir process
     */
    void firProcess()
    {
        // TODO
    }

    /**
     * Convert relative position to absolute in led strip, considering loop parameter
     * If loop disable and position if outside led strip, -1 retuned
     */
    int16_t convertPosition(int16_t pos)
    {
        int16_t maxPos = this->strip->numPixels();
        if (!this->isLoop() && (pos < 0 || pos >= maxPos)) {
            return -1;
        }

        pos = pos % maxPos;
        if (pos < 0) {
            pos = maxPos + pos;
        }

        return pos;
    }

    /**
     * Compte next color
     */
    uint32_t getNextColor(uint8_t luminosity)
    {
        // Compute next color
        if (this->rainbow) {
            // TODO
        }
        // Normal mode: use defined color
        else {
            uint32_t c = this->color;
            uint8_t r = (uint8_t)(c >> 16), g = (uint8_t)(c >> 8), b = (uint8_t)c;
            uint8_t br = this->convertLuminosityToBrightness(luminosity);

            // Apply luminosity
            r = (r * br) >> 8;
            g = (g * br) >> 8;
            b = (b * br) >> 8;
          
            return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
        }
    }

    /**
     * Check if mode is declared mode
     */
    bool isAllowedMode(uint8_t mode)
    {
        return mode == MODE_SNAKE || mode == MODE_FIR;
    }

    /**
     * Off all led
     */
    void off(bool show = true)
    {
        for (uint16_t i = 0; i < this->strip->numPixels(); i++) {
            this->strip->setPixelColor(i, 0);
        }

        if (show) {
            this->strip->show();
        }
    }

    /**
     * Move position, considering direction and loop
     */
    void move()
    {
        if (!this->isLoop()) {
            if (this->position == this->strip->numPixels() - 1 || this->position == 0) {
                this->direction = -this->direction;
            }
        }

        int16_t addSize = this->direction > 0 ? (int16_t)this->size : 0;
        this->position = this->convertPosition(this->position + this->direction - addSize) + addSize;
    }

    /**
     * Convert speed value to delay
     */
    unsigned long convertSpeedToDelay(float speed)
    {
        return (unsigned long)((float)(1/this->speed) * 1000UL);
    }

    /**
     * Convert luminosity percent to brightness value
     */
    uint8_t convertLuminosityToBrightness(uint8_t percent)
    {
        return (uint8_t)(percent/100.0 * 255);
    }

    /**
     * Send log
     *
     * @param char * message Log message (max 25 bytes). To confirm: 10 char max
     */
    void sendLog(const char * message)
    {
      MyMessage msg;
      msg.sender = getNodeId();
      msg.destination = GATEWAY_ADDRESS;
      msg.sensor = NODE_SENSOR_ID;
      msg.type = I_LOG_MESSAGE;
      mSetCommand(msg, C_INTERNAL);
      mSetRequestEcho(msg, true);
      mSetEcho(msg, false);

      msg.set(message);

      _sendRoute(msg);
    }

    /**
     * Strip to control
     */
    Adafruit_NeoPixel * strip;

    /**
     * Trigger to display next led configuration
     */
    DataBuffer * trigger;

    /**
     * Set led strip mode
     * Use static MODE_* constants to set value
     */
    uint8_t mode = 0; // Snake

    /**
     * snake mode: Size of snake (with gradient)
     * fir mode: Number of hole between two light
     */
    uint16_t size = 4;

    /**
     * Snake mode only: 
     *    If true, snake continue to beginning of strip after passing the end (like a loop). 
     *    If false, snake go back in reverse direction
     */
    bool loopMode = true;

    /**
     * Snake mode only: Position where start snake (will be changed during animation)
     */
    uint16_t position = 0;

    /**
     * Color to use
     */
    uint32_t color = 16711680; // Red

    /**
     * Rainbow mode. If enable, color parameter is ignored
     */
    bool rainbow = false;

    /**
     * Speed of moving point = number of time per second to refresh
     * Example: 
     *    1 = 1 refresh every second
     *    2 = 1 refresh every half-second
     *    0.5 = 1 refresh every 2 seconds
     */
    float speed = 1;

    /**
     * Direction of moving. 
     * Use 0 to stop moving.
     * Available values are -1, 0 or 1
     */
    int8_t direction = 1;
    
    /**
     * Light luminosity in percent (%)
     */
    uint8_t luminosity = 50;

    /**
     * Enable or disable led strip
     */
    bool enable = true;

    /**
     * Child identifier
     */
    uint8_t childMode; // @see mode property
    uint8_t childEnable; // @see enable property
    uint8_t childSize; // @see size property
    uint8_t childLoop; // @see loop property
    uint8_t childColor; // @see color property
    uint8_t childRainbow; // @see rainbow property
    uint8_t childSpeed; // @see speed property
    uint8_t childLuminosity; // @see luminosity property
    uint8_t childDirection; // @see direction property
    uint8_t childPosition; // @see position property

};

#endif // COM_OSTERES_AUTOMATION_ARDUINO_COMPONENT_MYSENSOR_MYLEDSTRIP_H
