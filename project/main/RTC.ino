/*
 * RTCAdjust.cpp
 *
 *  Created on: Dec 10, 2018
 *
 *  @license MIT use at your own risk
 *
 *  Copyright 2018 andrew goh
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/* note that the adjustment functions needs this RTClock
 * if you rename variable rt, update rtadjust.h so that the
 * extern refers to this RTClock*/
#include <Arduino.h>
#include "rtadjust.h"

void processkey();
void showtime();
void synctime(char *, int);
void settime(char *, int);
void calibrate(char *, int );
void setdriftdur(char *, int);
void help();
void clearbuf();


tm_t tm;    //Usado para o RTC
 
RTClock rt(RTCSEL_LSE); // initialise


void rtcStart(void)
{
      /* initialise access to backup registers,
   * this is necessary due to the use of backup registers */
  bkp_init();

  /* adjust rtc */
  adjtime();
  sDataHora[0]= 0;
}

void synctime(char *buf, int len) {
  if (len == BUFLEN) buf[BUFLEN-1] = 0; //terminate the string for safety
  if(parsetimestamp(buf, tm) <0) {
    DEB.println(F("invalid date/time"));
    return;
  }

  time_t time = rt.makeTime(tm);
  /* this call the actual function to set the RTC and update
   * the last adjusted time simultaneously
   */
  synctime(time);
}



void calibrate(char *buf, int len) {
  if (len == BUFLEN) buf[BUFLEN-1] = 0; //terminate the string for safety
  if(parsetimestamp(buf, tm) <0) {
    DEB.println(F("invalid date/time"));
    return;
  }

  time_t time = rt.makeTime(tm);

  /* this call the calibratertc() function to calculate
   * the drift duration
   */
  calibratertc(time);
}

/*
 * this function sets the rtc directly by-passing all the adjustments
 *
 * note that this function is used during tests to simulate drifts etc
 * hence it is not featured in help();
 *
 * in a normal context use synctime() to set the RTC time so that
 * the last adjustment date/time is updated as well
 */
void settime(char *buf, int len) {
  if (len == BUFLEN) buf[BUFLEN-1] = 0; //terminate the string for safety
  if(parsetimestamp(buf, tm) <0) {
    DEB.println(F("invalid date/time"));
    return;
  }
  rt.setTime(tm);
}

void setdriftdur(char *buf, int len) {
  if (len == BUFLEN) buf[BUFLEN-1] = 0; //terminate the string for safety
  int16_t drift_dur = atoi(buf);
  /* this funciton updates the drift duration directly */
  setbkpdrift(drift_dur);
}
