#line 2 "actions_test.ino"
#include <ArduinoUnit.h>

/*
 * ActionsTest - test the Actions class
 * 
 * This sketch tests the Actions class. It outputs the
 * test results through the main serial port.
 * 
 * Uses arduinounit from https://github.com/mmurdoch/arduinounit
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

/*
 * negativeQueueSizeTest - tests negative queue size
 * 
 * Nobody should specify a negative queue size. This
 * confirms that when an Actions instance is created
 * with a negative queue size, you cannot add an action.
 */
test(negativeQueueSizeTest) {
  Actions actions(-1);
  assertEqual(false, actions.runLater(dummy, 0));
}

/*
 * queueSizeZeroTest - tests queue size of zero
 * 
 * Nobody should specify a queue size of zero. This
 * confirms that when an Actions instance is created
 * with no queue, you cannot add an action.
 */
test(queueSizeZeroTest) {
  Actions actions(0);
  assertEqual(false, actions.runLater(dummy, 0));
}

/*
 * queueSizeOneTest - tests queue size of one.
 * 
 * This ensures that when an Actions instance
 * has only a single queue slot, you can add one
 * action but adding the second one fails. It also
 * verifies that when that action has executed,
 * a second one may be added.
 */
int queueSizeOneTestFunctionRan = 0;
test(queueSizeOneTest) {
  Actions actions(1);
  // should be able to add exactly one function with runLater
  assertEqual(true, actions.runLater(queueSizeOneTestFunction, 0));
  assertEqual(false, actions.runLater(queueSizeOneTestFunction, 0));
  // ensure the test function runs just once
  delay(2);         // let millis() increment
  assertEqual(0, queueSizeOneTestFunctionRan);
  actions.loop();   // run the queue. dummy should excute
  assertEqual(1, queueSizeOneTestFunctionRan);
  actions.loop();   // run the queue again. dummy should not excute again
  assertEqual(1, queueSizeOneTestFunctionRan);
  // now, should be able to add exactly one function with runLater
  assertEqual(true, actions.runLater(queueSizeOneTestFunction, 0));
  assertEqual(false, actions.runLater(queueSizeOneTestFunction, 0));
  // and the test function should again be called only once
  delay(2);         // let millis() increment
  assertEqual(1, queueSizeOneTestFunctionRan);
  actions.loop();   // run the queue. dummy should excute
  assertEqual(2, queueSizeOneTestFunctionRan);
  actions.loop();   // run the queue again. dummy should not excute again
  assertEqual(2, queueSizeOneTestFunctionRan);
}

/*
 * Passed to runLater in the test above.
 */
void queueSizeOneTestFunction() {
  queueSizeOneTestFunctionRan++;
}

/*
 * queueSizeTwoTest - tests queue size of two.
 * 
 * This ensures that two actions can be added but the third fails.
 */
test(queueSizeTwoTest) {
  Actions actions(2);
  assertEqual(true, actions.runLater(dummy, 0));
  assertEqual(true, actions.runLater(dummy, 0));
  assertEqual(false, actions.runLater(dummy, 0));
}

// Used for runLaterTest.
unsigned long runLaterTestStartTime = 0;
unsigned long runLaterTestEndTime = 0;
Actions runLaterTestAction(1);

/*
 * runLaterTest - tests runLater function
 * 
 * This tests checks to see if runLater executes within 5 ms of when we expect it to.
 * It tests a delay of 0 ms and 250 ms.
 */
test(runLaterTest) {
  unsigned long testStart = millis();
  runLaterTestAction.runLater(runLaterTestFunction, 0);
  // Run Actions.loop() for 300 milliseconds, which is enough to run the test
  while (millis() < testStart + 300ul) runLaterTestAction.loop();
  unsigned long testEnd = millis();
  assertLess(testStart, runLaterTestStartTime);
  assertMore(testStart + 5ul, runLaterTestStartTime);
  assertLess(runLaterTestEndTime + 45ul, testEnd);
  assertMore(runLaterTestEndTime + 55ul, testEnd);
}

/*
 * A dummy function for passing to runLater.
 */
void dummy() {}

/*
 * A function for passing to runLater for the test named "runLaterTest".
 */
void runLaterTestFunction() {
  const static int STATE_OFF = 0;
  const static int STATE_RUNNING = 1;
  static int state = STATE_OFF;
  
  switch(state) {
    case STATE_OFF:
      runLaterTestStartTime = millis();
      state = STATE_RUNNING;
      runLaterTestAction.runLater(runLaterTestFunction, 250);
      break;
    case STATE_RUNNING:
      runLaterTestEndTime = millis();
      break;
  }
}

/*
 * A class used for verifying that Actions runLater works not only
 * on functions but also on non-static members of instantiated classes.
 */
class TestClass {
private:
  unsigned long startTime = 0;
  unsigned long endTime = 0;
  Actions testActions;
public:
  void loop() {
    testActions.loop();
  }
  void start() {
    startTime = millis();
    testActions.runLater((MethodInstance) this, (MethodCallback) &TestClass::testFunction, 250);
  }
  void testFunction() {
    endTime = millis();
  }
  unsigned long getStartTime() {
    return startTime;
  }
  unsigned long getEndTime() {
    return endTime;
  }
};

/*
 * runLaterUsingClassMethod - tests use of class method
 * 
 * This test exercises the class above to check whether or not
 * class methods can be provided to the function version of runLater.
 */
test(runLaterUsingClassMethod) {
  TestClass testClass;
  unsigned long startTime = millis();
  delay(2);
  testClass.start();
  while (startTime + 300 > millis()) {
    testClass.loop();
  }
  unsigned long endTime = millis();
  assertLess(startTime, testClass.getStartTime());
  assertMore(startTime + 4, testClass.getStartTime());
  assertLess(endTime - 52, testClass.getEndTime());
  assertMore(endTime - 46, testClass.getEndTime());
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  Test::run();
}