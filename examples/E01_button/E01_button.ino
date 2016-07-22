// -----------------------------------------------------------------
// Examples of the ArduinoView Library
// 1. Button
// -----------------------------------------------------------------
// This file demonstrates the basic use of the library by generating
// a button (labeled by "Start") based on the shorthand mechanisms.
// The execution waits for a click and transmits a "Program started"
// afterwards 

#include <FrameStream.h>
#include <Frameiterator.h>

#define OUTPUT__BAUD_RATE 57600
FrameStream frm(Serial);

// Hiracical runner list, that connects the gui elements with 
// callback methods
// First level
beginrunnerlist();
fwdrunner(!g, BUTTONrunnerlist); //Reference to the second level (BUTTON)
endrunnerlist();
// Second level
beginrunnerlist(BUTTON);
runner(GO, buttonCallback);
endrunnerlist();

int go;

void buttonCallback(char * str, size_t length){
  go = 1;
}

void setup() {
  Serial.begin(OUTPUT__BAUD_RATE);
  
  go = 0;
  pinMode(13, OUTPUT);
  frm.print("!h<h1>ArduinoView</h1> <h2>Demo 1 - Button </h2>");
  frm.print("<p>Please press the button to start the program!</p>");
  frm.end();
  // Generation of the button
  // !S<E><ID><DATA>
  // Start      Element     ID         v=Value 
  // indicator  b=Button    [2 char]   
  // !S         b           GO         vStart
  frm.print("!SbGOvStart");
  frm.end();
}

void loop() {
  frm.run();
  if (go==1){
    frm.print("!!"); 
    frm.end();
    delay(1000);
    frm.print("!h The program was started, LED is blinking ones");
    frm.end();
    digitalWrite(13,1);
    delay(2000);
    go = 0;
    digitalWrite(13,0);
  }
}
