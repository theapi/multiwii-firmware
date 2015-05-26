
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
  nrf24AckPayload.val = 0;
}

void NRF24_Init() {

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
  static byte ack_ready = 0;
  static byte ack_key = 0;
  static unsigned long lastRecvTime = 0;

  nrf24AckPayload.key = 0;
  nrf24AckPayload.val = 0;
  /*
  nrf24AckPayload.heading = att.heading;
  nrf24AckPayload.pitch = att.angle[PITCH];
  nrf24AckPayload.roll = att.angle[ROLL];
  nrf24AckPayload.alt = alt.EstAlt;
  memcpy(&nrf24AckPayload.flags, &f, 1); // first byte of status flags
  */
  
  if (!ack_ready) {
    // Create the ack payload read for the next transmission response
    radio.writeAckPayload(1, &nrf24AckPayload, sizeof(RF24AckPayload) ); 
    ack_ready = 1;
  }

  unsigned long now = millis();
  while ( radio.available() ) {
    radio.read(&nrf24Data, sizeof(RF24Data));
    lastRecvTime = now;
    ack_ready = 0;
    nrf24AckPayload.key = ack_key;
    
    switch (ack_key) {
      case 0:
        nrf24AckPayload.val = 3333; //@todo vcc 
        break;
      case 1:
        nrf24AckPayload.val = nrf24Data.throttle; 
        break;
      case 2:
        nrf24AckPayload.val = nrf24Data.yaw; 
        break;
      case 3:
        nrf24AckPayload.val = nrf24Data.pitch; 
        break;
      case 4:
        nrf24AckPayload.val = nrf24Data.roll; 
        break;
    }  
      
    if (++ack_key > 4) {
      ack_key = 0; 
    }
    
  }
  if ( now - lastRecvTime > 1000 ) {
    // signal lost?
    resetRF24Data();
  }

  nrf24_rcData[THROTTLE] = nrf24Data.throttle;
  nrf24_rcData[YAW] =      nrf24Data.yaw;
  nrf24_rcData[PITCH] =    nrf24Data.pitch;
  nrf24_rcData[ROLL] =     nrf24Data.roll;
}

#endif

