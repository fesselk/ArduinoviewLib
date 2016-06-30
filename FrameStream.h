#pragma once
#include <Stream.h>
#include <Frameiterator.h>

#define INNERCAT(x, y) x ## y
#define CAT(x, y) INNERCAT(x, y)

#define beginrunnerlist(NAME) void NAME##runnerlist(char * frm , size_t len){\
    if (len<2) return;\
    uint16_t head = (frm[0] << 8) | (frm[1]);
#define runner(ID,runner_function)\
    uint16_t CAT(fnhead,__LINE__) = (#ID[0] << 8)+ #ID[1];\
    if (CAT(fnhead,__LINE__)  == head) runner_function(frm,len)
#define fwdrunner(ID,runner_function)\
    uint16_t CAT(fnhead,__LINE__) = (#ID[0] << 8)+ #ID[1];\
    if (CAT(fnhead,__LINE__)  == head) runner_function(&frm[2],(len-2))
#define endrunnerlist() }

// beginrunnerlist(g);
// endrunnerlist();
//
// beginrunnerlist();
// runner(!!,gui_init);
// runner(BB,test);
// fwdrunner(!g,grunnerlist);
// endrunnerlist();

void runnerlist(char * frm , size_t len);

struct FramePrint : public Print{
    StringtoFrame framer;
    Stream &ser; // expect Serial interface
    FramePrint(Stream &serial):ser(serial){
    }

    size_t write(uint8_t c){
        if( framer.addString( (char) c , 1 ) ) {
            while(!framer.done()){
                ser.write(framer.next());
            }
        return 1;
        } else return 0;
    }

    size_t write(const uint8_t *buffer, size_t size){
        if( framer.addString( (const char *)buffer , size )){
            while(!framer.done()){
                ser.write(framer.next());
            }
            return size;
        } else return 0;
    }

    void end(){
        while(!framer.end())
            ser.write(framer.next());

    }

};

struct FrameStream : public FramePrint{
    Framereader deframer;
    FrameStream(Stream &serial):FramePrint(serial){
    }

    bool run(){
        if (ser.available())                              // wait for SYNC package
        {
            if(deframer.length()==0)
                deframer.put(ser.read());
            if(deframer.length() != 0){ // frame might be complete after puting char in
                if(deframer.length() >= 2)
                    runnerlist(deframer.frame(),deframer.length());
                deframer.clearframe();
            }
        }
        return ser.available();
    }

//  these would be need to complete the Stream interface
//  int available();
//  int read();
//  int peek();
//  void flush();
};

struct FrameRun {
    Framereader deframer;
    Stream &ser;
    FrameRun(Stream &serial):ser(serial){
    }

    bool run(){
        if (ser.available())                              // wait for SYNC package
        {
            if(deframer.length()==0)
                deframer.put(ser.read());
            if(deframer.length() != 0){ // frame might be complete after puting char in
                if(deframer.length() >= 2)
                    runnerlist(deframer.frame(),deframer.length());
                deframer.clearframe();
            }
        }
        return ser.available();
    }
};
