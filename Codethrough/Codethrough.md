# Codethrough 
This document will guide you though the code in the IoT_Sketch

A sketch is the name that Arduino uses for a program. It's the unit of code that is uploaded to and run on an Arduino board.
The programming language C++ is divided into main parts: functions, values(variables and constants) and structure. 

# Code 101
## Function()
A function is a group of statements that together perform a task. Every Arduino Board requries a `void setup()` and a `void loop()`. You can define additional functions which makes for better reading.

A simple sketch may look something like this. 

```C++
/**
 * These are 
 * Multiline comments
 */

// These are single line comments

// this function runs once when the Ardunio is powered up
void setup()
{
  // Put some code here  
}

// this runs forever, or until the Arduino is turned off
void loop()
{
    somefunction(); // note that you call a function you created like this 
}

// this is a custom function
void somefunction()
{

}
```
