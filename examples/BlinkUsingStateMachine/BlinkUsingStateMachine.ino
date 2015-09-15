/*
 * Blink As a State Machine
 *   
 * Blinks an LED without using delay() using the Actions library.
 * This version shows how Actions can be used to drive a state
 * machine, that allows other things to occur in the sketch's
 * loop() between state changes.
 *
 * You should glance at the code below before reading the rest
 * of this comment.
 *  
 * The method changeState() turns the LED on or off depending on
 * the current state and sets the next state accordingly. It
 * then uses runLater() to call itself again when it's time to
 * switch the LED back. It is basically ping-ponging
 * the LED on or off continuously.
 *  
 * In loop(), machine.loop() checks to see whether or not it's time
 * to run the function specified by runLater() (changeState() in this 
 * case). If so, it runs it, then exits. If not, it simply exits.
 * Therefore, machine.loop() must be called repeatedly by the sketch. 
 * Here we call it from within the sketch's loop() function. In 
 * between calls to machine.loop(), you can do other things. But if 
 * they hog the loop (take too much time before returning), 
 * machine.loop() won't be called in time, and the LED will appear 
 * to be blinking more slowly, erratically, or not at all.
 *
 * A final note. The declaration
 *    Actions machine(1);
 * tells Actions to create only one slot for a function to be
 * run later. Since we call runLater only once before the function
 * is actually run, we only need one slot. We would want more slots
 * if we need to have more than one function pending. 
 *  
 * If instead we had used
 *    Actions machine;
 * it would have still worked. But Actions would have assigned the
 * default number of slots, which is 10, each taking up 11 bytes
 * of memory. Actions does not automatically grow the number of slots.
 *
 * Connections:
 *   * Uses the LED on the Uno and Mega boards that's on pin 13.
 *
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

#include <Actions.h>
Actions machine(1);

const static int LED_PIN = 13;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  changeState();              // Calling this starts the blinking.
}

void loop() {
  machine.loop();             // Calls the function set by runLater if it's time, otherwise exits quickly.
  // do other stuff here
  // as long as it doesn't hog the loop
}

/*
 * Handles turning the LED on and off.
 * 
 * While this is a bit verbose for turning an LED on or off, the
 * point is to show how you would use a switch() to check which
 * state you are in and transition to another state if you were
 * building something more complex.
 */
void changeState() {
  
  // Possible states.
  const static int STATE_OFF = 0;
  const static int STATE_ON  = 1;  

  // Because it's static, currentState is saved between calls.
  static int currentState = STATE_OFF;         // only initialized during the first call.

  const static int OFF_TIME = 900;
  const static int ON_TIME  = 100;
  
  switch(currentState) {
    
    case STATE_OFF:                            // starting state.
      digitalWrite(LED_PIN, HIGH);             // action to take.
      currentState = STATE_ON;                 // ending state.
      machine.runLater(changeState, ON_TIME);  // set time for next state change.
      break;
      
    case STATE_ON:
      digitalWrite(LED_PIN, LOW);
      currentState = STATE_OFF;
      machine.runLater(changeState, OFF_TIME);
      break;
  }
}