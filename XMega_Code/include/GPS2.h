#ifndef GPS2_H
#define GPS2_H 1

// Our GPS Unit is connected on USARTD1
// GPS TX -> PD6
// GPS RX <- PD7
#define GPSUART USARTD1
#define GPSPORT PORTD

#define FALSE 0
#define TRUE 1
#define minutes (1/60)

typedef unsigned char u08;
typedef unsigned int u16;
typedef unsigned long u32;
typedef signed long s32;

typedef struct
{
/*
	u32 time;					//milliseconds/week
	s32 vnorth;					//cm/s
	s32 veast;
	s32 vdown;
*/
	float longitude;					//degrees/10^-7
	float latitude;
	float altitude;					//height/mm
	float speed;
	float heading;
	u08 packetflag;					//packetflag lets us see when our packet has been updated
	u08 status;					//type of fix
	u08 nosats;					//number of tracked satellites
} gps_type;

void init_gps();
uint8_t gps_xor_checksum(char *string);
void GPSWriteLine(char *string);
void GPSWriteString(char *string);
void GPSWriteChar(unsigned char data);
void sendNMEA(char *string);

#endif