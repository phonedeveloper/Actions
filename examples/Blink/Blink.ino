/*
 * Blink - Uses the Actions library to blink an LED without using delay()
 *   
 * "Actions" lets you do some action at later time.
 *
 * The Actions method "runLater(function, time)" lets you run
 * "function" at some at some "time" in the future.
 * 
 * The Actions method "loop()" checks to see whether or not
 * it's time to run a function postponed using runLater, and if
 * so, runs it, then exits. If it's not time to run a function,
 * it simply exits. Therefore, it must be called repeatedly by
 * the sketch (here we do it from the sketch's "loop()" function).
 *
 * Connections:
 *   * Uses the LED on the Uno and Mega boards that's on pin 13.
 *
 * Author: PhoneDeveloper, LLC
 * Date: 09/15/2015
 *
 * Copyright (c) 2015, PhoneDeveloper, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <Actions.h>                      // Use the Actions library...
Actions led;                              // For running LED on/off functions...

const static int LED_PIN=13;              // Pin the LED is attached to
const static int BLINK_DELAY=500;         // How long the LED is on or off

void setup() {
  pinMode(LED_PIN, OUTPUT);               // Set the LED pin to OUTPUT
  turnLedOn();                            // Start the blinking.
}

void turnLedOn() {                        // TURN LED ON
  digitalWrite(LED_PIN, HIGH);
  led.runLater(turnLedOff, BLINK_DELAY);  // ...run turnLedOff after BLINK_DELAY
}

void turnLedOff() {                       // TURN LED OFF
  digitalWrite(LED_PIN, LOW);
  led.runLater(turnLedOn, BLINK_DELAY);   // ...run turnLedOn after BLINK_DELAY
}

void loop() {
  led.loop();                             // If time to call a runLater function, do it
}
