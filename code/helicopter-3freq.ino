/* S107 3-channel with checksum helicopter control code
 * Copyright (C) 2012, Andrew Barry, Dan Barry
 *
 * Uses an Arduino to control a S107 helicopter
 *
 *
 * Instructions:
 *  Connect an IR LED array to pin 8 (using a FET to amplify the signal)
 *  and use the serial monitor to send commands to the system
 *
 */



#define LED 8

#define STATUS 13

//#define TAKEOFF_THROTTLE 240
//#define HOLDING_THROTTLE 130

byte yawCmd, pitchCmd, throttleCmd, trimCmd;

// Set this value for the default channel
// A = 0
// B = 1
// C = 2
byte channel = 0;


/*
 * Setup function that initializes the serial port and
 * sets some default values for the control variables.
 * Also sets up the pins we'll be using.
 */
void setup()
{
 Serial.begin(9600);
 pinMode(STATUS,OUTPUT);
 digitalWrite(STATUS,LOW);

 pinMode(LED,OUTPUT);
 digitalWrite(LED,LOW);

 yawCmd = 8;
 pitchCmd = 8;
 trimCmd = 0;
 throttleCmd = 0;

 Serial.println("throttle = 0, standing by for commands.");
}


/*
 * Function that does the actual work of converting commands into
 * IR LED pulses and changes the pins in the appropriate manner.
 */
byte sendPacket(byte yaw, byte pitch, byte throttle, byte trim)
{


 int packetData[100];
 int pulseNum;


 digitalWrite(STATUS,HIGH);


 float channelDelayValue = 136500;

 // channel A B or C
 // A is 10 with 136500us packet delay
 // B is 01 with 105200us packet delay
 // C is 11 with 168700us packet delay
 if (channel == 0)
 {
   packetData[0] = 1;
   packetData[1] = 0;
   channelDelayValue = 136500;

 } else if (channel == 1)
 {
   packetData[0] = 0;
   packetData[1] = 1;
   channelDelayValue = 105200;

 } else {
   packetData[0] = 1;
   packetData[1] = 1;
   channelDelayValue = 168700;

 }
 packetData[2] = 0;
 packetData[3] = 0;

 // pitch
 
 packetData[7] = (pitch & 0b1000) >> 3; // direction bit

 if (pitch < 8)
 {
   pitch = 8 - pitch;
 }
 packetData[6] = (pitch & 0b0100) >> 2; // others are speed bits, note that they are reversed
 packetData[5] = (pitch & 0b0010) >> 1;
 packetData[4] = (pitch & 0b0001);

 // throttle
 // bits are reversed in the throttle command
 packetData[15] =  (throttle & 0b10000000) >> 7;
 packetData[14] =  (throttle & 0b01000000) >> 6;
 packetData[13] = (throttle & 0b00100000) >> 5;
 packetData[12] = (throttle & 0b00010000) >> 4;

 packetData[11] = (throttle & 0b00001000) >> 3;
 packetData[10] = (throttle & 0b00000100) >> 2;
 packetData[9] = (throttle & 0b00000010) >> 1;
 packetData[8] = (throttle & 0b00000001);

 // yaw
 packetData[19] = (yaw & 0b1000) >> 3; // direction bit
 if (yaw < 8)
 {
   yaw = 8 - yaw;
 }
 packetData[18] = (yaw & 0b0100) >> 2;
 packetData[17] = (yaw & 0b0010) >> 1;
 packetData[16] = (yaw & 0b0001);

 // these 4 bits are the checksum, so make sure they
 // are 0s so they don't change the XOR later on
 packetData[20] = 0;
 packetData[21] = 0;
 packetData[22] = 0;
 packetData[23] = 0;

 // yaw trim / yaw adjust (the little dial on the controller)
 // 6 bits
 packetData[24] = 0;
 packetData[25] = 0;
 packetData[26] = 0;
 packetData[27] = 0;

 packetData[28] = 0;
 packetData[29] = 0;

 // these bits are never sent but we do the checksum
 // computation in 4-bit chunks, with the trailing two
 // bits set to zero, so we set them to zero here to make
 // the checksum a bit easier to compute
 packetData[30] = 0;
 packetData[31] = 0;

 int i;

 int checksum[10];
 checksum[0] = 0;
 checksum[1] = 0;
 checksum[2] = 0;
 checksum[3] = 0;

 // compute checksum -- bitwise XOR of 4-bit chunks
 // with two zeros padding the *end* of the last two bits
 for (i=0; i<32; i+=4)
 {
   // XOR
   checksum[0] = packetData[i + 0] ^ checksum[0]; // the "^" operator is bitwise XOR (exclusive OR)
   checksum[1] = packetData[i + 1] ^ checksum[1];
   checksum[2] = packetData[i + 2] ^ checksum[2];
   checksum[3] = packetData[i + 3] ^ checksum[3];
 }

 // now set bits 21-24 (array values 20-23) to the checksum
 packetData[20] = checksum[0];
 packetData[21] = checksum[1];
 packetData[22] = checksum[2];
 packetData[23] = checksum[3];

 /*
  * Uncomment for realtime display of packet data
  */
  /*
 for (i=0; i<30; i++)
 {
   Serial.print(packetData[i]);
   if ((i+1)%4 == 0)
   {
     Serial.print(" ");
   }
 }
 Serial.println(" ");
 */
 
 // Send the packet by flashing the LEDs.  Also remember how long the packet takes to send
 // so we can properly compute how long to wait before sending the next packet.
 int bitsum = 0;

 for (i=0; i<30; i++)
 {
   // a "0" bit is 16 pulses and a "1" bit is 32 pulses
   if (packetData[i] == 1)
   {
     bitsum ++;
     pulseNum = 32;
   } else {
     pulseNum = 16;
   }

   // flash pulseNum times
   // a "0" bit is 16 pulses and a "1" bit is 32 pulses
   while(pulseNum--)
   {
     digitalWrite(LED,LOW);
     delayMicroseconds(9);
     digitalWrite(LED,HIGH);
     delayMicroseconds(8);
   }
    
   // there is a 300 microsecond delay between pulses of the LED
   delayMicroseconds(300);
 }

 // channel A B or C
 // A is 10 with 136500us packet delay
 // B is 01 with 105200us packet delay
 // C is 11 with 168700us packet delay
 //
 // that is the delay between sending 30 bit packets
 // note that this does not change if our packets are longer
 // or shorter, so we must take that into account

 return((channelDelayValue - bitsum * 272)/1000); // in ms.

}


void HoldCommand(int yawIn, int pitchIn, int throttleIn, int delayTime)
{
  Serial.print("Holding: Yaw:");
  Serial.print(yawIn);
  Serial.print(" Pitch: ");
  Serial.print(pitchIn);
  Serial.print(" Throttle: ");
  Serial.print(throttleIn);
  Serial.print(" for ");
  Serial.print(delayTime);
  Serial.println("ms");
  
  int i;
  int delayConst = 50;


  int delayAmount = delayTime/delayConst;
  int packetDelay;

  while (delayTime > 0)
  {
    if (Serial.available() == true)
    {
      Serial.println("HOLD ABORTED");
      break;
    }


    packetDelay = sendPacket(yawIn, pitchIn, throttleIn, trimCmd);
    delayTime = delayTime - packetDelay;

    delay(packetDelay);

    delay(delayAmount);
    delayTime = delayTime - delayAmount;
  }
  Serial.println("Done holding.");
}

void Land()
{
 static int i;
 Serial.println("Landing");
 for(i=throttleCmd;i>0;i--){
   HoldCommand(8,8,throttleCmd,50);
 }  
 throttleCmd = 0;
}




/*
 * Function that manages receiving data from the serial port.
 * Mostly changes the global variables that are passed to the
 * control functions.
 */
void serialEvent()
{
 char cmd = Serial.read();
 Serial.println();
 Serial.print("command received is ");
 Serial.println(cmd);

 switch (cmd)
 {
   // Take off with 't'
   case 't':
     Serial.println("Taking Off");

     // Yaw: 1-15
     //    8 = no turn
     //    1 = max right turn
     //    15 = max left turn
     //
     // Pitch: 1-15
     //    8 = no pitch
     //    15 = max forward
     //    1 = max backwards
     //
     // Throttle: 0-255
     //    0 = off
     //    ~130 = steady flight
     //    ~240 = fast climb
     
     // First, go up with lots of throttle for 650ms
     // yaw: 8 --> no yaw
     // pitch: 8 --> no pitch
     // throttle: 240 --> fast climb
     // delay: 650ms --> enough time to climb, not too long so won't hit ceiling
     
     // HoldCommand: a function that sends the same data for a given amount of time
     // HoldCommand(yaw, pitch, throttle, time-to-hold-in-ms);
     HoldCommand(8, 8, 240, 650);
     
     
     // set the *global* throttle to steady flight throttle
     throttleCmd = 130;
     break;

   // land with 'x' or 'q'
   case 'x':
   case 'q':
     Land();
     break;

   // throttle commands
   case '0':
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
     throttleCmd = atoi(&cmd) * 25;  //single character, so we can go from 0 to 255 by inputting 0 to 9 in the serial monitor
     break;

   // turn left
   case 'a':
     if (yawCmd < 15)
     {
       yawCmd ++;
     }
     Serial.print("Yaw is ");
     Serial.println(yawCmd);
     break;

   // turn right
   case 'd':
     if (yawCmd > 1)
     {
       yawCmd --;
     }
     Serial.print("Yaw is ");
     Serial.println(yawCmd);
     break;

   // move forwards
   case 'w':
     if (pitchCmd < 15){
       pitchCmd ++;  // moves forward
     }
     Serial.print("Pitch is ");
     Serial.println(pitchCmd);
     break;
 
   // move backwards
   case 's':
     if (pitchCmd > 1)
     {
       pitchCmd --;  // moves backward
     }
     Serial.print("Pitch is ");
     Serial.println(pitchCmd);
     break;

   // increase throttle
   case 'u':
     if (throttleCmd < 255 - 6)
     {
       throttleCmd += 6;
     }
     Serial.print("Throttle is ");
     Serial.println(throttleCmd);
     break;
   
   // decrease throttle
   case 'j':
     if (throttleCmd > 6)
     {
       throttleCmd -= 6;
     }
     Serial.print("Trottle is ");
     Serial.println(throttleCmd);
     break;

   // change channel
   case 'c':
     Serial.println("Changing channel");
     if (channel >= 2)
     {
       channel = 0;
     } else
     {
       channel ++;
     }
     Serial.print("Channel is: ");
     Serial.println(channel);
     break;

   // reset yaw and pitch
   case 'r':
     Serial.println("resetting yaw and pitch");
     yawCmd = 8;
     pitchCmd = 8;
     break;


   default:
     Serial.println("Unknown command");
 }
 Serial.print("Throttle is at ");
 Serial.println(throttleCmd);
}

/*
 * Loops continuously sending and delaying for the transmission
 */
void loop()

{
   // Note that serialEvent() gets called on each path of the loop
   // and runs if there is data at the serial port

   // we call delay here on the return value of sendPacket because that will
   // cause us to put the right amount of time between packets.  The delay is
   // not constant, but is instead based on how long the packet was
   // that we sent
   delay(sendPacket(yawCmd, pitchCmd, throttleCmd, trimCmd));
}
