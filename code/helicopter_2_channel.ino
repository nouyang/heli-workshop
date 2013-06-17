
// kodek.pde - ver 1.0 - S107G IR packet transmitter

//

// Connect anode (+) of IR LED to 5V and connect

// cathode (-) to pin 8 using a 100 ohm resistor



#define LED 8

#define STATUS 13

#define TAKEOFF_THROTTLE 110

#define HOLDING_THROTTLE 58

bool loopOK;
byte yawCmd, pitchCmd, throttleCmd, trimCmd, channel = 0;
int counter;



void setup()
{
  Serial.begin(9600);
  pinMode(STATUS,OUTPUT);
  digitalWrite(STATUS,LOW);

  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);

  yawCmd = 63;
  pitchCmd = 63;
  trimCmd = 0;
  throttleCmd = 0;

  Serial.println("throttle = 0, standing by for commands.");
}



byte sendPacket(byte yaw,byte pitch,byte throttle,byte trim)

{

  static byte markL,countP,countR,one,zero, flag;

  static bool data;

  static const byte maskB[] = {1,2,4,8,16,32,64,128};

  static byte dataP[4];



  digitalWrite(STATUS,HIGH);

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

    if((dataP[4-countP] & maskB[--countR]) || (flag == 8 && countP ==
2 && channel == 1))
    {

      one++;
      //Serial.print("1");
      delayMicroseconds(688);

    }

    else

    {
      //Serial.print("0");
      zero++;

      delayMicroseconds(288);

    }

    if(!countR)

    {

      countR = 8;

      countP--;
      //Serial.println("... next command");

    }

    if(!countP)

    {

      data = false;

    }

  }

  while(markL--)

  {

    digitalWrite(LED,LOW);

    delayMicroseconds(10);

    digitalWrite(LED,HIGH);

    delayMicroseconds(10);

  }

  digitalWrite(STATUS,LOW);

  return((.1-.014296-one*.000688-zero*.000288)*1000); // in ms.

}

void SmallBox()
{
  int pitchTime = 1500;
  int yawTime = 1500;

  int pitchAmount = -30;

  int yawAmount = 25;

  int throttleForYaw = 0;

  HoldCommand(yawCmd, pitchCmd + pitchAmount, throttleCmd, pitchTime);
  HoldCommand(yawCmd + yawAmount, pitchCmd, throttleCmd +
throttleForYaw, yawTime);
  HoldCommand(yawCmd, pitchCmd + pitchAmount, throttleCmd, pitchTime);
  HoldCommand(yawCmd + yawAmount, pitchCmd, throttleCmd +
throttleForYaw, yawTime);
  HoldCommand(yawCmd, pitchCmd + pitchAmount, throttleCmd, pitchTime);
  HoldCommand(yawCmd + yawAmount, pitchCmd, throttleCmd +
throttleForYaw, yawTime);
  HoldCommand(yawCmd, pitchCmd + pitchAmount, throttleCmd, pitchTime);
  HoldCommand(yawCmd + yawAmount, pitchCmd, throttleCmd +
throttleForYaw, yawTime);
}

void HoldCommand(int yawIn, int pitchIn, int throttleIn, int delayTime)
{
   Serial.println("Holding throttle");
   int i;
   int delayConst = 50;

   //yawCmd = 63;
   //pitchCmd = 63;
   //trimCmd = 0;


   int delayAmount = delayTime/delayConst;
   int packetDelay;

   while (delayTime > 0)
   {
     if (Serial.available() == true)
     {
       Serial.println("SEQUENCE ABORTED");
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

void TestCopter()
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

void TakeOff()
{
  static int i;
  Serial.println("Taking Off");
  yawCmd = 63;
  pitchCmd = 63;
  trimCmd = 0;
  throttleCmd = TAKEOFF_THROTTLE;
  /*
  for (i=0; i < 10; i++)
 {
  Serial.print(i);
  delay(sendPacket(yawCmd,pitchCmd,throttleCmd,trimCmd));
}
  Serial.println();
throttleCmd /= 2;
  for (i=0; i < 10; i++)
 {
  Serial.print(i);
  delay(sendPacket(yawCmd,pitchCmd,throttleCmd,trimCmd));
}
  Serial.println(); */
  HoldCommand(yawCmd, pitchCmd, throttleCmd, 650);
  throttleCmd = HOLDING_THROTTLE;
}

void Land()
{
  static int i;
  Serial.println("Landing");
  for(i=throttleCmd;i>0;i--) delay(sendPacket(63,63,i,trimCmd));
  yawCmd = 63;
  pitchCmd = 63;
  trimCmd = 0;
  throttleCmd = 0;
}

void serialEvent()
{
  char cmd = Serial.read();
  Serial.println();
  Serial.print("command received is ");
  Serial.println(cmd);

  switch (cmd)
  {
    case 'x':
    case 'q':
      Land();
      break;

    case 't':
      TakeOff();
      break;

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
      throttleCmd = atoi(&cmd) * 14;  //single character, so we can go from 0 to 124 by inputting 0 to 9 in the serial monitor
      break;

    case 'a':
      yawCmd += 5;
      Serial.print("Yaw is ");
      Serial.println(yawCmd);
      break;

    case 'd':
      yawCmd -= 5;
      Serial.print("Yaw is ");
      Serial.println(yawCmd);
      break;

    case 'w':
      pitchCmd -= 5;  // moves forward
      Serial.print("Pitch is ");
      Serial.println(pitchCmd);
      break;

    case 's':
      pitchCmd += 5;  // moves backward
      Serial.print("Pitch is ");
      Serial.println(pitchCmd);
      break;

    case 'u':
      if (throttleCmd < 255 - 3)
      {
        throttleCmd += 3;
      }
      Serial.print("Throttle is ");
      Serial.println(throttleCmd);
      break;

    case 'j':
      if (throttleCmd > 3)
      {
        throttleCmd -= 3;
      }
      Serial.print("Trottle is ");
      Serial.println(throttleCmd);
      break;

    case 'b':
      Serial.println("Running small box");
      SmallBox();
      break;

    case 'c':
      Serial.println("Changing channel");
      if (channel == 1)
      {
        channel = 0;
      } else
      {
        channel = 1;
      }
      break;

    case 'r':
      Serial.println("resetting yaw and pitch");
      yawCmd = 63;
      pitchCmd = 63;
      break;
      
   case 'v':
      Serial.println("running test...");
      TestCopter();
      break;

    default:
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

