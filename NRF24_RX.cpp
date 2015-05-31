
#include "Arduino.h"
#include "config.h"
#include "def.h"
#include "types.h"
#include "MultiWii.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "NRF24_RX.h"

#if defined(NRF24_RX)

#define NRF24_RX_ADDRESS "001RX"
#define NRF24_TX_ADDRESS "001TX"

#define NRF24_PIN_CE  7
#define NRF24_PIN_CSN 8

int16_t nrf24_rcData[RC_CHANS];

byte pipe_tx[6] = NRF24_TX_ADDRESS;
byte pipe_rx[6] = NRF24_RX_ADDRESS;

RF24 radio(NRF24_PIN_CE, NRF24_PIN_CSN);

RF24Data nrf24Data;
RF24AckPayload nrf24AckPayload;
extern RF24AckPayload nrf24AckPayload;

void resetRF24Data()
{
  nrf24Data.throttle = 0;
  nrf24Data.yaw = 1500;
  nrf24Data.pitch = 1500;
  nrf24Data.roll = 1500;
  nrf24Data.switches = 0;
}

void resetRF24AckPayload()
{
  nrf24AckPayload.key = 0;
  nrf24AckPayload.val = analog.vbat;
}

void NRF24_Init() {

  memset(&nrf24_rcData, 0, sizeof(nrf24_rcData));

  resetRF24Data();
  resetRF24AckPayload();

  radio.begin();
  radio.setChannel(80);
  radio.setPayloadSize(sizeof(nrf24Data));
  
  // For the slower data rate (further range) see https://github.com/TMRh20/RF24/issues/98
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(1);
  radio.enableAckPayload();
  
  // How long to wait between each retry, in multiples of 250us,
  // max is 15.  0 means 250us, 15 means 4000us.
  // count How many retries before giving up, max 15
  // 3 * 250 = 750 = time required for an 8 byte ack
  radio.setRetries(3, 1);

  radio.openWritingPipe(pipe_rx);        
  radio.openReadingPipe(1, pipe_tx);
  
  radio.startListening();
}

void NRF24_Read_RC() {
  static unsigned long lastRecvTime = 0;

  //nrf24AckPayload.key = 0;
  //nrf24AckPayload.val = analog.vbat; // vcc

  /*
  nrf24AckPayload.heading = att.heading;
  nrf24AckPayload.pitch = att.angle[PITCH];
  nrf24AckPayload.roll = att.angle[ROLL];
  nrf24AckPayload.alt = alt.EstAlt;
  memcpy(&nrf24AckPayload.flags, &f, 1); // first byte of status flags
  */

  unsigned long now = millis();
  while ( radio.available() ) {
    radio.read(&nrf24Data, sizeof(RF24Data));
    lastRecvTime = now;
    
    if (++nrf24AckPayload.key > 4) {
      nrf24AckPayload.key = 0; 
    }

    switch (nrf24AckPayload.key) {
      case 0:
        nrf24AckPayload.val = analog.vbat; // vcc 
        break;
      case 1:
        nrf24AckPayload.val = att.heading; 
        break;
      case 2:
        nrf24AckPayload.val = att.angle[PITCH]; 
        break;
      case 3:
        nrf24AckPayload.val = att.angle[ROLL]; 
        break;
      case 4:
        //nrf24AckPayload.val = analog.vbat;
        memcpy(&nrf24AckPayload.val, &f, 1); // first byte of status flags 
        break;
    }  

    radio.writeAckPayload(1, &nrf24AckPayload, sizeof(RF24AckPayload) ); 
  }
  if ( now - lastRecvTime > 1000 ) {
    // signal lost?
    resetRF24Data();
  }

  nrf24_rcData[THROTTLE] = nrf24Data.throttle;
  nrf24_rcData[YAW] =      nrf24Data.yaw;
  nrf24_rcData[PITCH] =    nrf24Data.pitch;
  nrf24_rcData[ROLL] =     nrf24Data.roll;
  
  //debug[0]= nrf24_rcData[THROTTLE];
  //debug[1]= nrf24_rcData[YAW];
}

#endif

