/*
 * Actions.cpp
 *
 * A library for running functions or methods after a delay has passed,
 * without blocking, using delay() or interrupt, and on a single thread.
 *
 * This is a useful replacement for delay(), enabling timeouts and state
 * machines that require some time to pass before moving to the next
 * state while allowing other stuff to happen in the meantime.
 * 
 * To have a function or method called later, create a function or
 * instance method with no arguments or return value, and use the public
 * runLater(funcion, delay) or runLater(instance, method, delay)
 * method to request it to be run after a delay time has passed.
 *
 * Requires frequently calling the instance method loop() from the
 * sketch's loop() function to check for and run functions or methods
 * that are ready to be run.
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
#include "Actions.h"





/************************************************************
 * PUBLIC METHODS THAT MOST SKETCHES WILL USE               *
 *                                                          *
 * The constructor and two methods below are all that most  *
 * sketches will ever need to use.                          *
 ************************************************************/

/*
 * constructor - creates an instance of the action processor
 * 
 * Functions or instance methods stored by runLater are stored as an "Action"
 * which includes the name (and instance) of the function (method), when
 * the callback was requested, and how much time should be passed before
 * it is called.
 * 
 * This constructor sets up a queue of the specified size. The size parameter
 * is optional and if omitted, a queue of DEFAULT_QUEUE_SIZE is created.
 * If you need a larger queue, or want to save memory by specifying a smaller
 * queue, you would provide the size parameter.
 */
Actions::Actions(int size)
{
    if(size < 0)
        queueSize = 0; // don't make a queue if size improperly specified
    size += 1;         // this queue requires one slot to always be empty
    queue = new Action[size];
    queueSize = size;        // save the desired queue size
    head = tail = 0;         // point head and tail to start of queue; queue empty
    overflowCallback = NULL; // no overflow callback
}

/*
 * loop() - processes actions stored using runLater
 *
 * Looks in the queue containing actions stored there by runLater,
 * and if one is found to be ready to execute, calls the function or
 * instance method stored in the action and removes it from the queue.
 *
 * See the margin comments for an explanation of how the queue is
 * processed.
 */
void Actions::loop()
{
    Action action;
    for(int i = 0; i < getSize(); i++) // walk through queue at most one time each pass
    {
        remove(action); // remove action from the queue
        unsigned long now = action.useMicros ? micros() : millis();
        if(now - action.created > action.delay) // time to call function/method?
        {
            action.callback.call(); // call it (it's not placed back on the queue)
            break;                  // at most one method called per call to loop()
        }
        else // else not time to call function/method...
        {
            runLater(action); // put it back on the queue
        }
    }
}

/*
 * runLater - requests the supplied function is run at a later time.
 *
 * The caller must provide the name of the function to run, as well 
 * as the delay which defaults to milliseconds.  Use this if the function
 * to be called is not part of a class or is a class (not instance) method.
 * 
 * The function to be called must take no parameters and return void.
 *
 * useMicros is an optional parameter that if not specified defaults to
 * false. When set true, the delay parameter is in microseconds instead of
 * milliseconds. Using microseconds requires calling the Action
 * instance's loop() method extremely frequently, and the best resolution
 * is only tens or hundreds of microseconds. 
 * 
 * An example call to this method would look like this, where function is
 * void someFunction():
 *
 * unsigned long delay = 1000ul;
 * bool result = runLater( someFunction, delay );
 *
 * Note the lack of parenthesis after the function name.
 */
bool Actions::runLater(FunctionCallback functionCallback, unsigned long delay, bool useMicros)
{
    Action action(Callback(functionCallback), delay, useMicros);
    return runLater(action);
}





/*************************************************************************
 * PUBLIC METHODS THAT FEW SKETCHES WILL USE                             *
 *                                                                       *
 * The methods below are those that most sketches will ever need to use. *
 * Sketches that use classes might need this, and library authors that   *
 * wish to use Actions with instance methods will need this.             *
 *************************************************************************/

/*
 * runLater - requests the supplied instance method is run at a later time.
 *
 * The caller must provide a reference to the instance and the name of the
 * instance method to run, as well as the delay which defaults to milliseconds. 
 * 
 * The method to be called must take no parameters and return void.
 *
 * useMicros is an optional parameter that if not specified defaults to
 * false. When set true, the delay parameter is in microseconds instead of
 * milliseconds. Using microseconds requires calling the Action
 * instance's loop() method extremely frequently, and the best resolution
 * is only tens or hundreds of microseconds. 
 * 
 * An example call to this method, from another instance method in the
 * same class as the method that will be called later, would look like
 * this (the class name is MyClass and the method to be called is named
 * someMethod().)
 *
 * unsigned long delay = 1000ul;
 * bool result = runLater( (MethodInstance) this, 
 *                         (MethodCallback) &MyClass::someMethod, 
 *                         delay );
 *
 * Note the lack of parenthesis after the method name.
 */
bool Actions::runLater(MethodInstance methodInstance, MethodCallback methodCallback, unsigned long delay, bool useMicros)
{
    Action action(Callback(methodInstance, methodCallback), delay, useMicros);
    return runLater(action);
}





/*************************************************************************
 * PUBLIC METHODS THAT FEW SKETCHES WILL USE                             *
 *                                                                       *
 * Since runLater already returns false if a queue is full, most         *
 * sketches will not need this.  These are provided to allow someone     *
 * expecting a rare queue full event to capture that info in one place   *
 * (though it won't tell them where it happened.)                        *
 *************************************************************************/

/*
 * setOverflowCallback - set a function to be called if runLater is called on a full queue
 */
void Actions::setOverflowCallback(FunctionCallback functionCallback)
{
    *overflowCallback = Callback(functionCallback);
}

/*
 * setOverflowCallback - set an instance method to be called if runLater is called on a full queue
 * 
 * Call this using the same technique for the runLater method that takes a class instance
 * and instance method as parameters.
 */
void Actions::setOverflowCallback(MethodInstance methodInstance, MethodCallback methodCallback)
{
    *overflowCallback = Callback(methodInstance, methodCallback);
}





/***********************************************
 * METHODS THAT SKETCHES WILL *NOT* USE        *
 *                                             *
 * The methods below are used by the library.  *
 ***********************************************/

/*
 * call - Calls a callback function or method
 *
 * Two types of callbacks can be stored in Callback:
 * - an ordinary function (not part of a class)
 * - a method in an instantiated class
 *
 * This method calls the type currently stored.
 */
void Callback::call()
{
    if(functionCallback != NULL)
    { // callback is a function
        functionCallback();
    }
    else if(methodInstance != NULL && methodCallback != NULL)
    { // callback is an instance method
        (methodInstance->*methodCallback)();
    }
}

/*
 * constructor - creates an Action
 *
 * Stores the callback function/method as stored in an instance of Callback,
 * the time the function was stored (in millis() or micros()) and the
 * delay that must pass before the callback can be called.
 */
Action::Action(Callback callback, unsigned long delay, bool useMicros)
    : callback(callback)
    , delay(delay)
    , useMicros(useMicros)
{
    if(!useMicros)
        created = millis();
    else
        created = micros();
}

/*
 * copy constructor - creates a copy of an Action
 *
 * Creates a copy of the source object.
 */
Action::Action(const Action& source)
{
    callback = source.callback;
    delay = source.delay;
    created = source.created;
    useMicros = source.useMicros;
}

/*
 * empty constructor - creates an empty action
 */
Action::Action()
{
    callback = Callback();
}

/*
 * getSize - returns the number of slots available in the action queue
 */
int Actions::getSize()
{
    if(head > tail)
        return head - tail;
    else if(!QUEUE_IS_EMPTY)
        return queueSize - (tail - head);
    else
        return 0;
}

/*
 * isFull - indicates whether or not the action queue has any room left
 */
bool Actions::isFull()
{
    if(queueSize <= 1)
        return true;
    if(head == queueSize - 1) // is head at last slot in queue?
    {
        return (tail == 0); // then queue is full if tail is at first slot
    }
    else // but if head anywhere else
    {
        return (tail == head + 1); // then queue is full if tail is one past head
    }
}

/*
 * nextSlot - returns the index of the slot one past the current slot
 * 
 * This does not check to see if the queue is full, so call isFull()
 * before using this to obtain a free slot.
 */
int Actions::nextSlot(int currentSlot)
{
    if(currentSlot == queueSize - 1) // if current slot is last slot of queue
    {
        return 0; // then next slot is first slot
    }
    else
    {
        return currentSlot + 1; // otherwise first slot is one past next
    }
}

/*
 * runLater - places an Action into the queue
 *
 * Places a copy of the provided Action into the head
 * of the queue and returns true. If the queue is full,
 * the copy does not occur, the overflowCallback is
 * called if it is present, and false is returned.
 */
bool Actions::runLater(Action action)
{
    if(!isFull())
    { // queue has space; add a copy of the action
        queue[head] = Action(action);
        head = nextSlot(head);
        return true;
    }
    else
    {
        if(overflowCallback != NULL)
        {
            overflowCallback->call();
        }
        return false;
    }
}

/*
 * remove - retrieves a copy of the action at the end of the queue and removes it.
 * 
 * Returns false (and doesn't retrieve an action) if the queue is empty.
 */
bool Actions::remove(Action& action)
{
    if(!QUEUE_IS_EMPTY)
    {
        action = Action(queue[tail]);
        tail = nextSlot(tail);
        return true;
    }
    else
        return false;
}