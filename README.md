# Actions
An Arduino library for doing things in the future, without using `delay()`, and without getting into timers or interrupts.

You can have multiple functions waiting to be run at the same time.

It's a pretty simple library, with just a few methods!

## Installation
From the command line, go to your **sketchbook** folder. Inside is a folder named **libraries** (if not, create it). **cd** to the **sketchbook\libraries** folder, and if there is no other library named **Actions** in that folder, run the following command:

`$ git clone https://github.com/phonedeveloper/Actions.git`

This will install the Actions library and some examples. Restart the Arduino app to see them.

## Key Functions

`Action actions` - creates an Action queue called **actions**

`actions.runLater(function, delay)` - adds a **function** to the queue, to be called **delay** milliseconds in the future.

`actions.loop()` - call this repeatedly from your sketch's **loop()**. It looks at the queue and calls any **function** whose **delay** has passed since it was added. 

**Power users**: there's a version of **runLater** for calling instance methods of classes. See the **Power Users** section below.

## Example
This example blinks the LED attached to pin 13 without using `delay()`.

```
#include <Actions.h>
Actions led;

const static int LED_PIN=13;
const static int BLINK_DELAY=500;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  turnLedOn();
}

void turnLedOn() {
  digitalWrite(LED_PIN, HIGH);
  led.runLater(turnLedOff, BLINK_DELAY); 
}

void turnLedOff() {
  digitalWrite(LED_PIN, LOW);
  led.runLater(turnLedOn, BLINK_DELAY);
}

void loop() {
  led.loop();
}
```

## Caveats

If you don't call **actions.loop()** often enough, it won't be able to check to see if it's time to run a stored action and when **actions.loop()** eventually does run, the action might run much later than you intended.

If you add functions using **runLater()** faster than they are executed later, you will eventually overflow the Action queue. See the **Queue Size** section to see how to change the queue size.

Actions should not be used for accurate timing, because the time an action is called will always be some time after the timeout expires, and perhaps much later, if some other function in the sketch's **loop()** method or an executing action is still running when an action becomes ready to run.

## Some usage tips

Sometimes you will want an action to be repeated some time later, as shown in the example above. For this to happen, you should call `runLater()` before you return from the action. If you only need to run an action once, just let it return.

The included example **BlinkUsingStateMachine** gives an example of how you might use Actions to create a state machine where some state changes can't occur until a timeout happens.

Be sure to call `actions.loop()` repeatedly, such as from within your sketch's `loop()` method, keeping in mind any long-running operations might delay the execution of a future action. Conversely, if your loop and actions are short (testing and setting digital lines, for instance), you can process thousands of actions per second.

### Under the hood

Only one action is called per loop, even if several actions happen to be ready at the same time. If multiple actions come ready during the same `loop()`, the first one that was added (the older action) will be the first one that is run.

Take a look at the source code for **Action::loop()** and **Action::runLater()** if you want to understand more about how actions are added and removed from the queue.

### Queue Size (Memory Usage)

So that there are no surprises when it comes to memory, the queue size is fixed to ten slots when Actions is created without specifying the queue size (see **Power Users** below).

That the number of actions that can be waiting for execution is fixed when you instantiate Actions. If you call `actions.runLater()` and it returns `false`, then you've just tried adding an action to a full queue.

I thought about making the queue dynamic (grows in size when you add an action, shrinks when an action is called) but I wanted the predictability of fixed memory consumption.

I figured it would be easier to debug an app where actions are added faster than they're executed, by having a call to add() fail because the queue filled, rather than having the queue grow to consume *most* of the memory but the last byte of memory is consumed in some arbitrary function.

You can increase (or decrease) the size of the queue by editing the source. Each additional entry in the queue consumes 11 more bytes, if I recall correctly.

Finally, in the example, I've named my instance of the Action class `actions` for legibility. You can name it anything you want, and you can have multiple queues, named differently of course, and each with its own `loop()` in your sketch's `loop()`.

## Power Users
### Changing the queue size
Simply specify the desired queue size when you instantiate actions, as in
```
Actions actions(5);    // queue size is 5 slots
```
### Using an instance method instead of a function
You cannot call **runLater(function, delay)** and pass an instance method, even if the instance method returns void and has no parameters. This is because the instance method actually has one parameter, the self parameter, which identifies the instance that the method belongs to.

To get around this, another **runLater(instance, method, delay)** function is provided that takes the method and its instance as parameters. Please see the instance method test in the **ActionsTest** example to see how this is used.

This is provided for people who want to use Actions
in a library that uses classes. Many Arduino
developers will never need this.

### Ensuring all high priority actions are called before any low priority actions
If you need to prioritize actions, you can do this by creating two queues:

```
Action highPriorityActions;
Action lowPriorityActions;

void loop() {
  if (!highPriorityActions.loop()) {
    lowPriorityActions.loop();
  }
}
```
This is an equivalent implementation:

```
void loop() {
  while(highPriorityActions.loop());
  lowPriorityActions.loop();
}
```

The `loop()` method returns **true** if an action was called, and **false** if not. This code ensures that if multiple high priority actions are ready to go at the same time, they are all executed before any low priority actions are called. It also makes sure that only one low priority action is run so that if a high priority action becomes ready, it runs before a second low priority action that becomes ready just before it.

If a low priority action is running and taking its jolly time, and a high priority action becomes ready, the high priority action is not going to run until the low priority action completes.

Remember that each call to the action `loop()` only results in at most one action call. If several actions are ready, it will take the same number of calls to the action queue's `loop()` to process them all.
## License
(c) 2015, PhoneDeveloper LLC

Licensed under the BSD license. See the LICENSE file.

## Author
Sean Sheedy, PhoneDeveloper LLC