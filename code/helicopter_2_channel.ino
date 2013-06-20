
// kodek.pde - ver 1.0 - S107G IR packet transmitter

#define LED 13 // Pin connected to leds
#define TAKEOFF_THROTTLE 110
#define HOLDING_THROTTLE 58
#define CHANNEL_A 0
#define CHANNEL_B 1
#define DELAY_CONST 50

byte yawCmd, pitchCmd, throttleCmd, trimCmd, channel = CHANNEL_A;

/* Setup runs once, when the Arduino starts */
void setup()
{
  // Start the serial port communications
  Serial.begin(9600);

  // set the IR LED pin to be an output, take it to 0 volts
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);

  // initialize global command variables to be neutral.
  yawCmd = 63; // yaw: 0-127, 63 is no yaw
  pitchCmd = 63; // pitch: 0-127, 63 is no pitch
  trimCmd = 0;
  throttleCmd = 0; // throttle: 0-127, 0 is no throttle
  
  // send the first serial communication
  Serial.println("throttle = 0, standing by for commands.");
}


/*
 * Function to send a packet to the helicopter
 *
 * Inputs: yaw, pitch, throttle, trim
 *
 * Yaw: 0-127, 63 = no yaw
 * Pitch: 0-127, 63 = no pitch
 * Throttle: 0-127, 0 = off
 * Trim: 0-127, 0 = no trim
 *
 * Outputs:
 *  time it took to send the packet
 */
byte sendPacket(byte yaw,byte pitch,byte throttle,byte trim)
{
  static byte markL, countP, countR, one, zero, flag;
  static bool data;
  static const byte maskB[] = {1,2,4,8,16,32,64,128};
  static byte dataP[4];

  dataP[0] = yaw;
  dataP[1] = pitch;
  dataP[2] = throttle;
  dataP[3] = trim;
  
  markL = 77;
  countP = 4;
  countR = 8;
  one = 0;
  zero = 0;
  data = true;
  
  // flash the IR LED 77 times
  while(markL--)
  {
    digitalWrite(LED,LOW);
    delayMicroseconds(10);
    digitalWrite(LED,HIGH);
    delayMicroseconds(10);
  }

  // wait 2ms
  delayMicroseconds(1998);
  markL = 12;
  
  while(data)
  {
    // flash 12 times
    while(markL--)
    {
      digitalWrite(LED,LOW);
      delayMicroseconds(10);
      digitalWrite(LED,HIGH);
      delayMicroseconds(10);
    }
    markL = 12;
    flag = countR;

    if((dataP[4-countP] & maskB[--countR]) || (flag == 8 && countP == 2 && channel == 1)) {
      one++;
      //Serial.print("1");
      delayMicroseconds(688);
    } else {
      //Serial.print("0");
      zero++;

      delayMicroseconds(288);
    }

    if(!countR) {
      countR = 8;
      countP--;
      //Serial.println("... next command");
    }

    if(!countP) {
      data = false;
    }

  }

  while(markL--) {
    digitalWrite(LED,LOW);
    delayMicroseconds(10);
    digitalWrite(LED,HIGH);
    delayMicroseconds(10);
  }
  
  return((.1-.014296-one*.000688-zero*.000288)*1000); // in ms.
}

void HoldCommand(int yawIn, int pitchIn, int throttleIn, int delayTime)
{
   Serial.println("Holding throttle");

   // Initial conditions
   //yawCmd = 63;
   //pitchCmd = 63;
   //trimCmd = 0;

   int delayAmount = delayTime/DELAY_CONST;
   int packetDelay;

   while (delayTime > 0) {
     if (Serial.available() == true) {
       Serial.println("HOLD COMMAND ABORTED");
       break;
     }

     packetDelay = sendPacket(yawIn, pitchIn, throttleIn, trimCmd);
     delayTime = delayTime - packetDelay;
     Serial.print("packet delay:");
     Serial.println(packetDelay);

     delay(packetDelay);

     delay(delayAmount);
     delayTime = delayTime - delayAmount;
   }
   Serial.println("Done holding throttle");
}

void TestCopter() // Small function that tests the helicopters yaw, pitch and throttle at once
{
  yawCmd = 15;
  pitchCmd = 15;
  trimCmd = 0;
  throttleCmd = TAKEOFF_THROTTLE;
  HoldCommand(yawCmd, pitchCmd, throttleCmd, 500);
  
  throttleCmd = 0;
  yawCmd = 63;
  pitchCmd = 63;
}

void serialEvent()  // Called every time a command is recieved on the serial port
{
  char cmd = Serial.read();  // Reads in a command from the serial port
  Serial.println();
  Serial.print("command received is ");
  Serial.println(cmd);

  switch (cmd) {
    case '0':
      Serial.print("Killing throttle.");
      throttleCmd = 0;
      yawCmd = 63;
      pitchCmd = 63;
      break;
      
    case '5': // Attempt at hover throttle?
      Serial.print("Got a throttle command.");
      throttleCmd = 77; // modify this slightly to make it easy to fly
      break;
      
    case '1':
    case '2':
    case '3':
    case '4':
    case '6':
    case '7':
    case '8':
    case '9':
      Serial.print("Got a throttle command.");
      throttleCmd = atoi(&cmd) * 14;  // Single character, so we can go from 0 to 124 by inputting 0 to 9 in the serial monitor
      break;

    case 'a':  // Causes the helicopter to rotate counter-clockwise
      yawCmd += 5;
      Serial.print("Yaw is ");
      Serial.println(yawCmd);
      break;

    case 'd':  // Causes the helicopter to rotate clockwise
      yawCmd -= 5;
      Serial.print("Yaw is ");
      Serial.println(yawCmd);
      break;

    case 'w':  // Causes the helicopter to pitch forward
      pitchCmd -= 5;
      Serial.print("Pitch is ");
      Serial.println(pitchCmd);
      break;

    case 's':  // Causes the helicopter to pitch backwards
      pitchCmd += 5;
      Serial.print("Pitch is ");
      Serial.println(pitchCmd);
      break;

    case 'u':  // Causes the helicopter to inc in throttle
      if (throttleCmd < 255 - 3) {
        throttleCmd += 3;
      }
      Serial.print("Throttle is ");
      Serial.println(throttleCmd);
      break;

    case 'j':  // Causes the helicopter to dec in throttle
      if (throttleCmd > 3) {
        throttleCmd -= 3;
      }
      Serial.print("Trottle is ");
      Serial.println(throttleCmd);
      break;

    case 'c':  // Changes the channel A = 0, B = 1
      Serial.println("Changing channel");
      if (channel == 1) {
        channel = 0;
      } else {
        channel = 1;
      }
      break;

    case 'r':  // Reset
      Serial.println("Resetting yaw and pitch");
      yawCmd = 63;
      pitchCmd = 63;
      break;
      
    case 'b': // Secret box flight function
      HoldCommand(63, 63, 67, 1000); // take off
      HoldCommand(63, 63, 60, 3000);// hover
      HoldCommand(63, 32, 60, 900);//f1
      HoldCommand(40, 63, 60, 1000);//r1
      HoldCommand(63, 32, 60, 900);//f2
      HoldCommand(40, 63, 60, 1000);//r2
      HoldCommand(63, 32, 60, 900);//f3
      HoldCommand(40, 63, 60, 1000);//r3
      HoldCommand(63, 32, 60, 900);//f4
      break;

    default:  // Bad command
      Serial.println("Unknown command");
  }
  
  Serial.print("Throttle is at ");
  Serial.println(throttleCmd);
}

void loop()
{
    // Note that serialEvent() gets called on each path of the loop
    // and runs if there is data at the serial port

    delay(sendPacket(yawCmd,pitchCmd,throttleCmd,trimCmd));
}

