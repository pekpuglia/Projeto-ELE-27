#ifdef OZONIO
/*******************************************************************************
   Sample the ozone concentration every 60 seconds

   Example code base on high concentration sensor (metal)
   and load resistance of 1MOhms

   Schematics and details available on https://github.com/ostaquet/Arduino-MQ131-driver
 ******************************************************************************
   MIT License

   Copyright (c) 2018 Olivier Staquet

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
 *******************************************************************************/


void OZSetup()
{
  pinMode(OZ_HIGH, INPUT);      // RL= 10kOhms
  pinMode(OZ_LOW, INPUT);       // RL= 800kOhms
  ulTimeRD_OZ = millis();
}

void OZRead() 
{
    stPayloadData.uiOZHigh= analogRead(OZ_HIGH);
    stPayloadData.uiOZLow= analogRead(OZ_LOW);
}
#endif
