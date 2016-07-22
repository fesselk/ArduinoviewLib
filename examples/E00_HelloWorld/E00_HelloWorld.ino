// -----------------------------------------------------------------
// Examples of the ArduinoView Library
// 0. Hello World 
// -----------------------------------------------------------------
// This example demonstates the integration of html content within
// the arduino code

#include <FrameStream.h>
#include <Frameiterator.h>

#define OUTPUT__BAUD_RATE 57600
FrameStream frm(Serial);

// We do not have any interaction in this example, but a default
// callback definition is mandatory.
beginrunnerlist();
fwdrunner(!g,defaultcall);
endrunnerlist(); 

void defaultcall(char* cs, int len){ 
}

void setup() {
  Serial.begin(OUTPUT__BAUD_RATE); 
  frm.print("!h<h1>ArduinoView</h1> <h2>Demo 0 - Hello World </h2>");
  frm.print("<p>Congratulation you transmitted the first HTML Code from your Arduino!!!</p>");
  frm.print("<img src=\"https://upload.wikimedia.org/wikipedia/commons/8/87/Arduino_Logo.svg\" height=\"200\" width=\"200\" >");
  frm.end();
}

void loop() {
  frm.run();
  // just for avoiding a continous refresh of the website
  delay(1000);          
}
