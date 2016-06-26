#include "Isis.h"

#include <Wire.h>

extern "C" {
#include <string.h>
#include <stdlib.h>
}

//******************************************************************************
//* Constructors
//******************************************************************************

/**
 * The Isis class.
 * An instance named "Isis" is created automatically for the user.
 */
IsisClass::IsisClass()
{
  firmwareVersionCount = 0;
  firmwareVersionVector = 0;
 // systemReset();

    // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
}


//******************************************************************************
//* Public Methods
//******************************************************************************

/**
 * Initialize the default Serial transport at the default baud of 57600.
 */
void IsisClass::begin(void)
{
  begin(57600);
}

/**
 * Initialize the default Serial transport and override the default baud.
 * Sends the protocol version to the host application followed by the firmware version and name.
 * blinkVersion is also called. To skip the call to blinkVersion, call Firmata.disableBlinkVersion()
 * before calling Firmata.begin(baud).
 * @param speed The baud to use. 57600 baud is the default value.
 */
void IsisClass::begin(long speed)
{
  Serial.begin(speed);
  IsisStream = &Serial;
  //blinkVersion();
  //printVersion();         // send the protocol version
  //printFirmwareVersion(); // send the firmware name and version
}

/**
 * Reassign the Firmata stream transport.
 * @param s A reference to the Stream transport object. This can be any type of
 * transport that implements the Stream interface. Some examples include Ethernet, WiFi
 * and other UARTs on the board (Serial1, Serial2, etc).
 */
void IsisClass::begin(Stream &s)
{
  FirmataStream = &s;
  // do not call blinkVersion() here because some hardware such as the
  // Ethernet shield use pin 13
  //printVersion();
  //printFirmwareVersion();
}

/**
 * Send the Firmata protocol version to the Firmata host application.
 */
void IsisClass::printVersion(void)
{
  //FirmataStream->write(REPORT_VERSION);
  //FirmataStream->write(FIRMATA_PROTOCOL_MAJOR_VERSION);
  //FirmataStream->write(FIRMATA_PROTOCOL_MINOR_VERSION);
}

/**
 * Blink the Firmata protocol version to the onboard LEDs (if the board has an onboard LED).
 * If VERSION_BLINK_PIN is not defined in Boards.h for a particular board, then this method
 * does nothing.
 * The first series of flashes indicates the firmware major version (2 flashes = 2).
 * The second series of flashes indicates the firmware minor version (5 flashes = 5).
 */
void IsisClass::blinkVersion(void)
{
/*#if defined(VERSION_BLINK_PIN)
  if (blinkVersionDisabled) return;
  // flash the pin with the protocol version
  pinMode(VERSION_BLINK_PIN, OUTPUT);
  strobeBlinkPin(VERSION_BLINK_PIN, FIRMATA_FIRMWARE_MAJOR_VERSION, 40, 210);
  delay(250);
  strobeBlinkPin(VERSION_BLINK_PIN, FIRMATA_FIRMWARE_MINOR_VERSION, 40, 210);
  delay(125);
#endif*/
}

/**
 * Sets the name and version of the firmware. This is not the same version as the Firmata protocol
 * (although at times the firmware version and protocol version may be the same number).
 * @param name A pointer to the name char array
 * @param major The major version number
 * @param minor The minor version number
 */
void IsisClass::setFirmwareNameAndVersion(const char *name, byte major, byte minor)
{
  const char *firmwareName;
  const char *extension;

  // parse out ".cpp" and "applet/" that comes from using __FILE__
  extension = strstr(name, ".cpp");
  firmwareName = strrchr(name, '/');

  if (!firmwareName) {
    // windows
    firmwareName = strrchr(name, '\\');
  }
  if (!firmwareName) {
    // user passed firmware name
    firmwareName = name;
  } else {
    firmwareName ++;
  }

  if (!extension) {
    firmwareVersionCount = strlen(firmwareName) + 2;
  } else {
    firmwareVersionCount = extension - firmwareName + 2;
  }

  // in case anyone calls setFirmwareNameAndVersion more than once
  free(firmwareVersionVector);

  firmwareVersionVector = (byte *) malloc(firmwareVersionCount + 1);
  firmwareVersionVector[firmwareVersionCount] = 0;
  firmwareVersionVector[0] = major;
  firmwareVersionVector[1] = minor;
  strncpy((char *)firmwareVersionVector + 2, firmwareName, firmwareVersionCount - 2);
}



// ============================== //
// callback for received data
// ============================== //
void IsisClass::receiveData(int p_ByteCount) {
  int l_BufIndex = 0;

  while (Wire.available()) {
    g_pInputMsgBuf[l_BufIndex++] = Wire.read();
  }

  // Decodage du message
  decodeMessage(g_pInputMsgBuf, p_ByteCount);
}


// ============================== //
// callback for sending data
// ============================== //
void IsisClass::sendData() {
  if (g_OutputMsgSize)
    Wire.write(g_pOutputMsgBuf, g_OutputMsgSize);
  g_OutputMsgSize = 0;
}


void IsisClass::decodeMessage(byte* p_pMsgBuffer,
                   int p_BufferSize)
{
  switch (p_pMsgBuffer[0])
  {
    case DEFINE_ENCODER_MSG_ID:
      {
#ifdef _DEBUG_
        Serial.println(" => DefineEncoderMsg");
#endif
        defineEncoderMsg_received(p_pMsgBuffer, p_BufferSize);
        break;
      }
    case GET_ENCODER_COUNTER_MSG_ID:
      {
#ifdef _DEBUG_
        Serial.println(" => GetEncoderCounterMsg");
#endif
        getEncoderCounterMsg_received(p_pMsgBuffer, p_BufferSize);
        break;
      }
    case PIN_MODE_MSG_ID:
      {
#ifdef _DEBUG_
        Serial.println(" => PinModerMsg");
#endif
        pinModeMsg_received(p_pMsgBuffer, p_BufferSize);
        break;
      }
    case DIGITAL_WRITE_MSG_ID:
      {
#ifdef _DEBUG_
        Serial.println(" => DigitalWriteMsg");
#endif
        digitalWriteMsg_received(p_pMsgBuffer, p_BufferSize);
        break;
      }
    case DIGITAL_WRITES_MSG_ID:
      {
#ifdef _DEBUG_
        Serial.println(" => DigitalWritesMsg");
#endif
        digitalWritesMsg_received(p_pMsgBuffer, p_BufferSize);
        break;
      }
    case ANALOG_WRITE_MSG_ID:
      {
#ifdef _DEBUG_
        Serial.println(" => AnalogWriteMsg");
#endif
        analogWriteMsg_received(p_pMsgBuffer, p_BufferSize);
        break;
      }
    case GET_IDL_VERSION_MSG_ID:
      {
#ifdef _DEBUG_
        Serial.println(" => GetIdlVersionMsg");
#endif
        getIdlVersionMsg_received();
        break;
      }
    case GET_SOFT_VERSION_MSG_ID:
      {
#ifdef _DEBUG_
        Serial.println(" => GetSoftVersionMsg");
#endif
        getSoftVersionMsg_received();
        break;
      }
    case GET_STATUS_MSG_ID:
      {
#ifdef _DEBUG_
        Serial.println(" => GetStatusMsg");
#endif
        getStatusMsg_received();
        break;
      }
    case GET_TYPE_ARDUINO_MSG_ID:
      {
#ifdef _DEBUG_
        Serial.println(" => GetArduinoTypeMsg");
#endif
        getTypeArduinoMsg_received();
        break;
      }
    case DIGITAL_READ_MSG_ID:
      {
#ifdef _DEBUG_
        Serial.println(" => digitaleReadMsg_received");
#endif
        digitaleReadMsg_received(p_pMsgBuffer, p_BufferSize);
        break;
      }
    case ANALOG_READ_MSG_ID:
      {
#ifdef _DEBUG_
        Serial.println(" => analogReadMsg_received");
#endif
        analogReadMsg_received(p_pMsgBuffer, p_BufferSize);
        break;
      }
    default :
      {
        Serial.println("   .. UNKNOWN ID !");
        break;
      }
  }

}

void IsisClass::encoderItA(EncoderStruct* p_pEncoder)
{
  p_pEncoder->prevCptA = p_pEncoder->cptA;
  p_pEncoder->cptA = digitalRead(p_pEncoder->pinA);
  if (p_pEncoder->cptA == p_pEncoder->prevCptB) {
    p_pEncoder->tick_counter--;
  }
  else p_pEncoder->tick_counter++;
}

void IsisClass::encoderItB(EncoderStruct* p_pEncoder)
{
  p_pEncoder->prevCptB = p_pEncoder->cptB;
  p_pEncoder->cptB = digitalRead(p_pEncoder->pinB);
  if (p_pEncoder->prevCptA == p_pEncoder->cptB) {
    p_pEncoder->tick_counter++;
  }
  else p_pEncoder->tick_counter--;
}


void IsisClass::interruptA_0()
{
  encoderItA(&(g_pEncoderTab[0]));
}

void IsisClass::interruptB_0()
{
  encoderItB(&(g_pEncoderTab[0]));
}
void IsisClass::interruptA_1()
{
  encoderItA(&(g_pEncoderTab[1]));
}

void IsisClass::interruptB_1()
{
  encoderItB(&(g_pEncoderTab[1]));
}

void IsisClass::display_encoder_info(int p_Id)
{
  Serial.print("g_pEncoderTab["); Serial.print(p_Id); Serial.println("]");
  Serial.print("\tId\t:"); Serial.println((g_pEncoderTab[p_Id]).Id);
  Serial.print("\tpinA\t:"); Serial.print((g_pEncoderTab[p_Id]).pinA);  Serial.print("\tpinB\t:"); Serial.println((g_pEncoderTab[p_Id]).pinB);
  Serial.print("\tcounter\t:"); Serial.println((g_pEncoderTab[p_Id]).tick_counter);
}
void IsisClass::display_encoder_info_light(int p_Id)
{
  Serial.print("Enc_"); Serial.print(p_Id); Serial.print(" ");
  Serial.print((g_pEncoderTab[p_Id]).pinA);  Serial.print("/"); Serial.print((g_pEncoderTab[p_Id]).pinB);
  Serial.print(" : "); Serial.println((g_pEncoderTab[p_Id]).tick_counter);
}

void IsisClass::display_all_Encoder()
{
  for (int i = 0;  i < NB_ENCODER_MAX; i++)
  {
    if (g_pEncoderTab[i].Id != -1)    display_encoder_info(g_pEncoderTab[i].Id);
  }
}
void IsisClass::display_all_Encoder_info_light()
{
  for (int i = 0;  i < NB_ENCODER_MAX; i++)
  {
    if (g_pEncoderTab[i].Id != -1)    display_encoder_info_light(g_pEncoderTab[i].Id);
  }
  Serial.println();
}



void IsisClass::defineEncoderMsg_received(byte* p_pData, int p_MsgSize)
{
  int l_EncoderId = p_pData[1];

  g_pEncoderTab[l_EncoderId].Id = l_EncoderId ;
  g_pEncoderTab[l_EncoderId].pinA = p_pData[2];
  g_pEncoderTab[l_EncoderId].pinB = p_pData[3];
  g_pEncoderTab[l_EncoderId].tick_counter = 0;
  g_pEncoderTab[l_EncoderId].cptA = g_pEncoderTab[l_EncoderId].prevCptA = 0;
  g_pEncoderTab[l_EncoderId].cptB = g_pEncoderTab[l_EncoderId].prevCptB = 0;
  display_all_Encoder();

  pinMode(p_pData[2], INPUT_PULLUP);
  pinMode(p_pData[3], INPUT_PULLUP);

#ifdef _USE_PCINPORT_
  PCintPort::attachInterrupt(p_pData[2], g_pEncoderTab[l_EncoderId].ptr_InterruptA, CHANGE);
  PCintPort::attachInterrupt(p_pData[3], g_pEncoderTab[l_EncoderId].ptr_InterruptB, CHANGE);
  Serial.print("PCintPort::attachInterrupt "); Serial.println(p_pData[2]);
  Serial.print("PCintPort::attachInterrupt "); Serial.println(p_pData[3]);
#else
  attachInterrupt(digitalPinToInterrupt(p_pData[2]), g_pEncoderTab[l_EncoderId].ptr_InterruptA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(p_pData[3]), g_pEncoderTab[l_EncoderId].ptr_InterruptB, CHANGE);
  Serial.print("attachInterrupt "); Serial.println(p_pData[2]);
  Serial.print("attachInterrupt "); Serial.println(p_pData[3]);
#endif

}


void IsisClass::pinModeMsg_received(byte* p_pData, int p_MsgSize)
{
  int l_Pin = p_pData[1] >> 2;
  byte l_Mode = p_pData[1] & 0x3;

  switch ( l_Mode)
  {
    case INPUT_PIN_MODE:
      l_Mode = INPUT;
      break;
    case OUTPUT_PIN_MODE:
      l_Mode = OUTPUT;
      break;
    case PULLUP_PIN_MODE:
      l_Mode = INPUT_PULLUP;
      break;
  }

  pinMode(l_Pin, l_Mode);
}


void IsisClass::digitalWriteMsg_received(byte* p_pData, int p_MsgSize)
{
  int l_Pin = p_pData[1] >> 2;
  byte l_Value = p_pData[1] & 0x1;

  if (l_Value == DIGITAL_LOW)
    l_Value = LOW;
  else
    l_Value = HIGH;

  digitalWrite(l_Pin, l_Value);
}


void IsisClass::digitalWritesMsg_received(byte* p_pData, int p_MsgSize)
{
  int l_Pin1 = p_pData[1] >> 2;
  int l_Pin2 = p_pData[2] >> 2;
  byte l_Value1 = p_pData[1] & 0x1;
  byte l_Value2 = p_pData[2] & 0x1;

  if (l_Value1 == DIGITAL_LOW)
    l_Value1 = LOW;
  else
    l_Value1 = HIGH;

  digitalWrite(l_Pin1, l_Value1);

  if (l_Value2 == DIGITAL_LOW)
    l_Value2 = LOW;
  else
    l_Value2 = HIGH;

  digitalWrite(l_Pin2, l_Value2);
}


void IsisClass::analogWriteMsg_received(byte* p_pData, int p_MsgSize)
{
  int l_Pin = p_pData[1];
  byte l_Value = p_pData[2];

  analogWrite(l_Pin, l_Value);
}

void IsisClass::getEncoderCounterMsg_received(byte* p_pData, int p_MsgSize)
{
  // il faut determiner s'il s'agit de la requete ou de la demande de lecture
  if (p_MsgSize == 2)
  {
    // il s'agit de la requete
    g_EncoderCounterIdRequest = p_pData[1];
  }
  else if (p_MsgSize == 1)
  {
    // il s'agit de la demande de recuperation des datas
    byte l_pBuffer[2];
    int l_Counter = g_pEncoderTab[g_EncoderCounterIdRequest].tick_counter;
    l_pBuffer[0] = ENCODER_COUNTER_MSG_ID;
    l_pBuffer[1] = ((g_EncoderCounterIdRequest << 5)  );
    if (l_Counter < 0)
    {
      l_pBuffer[1] |= 0x10;
      l_Counter *= -1;
    }
    l_pBuffer[1] |= ((l_Counter >> 8) & 0x0F);
    l_pBuffer[2] = l_Counter & 0xFF;

    prepareMsg2Send(l_pBuffer, 3);
  }

}

void IsisClass::digitaleReadMsg_received(byte* p_pData, int p_MsgSize)
{
  // il faut determiner s'il s'agit de la requete ou de la demande de lecture
  if (p_MsgSize == 2)
  {
    // il s'agit de la prepa
    g_DigitalePinReadRequest = p_pData[1];
  }
  else if (p_MsgSize == 1)
  {
    // il s'agit de la demande de recuperation des datas
    byte l_pBuffer[2];
    bool l_Value = digitalRead(g_DigitalePinReadRequest);
    l_pBuffer[0] = DIGITAL_READ_VALUE_MSG_ID;
    l_pBuffer[1] = l_Value | ((g_DigitalePinReadRequest << 2)  );

    prepareMsg2Send(l_pBuffer, 2);
  }
}

void IsisClass::analogReadMsg_received(byte* p_pData, int p_MsgSize)
{
  // il faut determiner s'il s'agit de la requete ou de la demande de lecture
  if (p_MsgSize == 2)
  {
    // il s'agit de la prepa
    g_AnalogPinReadRequest = p_pData[1];
  }
  else if (p_MsgSize == 1)
  {
    // il s'agit de la demande de recuperation des datas
    byte l_pBuffer[3];
    int l_Value = analogRead(g_AnalogPinReadRequest);
    l_pBuffer[0] = ANALOG_READ_VALUE_MSG_ID;
    l_pBuffer[1] = (l_Value >> 8) | ((g_AnalogPinReadRequest << 2)  );
    l_pBuffer[2] = l_Value & 0xFF;

    prepareMsg2Send(l_pBuffer, 3);
  }
}


void IsisClass::prepareMsg2Send(byte* p_pData, int p_MsgSize)
{
  for (int i = 0; i < p_MsgSize; i++)
  {
    g_pOutputMsgBuf[i] = p_pData[i];
  }
  g_OutputMsgSize = p_MsgSize;
}


void IsisClass::getIdlVersionMsg_received()
{
  byte l_pBuffer[2];
  l_pBuffer[0] = IDL_VERSION_MSG_ID;
  l_pBuffer[1] = MAJOR_IDL_VERSION;
  l_pBuffer[1] = ((l_pBuffer[1] << 4) | MINOR_IDL_VERSION);
  prepareMsg2Send(l_pBuffer, 2);
}


void IsisClass::getSoftVersionMsg_received()
{
  byte l_pBuffer[2];
  l_pBuffer[0] = SOFT_VERSION_MSG_ID;
  l_pBuffer[1] = MAJOR_SOFT_VERSION;
  l_pBuffer[1] = ((l_pBuffer[1] << 4) | MINOR_SOFT_VERSION);
  prepareMsg2Send(l_pBuffer, 2);
}

void IsisClass::getStatusMsg_received()
{
  byte l_pBuffer[2];
  l_pBuffer[0] = STATUS_MSG_ID;
  l_pBuffer[1] =  g_Status;
  prepareMsg2Send(l_pBuffer, 2);
}

void IsisClass::getTypeArduinoMsg_received()
{
  byte l_pBuffer[2];
  l_pBuffer[0] = TYPE_ARDUINO_MSG_ID;
  l_pBuffer[1] = TYPE_ARDUINO;
  prepareMsg2Send(l_pBuffer, 2);
}