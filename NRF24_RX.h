
/*
Tested with 16MHz pro-mini, 
Motors use pins 9,6,5,3 instead of 9,10,11,3
nRF24 connections (left is nRF24, right is arduino):q
  CE      7
  CSN     8
  MOSI   11
  MISO   12
  SCK    13
You can change CE and CSN in NRF24_RC.cpp
*/

#ifndef NRF24_RX_H_
#define NRF24_RX_H_

#include "config.h"

#if defined(NRF24_RX)

// The sizeof this struct should not exceed 32 bytes
struct RF24Data {
  int throttle;
  int yaw;
  int pitch;
  int roll;
  byte switches; // bitflag
};

struct RF24AckPayload {
  uint8_t key;
  uint16_t val;
};

extern RF24Data nrf24Data;
extern RF24AckPayload nrf24AckPayload;
extern int16_t nrf24_rcData[RC_CHANS];

void NRF24_Init();
void NRF24_Read_RC();

#endif

#endif /* NRF24_RX_H_ */
