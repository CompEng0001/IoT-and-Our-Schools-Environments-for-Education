# Codethrough 
This document will guide you though the code in the IoT_Sketch

## Contents

- [Sketch](#sketch)
- [Function()](#function)
- [Examples](#examples)

# Code 101

## Sketch 
A sketch is the name that Arduino uses for a program. It's the unit of code that is uploaded to and run on an Arduino board.
The programming language C++ is divided into main parts: functions, values(variables and constants) and structure. 

A simple sketch may look something like this. 

```C++
/**
 * These are 
 * Multiline comments
 */

// These are single line comments
int a = 0; // intialise an integer (int) a with 0
int b = 0, c = 0; // intialise two integers b and c with 0

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


## Function()
A function is a group of statements that together perform a task. Every Arduino Board requries a `void setup()` and a `void loop()`. You can define additional functions which makes for better reading.

### No return and no parameter 

Normally a function with no return and/or no parameters looks like `void somefunction()` this is *implicit* but it can look like `void somefunction(void)` this is *explicit* you can see some the examples below that we will be implementing through the project. 

```C++
void somefunction()
{
    int a = 5, b = 4, c = 0;
    
    c = a + b; // add a to b and make it too  

    Serial.print(a + b = );
    Serial.println(c);
}
```

So the `void` means no return, therefore the  variables `a,b,c` in the function only exist when the function is called. 

### Return and parameter

```C++
int somefunction(int l_a, int l_a)
{
int c = 0;
    
    c = l_a + l_b; 

    Serial.print(a + b = );
    Serial.println(c);
    return c;
}
```
So what is different here, `int somefunction(int l_a, int l_b)`?  Well first of all the function is expecting two paraments both of which are integers and are assigned as l_a and l_b (this is local a and local b).
Secondly there is a expected return for the function which is also an integer. Within the function before the final brace `}` there is a `return c` this returns the integer c. 

> **Note** 
> Functions can be mixture of the two examples above, here are two more... 
> `void somefunction(int a)`
> `int somefunction()`


## Examples

Here is an example of adding to integers together by using a function and displaying the sum in the serial monitor.

```C++

int a = 5, b = 4, additionResult = 0;

void setup()
{
    Serial.begin(9600); //  here beginning the serial line for transmission , inbound and outbound at a baud rate of 9600 bits per second.  
}

void loop()
{
    additionResult = addTwoInts(a, b);

    Serial.print( a + b = );
    Serial.println(additionResults);
}

int addTwoInts(int l_a, int l_b)
{
    int c = 0;
    
    c = l_a + l_b; 

    Serial.print(a + b = );
    Serial.println(c);
    return c;
}

```C++

