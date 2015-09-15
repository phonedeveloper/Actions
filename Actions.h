#ifndef Actions_h
#define Actions_h

/*
 * Actions.h
 * 
 * See Actions.cpp for comments on what methods do.
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
#include "Arduino.h"

typedef void (*FunctionCallback)(void); // For function callbacks.

/*
 * Macros to make calls to the instance method version of runLater less wieldy.
 */
#define MethodInstance MemberClassInstance<Callback>::MemberInstance
#define MethodCallback MemberCallback<Callback>::MemberFunction

template <class T> // For method callbacks; the method's instance
struct MemberClassInstance
{
    typedef T* MemberInstance;
};

template <class T> // For method callbacks; the method callback
struct MemberCallback
{
    typedef void (T::*MemberFunction)();
};

/*
 * A macro that says what (tail == head) means
 */
#define QUEUE_IS_EMPTY (tail == head)

/*
 * Callback
 *
 * A class that stores and provides a way to call a function,
 * class method or instance method that takes no parameters
 * and return void.
 */
class Callback
{
private:
    // used if the callback is a function or class method
    FunctionCallback functionCallback = NULL;

    // used if the callback is an instance method
    MethodInstance methodInstance = NULL;
    MethodCallback methodCallback = NULL;

public:
    // constructor when using a function or class method
    Callback(FunctionCallback functionCallback)
        : functionCallback(functionCallback){};
    // constructor when using an instance method
    Callback(MethodInstance methodInstance, MethodCallback methodCallback)
        : methodInstance(methodInstance)
        , methodCallback(methodCallback){};
    Callback(){};   // uninitialized Callback for an unitialized Action
    void call();
};

class Action
{
public:
    Action(Callback callback, unsigned long delay, bool useMicros);
    Action(const Action& source); // copy constructor
    Action();                     // for creating empty Actions for the action queue
    Callback callback;            // the function/method to execute after delay has passed
    bool useMicros;               // delay is in micros() instead of millis()
    unsigned long created;        // time in millis() (or micros()) when the Action was created
    unsigned long delay;          // how long after created() should the action be executed.
};

class Actions
{
private:
    const static int DEFAULT_QUEUE_SIZE = 10; // default queue size; each Action consumes 11 bytes SRAM
    Action* queue;                            // the queue, initialized in the constructor
    int queueSize;                            // the size of the queue, set in the constructor
    int head;                                 // index to queue head (points to first empty slot ahead of newest
                                              // item)
    int tail;                                 // index to queue tail (points to oldest item)
    bool isFull();                            // indicates whether or not the queue is full
    bool isEmpty();                           // indicates whether or not the queue is empty
    int getSize();                            // gets the size of the queue
    int nextSlot(int currentSlot);            // returns index of the queue slot (empty or
                                              // not) after the one passed.
    bool runLater(Action action);             // adds an Action to the queue. Returns false if
                                              // the queue is full.
    bool remove(Action& action);              // removes the oldest Action from the queue and
                                              // stores its action in the provided buffer.
    Callback* overflowCallback;               // points to a callback to be called when queue
                                              // would overflow on runLater()

public:
    Actions(int size = DEFAULT_QUEUE_SIZE); // constructor
    void loop(); 
    bool runLater(FunctionCallback functionCallback, unsigned long delay, bool useMicros = false);
    bool runLater(MethodInstance methodInstance,
                  MethodCallback methodCallback,
                  unsigned long delay, bool useMicros = false);
    void setOverflowCallback(FunctionCallback functionCallback);
    void setOverflowCallback(MethodInstance methodInstance, MethodCallback methodCallback);
};

#endif
