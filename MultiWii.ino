
/*
 * Welcome to MultiWii.
 *
 * If you see this message, chances are you are using the Arduino IDE. That is ok.
 * To get the MultiWii program configured for your copter, you must switch to the tab named 'config.h'.
 * Maybe that tab is not visible in the list at the top, then you must use the drop down list at the right
 * to access that tab. In that tab you must enable your baord or sensors and optionally various features.
 * For more info go to http://www.multiwii.com/wiki/index.php?title=Main_Page
 *
 * Have fun, and do not forget MultiWii is made possible and brought to you under the GPL License.
 *
 */
 
/*
 *
 * PIN USAGE:
 * 
 * 4  - Lights
 * 
 * 3  - motor[3]
 * 5  - motor[1]
 * 6  - motor[2]
 * 9  - motor[0]
 * 
 * 7  - NRF CE (3)
 * 8  - NRF CSN (4)
 * 11 - NRF MOSI (6)
 * 12 - NRF MISO (7)
 * 13 - NRF SCK (5)
 * 
 * A4 - SDA -> accelerometer
 * A5 - SCL -> accelerometer
 * 
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
