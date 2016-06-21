#pragma once
#include <Stream.h>
#include <Frameiterator.h>

// class Stream{ //effective virtual functions
//    virtual size_t write(uint8_t) = 0;
//    virtual size_t write(const uint8_t *buffer, size_t size);
//    virtual int available() = 0;
//    virtual int read() = 0;
//    virtual int peek() = 0;
//    virtual void flush() = 0;
//}


#define INNERCAT(x, y) x ## y
#define CAT(x, y) INNERCAT(x, y)

//{Serial.print(head,HEX);}

#define begineventlist(NAME) void NAME##eventlist(Framereader &frm_in){\
    uint16_t head = (frm_in.frame()[0] << 8) | (frm_in.frame()[1]);
#define event(ID,event_function)\
    uint16_t CAT(fnhead,__LINE__) = (#ID[0] << 8)+ #ID[1];\
    if (CAT(fnhead,__LINE__)  == head) event_function(frm_in.frame(),frm_in.length())
#define endeventlist() }

// begineventlist();
// event(!!,gui_init);
// event(BB,test);
// endeventlist();

void eventlist(Framereader&);

struct FrameStream : public Print{
    StringtoFrame framer;
    Stream &ser; // expect Serial interface
    Framereader deframer;
    FrameStream(Stream &serial):ser(serial){
    }
    
    size_t write(uint8_t c){
        if( framer.addString( (char) c , 1 ) ) {
        while(!framer.done()){
            ser.write(framer.next());
        }
        return 1;}
    }
    
    size_t write(const uint8_t *buffer, size_t size){
       if( framer.addString( (const char *)buffer , size )){
        while(!framer.done()){
            ser.write(framer.next());
        }
        return size;}
        return 0;
    }
    
    void end(){
        while(!framer.end())
            ser.write(framer.next());
        
    }
    
    void run(){
        if (ser.available())                              // wait for SYNC package
        {
            if(deframer.length()==0)
                deframer.put(ser.read());
            if(deframer.length() != 0){ // frame might be complete after puting char in
                if(deframer.length() >= 2)
                    eventlist(deframer);
                deframer.clearframe();
            }
        }
    }
    
//     int available();
//     int read();
//     int peek();
//     void flush();
};
