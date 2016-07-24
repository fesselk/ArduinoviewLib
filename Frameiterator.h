#pragma once
#include <string.h>
#include <avr/pgmspace.h>
#include <WString.h>

#define SOFRM   0x01
#define EOFRM   0x04
#define ESC     0x10
#define ESCMASK 0x40

// make a uint16_t from a String using this with string literal creates just the int value
#define StrInt( Str ) (uint16_t) ((Str[1] << 8) + Str[0])

/** StringtoFrame
 * converts a string to a Frame by enclosing it in SOFRM and EOFRM following the arduinoview definition
 */
struct StringtoFrame{
//class withstatus.out privacy
    struct{
        enum{ in, esc, out }frame : 4;
        enum{ tstring, pgm_tstring, tvalue }type: 2;
        bool end:1;
    }status;

    size_t i = 0;
    union{
        const char * str;
        PGM_P str_p;
        uint16_t val;
    };
    size_t length = 0;

    StringtoFrame(){
        //NoFrame
        this->length       = 0;
        this->i            = 0;
        this->status.end   = false;
        this->status.frame = status.out;
        this->status.type  = status.tstring;
    }

    StringtoFrame(const char* str){
        //regular String \0 terminated
        this->str          = str;
        this->length       = strlen(str);
        this->i            = 0;
        this->status.end   = false;
        this->status.frame = status.out;
        this->status.type  = status.tstring;
    }

    StringtoFrame(PGM_P str_p, char* mark){
        //programm memory String
        this->str          = str_p;
        this->length       = strlen_P(str_p);
        this->i            = 0;
        this->status.end   = false;
        this->status.frame = status.out;
        this->status.type  = status.pgm_tstring;
    }

    StringtoFrame(const __FlashStringHelper *str_p){
        //programm memory String
        this->str          = reinterpret_cast<PGM_P>(str_p);
        this->length       = strlen_P(reinterpret_cast<PGM_P>( str_p));
        this->i            = 0;
        this->status.end   = false;
        this->status.frame = status.out;
        this->status.type  = status.pgm_tstring;
    }

    StringtoFrame(const char* str, size_t length){
        //a length of bytes
        this->str          = str;
        this->length       = length;
        this->i            = 0;
        this->status.end   = false;
        this->status.frame = status.out;
        this->status.type  = status.tstring;
    }

    StringtoFrame(const uint16_t val, size_t  length ){
        //two bytes by value to frame
        //!! uint16_t is litte endian
        this->val          = val;
        this->length       = length<=2?length:2;
        this->i            = 0;
        this->status.end   = false;
        this->status.frame = status.out;
        this->status.type  = status.tvalue;
    }

    bool addString(const char* str){
        //add an zero terminated string
        if ( this->done() ){
            this->str          = str;
            this->length       = strlen(str);
            this->i            = 0;
            this->status.type  = status.tstring;
            return true;
        }else
            return false;
    }

    bool addString(PGM_P str_p, char* mark){
        //add programm memory String to frame
        if ( this->done() ){
            this->str          = str_p;
            this->length       = strlen_P(str_p);
            this->i            = 0;
            this->status.type  = status.pgm_tstring;
            return true;
        }else
            return false;

    }
    bool addString(const __FlashStringHelper *str_p){
        //add programm memory String to frame
        if ( this->done() ){
            this->str          = reinterpret_cast<PGM_P>(str_p);
            this->length       = strlen_P(reinterpret_cast<PGM_P>(str_p));
            this->i            = 0;
            this->status.type  = status.pgm_tstring;
            return true;
        }else
            return false;

    }
    bool addString(const char* str, size_t  length){
        if ( this->done() ){
            this->str          = str;
            this->length       = length;
            this->i            = 0;
            this->status.type  = status.tstring;
            return true;
        }else
            return false;
    }

    bool addString(const uint16_t val, size_t  length ){
        //add up too two bytes by value to frame
        //!! uint16_t is litte endian
        if ( this->done() && length <= 2){
            this->val         = val;
            this->length      = length;
            this->i           = 0;
            this->status.type = status.tvalue;
            return true;
        }else{
            return false;
        }
    }

    inline char getc(int i){
        char c;
        if(status.type ==  status.tstring){
                c = str[i];
        }else if(status.type ==  status.pgm_tstring){
                c = pgm_read_byte_near( str_p+i);
        }else if(status.type == status.tvalue){
                c= ((char*) &val )[i];
        }
        return c;
    }

    char next(){
      char ret;
      if(status.frame==status.in){
            if(i < length){
                char c= getc(i);
                if( c==SOFRM || c==EOFRM || c==ESC ){
                    status.frame=status.esc;
                    ret = ESC;
                }else{
                    ret = c;
                    i++;
                }
            }else{
                if (status.end) {
                    //end if asked to end
                    ret = EOFRM;
                    status.frame=status.out;
                }
            }
        }else if(status.frame==status.esc){
            char c = getc(i);
            status.frame = status.in;
            ret = ESCMASK ^ c;
            i++;
        }else if(status.frame==status.out){
            if(i==0){
                ret=SOFRM;
                status.frame=status.in;
            }else{
                ret = 0;
            }
        }
        return ret;
    }

    bool done(){
        return (i==length);
    }

    bool end(){
        status.end=true;
        return (status.frame==status.out && i==length);
    }
    bool begin(){
        this->length       = 0;
        this->i            = 0;
        this->status.end   = false;
        this->status.frame = status.out;
        this->status.type  = status.tstring;
    }

};

/** Framereader contains a buffer to read Frames
 * interpretes each character given by calling put(c) until a frame is complete it returns a length() of 0 if the frame is length() returns the number of bytes that are in the frame.
 *
 */


#ifndef FRAMEREADERSIZE
#define FRAMEREADERSIZE 200
#endif
struct Framereader {

    struct{ enum{ in,esc,out,end }frame : 4;} status;
    size_t i = 0;
    const int size =FRAMEREADERSIZE;
    //char stringReceived[size];
    char stringReceived[FRAMEREADERSIZE];
    //Framereader(size_t size=FRAMEREADERSIZE):size(size){};

    Framereader(){};

    size_t put(char c) {
        if( i >= size){
            // frame to large prevent overun
            status.frame == status.out;
            i = 0;
        }
        if(status.frame == status.in){
            if( c == SOFRM){
                //lost a frame but go on with a new one
                status.frame = status.in;
                i = 0;
                stringReceived[i] = 0;
            }else if( c == EOFRM){
                //frame complete
                stringReceived[i] = 0;
                status.frame  = status.end;
                i++;
            }else if( c == ESC ){
                status.frame  = status.esc;

            }else{
                stringReceived [i] = c;
                i++;
            }
        }else if(status.frame = status.esc){
            stringReceived[i] = ESCMASK ^ c;
            i++;
            status.frame  = status.in;
        }else if( status.frame == status.out){
            if( c == SOFRM){
                // start a new frame
                status.frame  = status.in;
                i = 0;
                stringReceived[i] = 0;
            }
        }else if ( status.frame == status.end ){
            return 0;
        }
        return i;
    }

    // length returns length of recived frame 0 if noframe complete
    size_t length() { return status.frame == status.end?i:0; }

    // returns pointer to frame buffer if frame complete
    char * frame(){ return status.frame == status.end?stringReceived:0; }

    //clearframe
    void clearframe(){ status.frame=status.out; }
};
