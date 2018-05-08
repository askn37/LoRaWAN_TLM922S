#include <Arduino.h>
#include <avr/pgmspace.h>
#include "LoRaWAN_TLM922S_Table.h"

const PROGMEM uint32_t PS_DICT[] = {
	0x0073016dU, 0x13730180U, 0x00650107U, 0x11210177U, 0x0061018fU, 0x006e0104U, 0x0065010bU, 0x006c0147U,
	0x006e0102U, 0x00410126U, 0x004d011fU, 0x00440126U, 0x0053011dU, 0x006f014aU, 0x00610199U, 0x005f0100U,
	0x00700191U, 0x00650115U, 0x006e010fU, 0x00690157U, 0x006f0128U, 0x00740110U, 0x005f0114U, 0x1768011aU,
	0x00630160U, 0x16740113U, 0x00630161U, 0x0c6401a4U, 0x006f015cU, 0x00320144U, 0x003d01c0U, 0x004c01b8U,
	0x0f660165U, 0x00650126U, 0x00650125U, 0x0077011cU, 0x006e0123U, 0x006b0126U, 0x02200159U, 0x006f0166U,
	0x006e0126U, 0x00620126U, 0x00750129U, 0x00740126U, 0x18790184U, 0x00690171U, 0x006e012dU, 0x006101b9U,
	0x0d650133U, 0x006f016fU, 0x00720170U, 0x006401bcU, 0x006101baU, 0x006e0134U, 0x004d014bU, 0x004d014cU,
	0x007301b5U, 0x0063017aU, 0x00650139U, 0x006a01b5U, 0x003c019eU, 0x040a013cU, 0x006401c3U, 0x006c0181U,
	0x07200173U, 0x007601bdU, 0x005f013eU, 0x006101c4U, 0x0032016bU, 0x01200178U, 0x00640109U, 0x005f0143U,
	0x003c0020U, 0x030a0148U, 0x006d0106U, 0x0064010dU, 0x0020017dU, 0x0b640111U, 0x00690192U, 0x00670108U,
	0x00490126U, 0x006e0150U, 0x15680155U, 0x086b0154U, 0x004f0126U, 0x0074014fU, 0x00690193U, 0x006e0156U,
	0x00550126U, 0x003e0178U, 0x006e0158U, 0x006b015aU, 0x006e015bU, 0x00720198U, 0x0065015dU, 0x0065015eU,
	0x00610126U, 0x005f015fU, 0x00740114U, 0x005f0162U, 0x006f01a3U, 0x006601abU, 0x006a0126U, 0x00690164U,
	0x006e0167U, 0x00650168U, 0x00500126U, 0x0039010aU, 0x127201aaU, 0x00790122U, 0x007301a6U, 0x006301b7U,
	0x00610136U, 0x00670132U, 0x006f01b2U, 0x007801adU, 0x0a6b0172U, 0x0047018eU, 0x006d0131U, 0x00640135U,
	0x003e000dU, 0x0079012fU, 0x006301c1U, 0x062001aeU, 0x0065017fU, 0x00650182U, 0x006901c2U, 0x006e017eU,
	0x0073013aU, 0x00610141U, 0x00630105U, 0x00640142U, 0x0073012aU, 0x0073016eU, 0x006f018dU, 0x00720186U,
	0x00640187U, 0x00200188U, 0x1064014eU, 0x00450189U, 0x002001bfU, 0x00770185U, 0x00620195U, 0x00760146U,
	0x0072018bU, 0x00650196U, 0x006c010eU, 0x005f019cU, 0x00720190U, 0x004e0126U, 0x00630118U, 0x006f0112U,
	0x00660116U, 0x00760151U, 0x006f0194U, 0x1a72019aU, 0x00740197U,          0U, 0x0020010cU,          0U,
	         0U,          0U,          0U, 0x006a0163U, 0x006501a9U, 0x19640169U, 0x0061016aU, 0x00690126U,
	0x00690127U, 0x006e01a8U, 0x00720121U, 0x006f0126U, 0x0e6e01abU, 0x00720126U, 0x003d018cU, 0x00610175U,
	0x0520011eU, 0x006501b4U, 0x005f01b6U, 0x00750126U, 0x007401afU, 0x006e01b3U, 0x0078012bU, 0x00200124U,
	0x00540145U, 0x007701b1U, 0x006d0176U, 0x1464017cU, 0x006f0137U, 0x006e01a7U, 0x00610183U, 0x00730179U,
	0x0020012eU, 0x00750138U, 0x006f013bU, 0x0069013fU, 0x007401beU
};

const PROGMEM uint8_t CMD_DICT[] = {
	0x0dU, 0xa2U, 0x01U, 	0x6cU, 0x99U, 0x01U, 	0x63U, 0xa5U, 0x01U, 	0x65U, 0x1aU, 0x02U,	// 0x0100
	0x72U, 0xddU, 0x02U, 	0x6eU, 0x2cU, 0x02U, 	0x5fU, 0x08U, 0x02U, 	0x68U, 0x06U, 0x01U,	// 0x010c
	0x0dU, 0x60U, 0x01U, 	0x20U, 0x2fU, 0x02U, 	0x77U, 0x32U, 0x02U, 	0x0dU, 0xd8U, 0x01U,	// 0x0118
	0x62U, 0x1aU, 0x02U, 	0x69U, 0xb9U, 0x02U, 	0x61U, 0x24U, 0x01U, 	0x74U, 0x20U, 0x02U,	// 0x0124
	0x20U, 0x23U, 0x02U, 	0x66U, 0x02U, 0x02U, 	0x74U, 0x6fU, 0x01U, 	0x72U, 0xbfU, 0x02U,	// 0x0130
	0x70U, 0x0eU, 0x02U, 	0x61U, 0xf0U, 0x01U, 	0x66U, 0xf6U, 0x01U, 	0x20U, 0x6cU, 0x01U,	// 0x013c
	0x69U, 0x51U, 0x01U, 	0x72U, 0xb3U, 0x02U, 	0x67U, 0x45U, 0x01U, 	0x6fU, 0x57U, 0x01U,	// 0x0148
	0x74U, 0x14U, 0x02U, 	0x6aU, 0x45U, 0x01U, 	0x74U, 0xadU, 0x02U, 	0x72U, 0xa7U, 0x02U,	// 0x0154
	0x6eU, 0xf6U, 0x01U, 	0x78U, 0x75U, 0x01U, 	0x66U, 0x89U, 0x02U, 	0x20U, 0x66U, 0x01U,	// 0x0160
	0x6eU, 0x3fU, 0x01U, 	0x6eU, 0xfcU, 0x01U, 	0x73U, 0x45U, 0x01U, 	0x74U, 0x45U, 0x01U,	// 0x016c
	0x0dU, 0x36U, 0x01U, 	0x76U, 0x9bU, 0x02U, 	0x74U, 0xeaU, 0x01U, 	0x79U, 0x4bU, 0x01U,	// 0x0178
	0x64U, 0x71U, 0x02U, 	0x20U, 0x3cU, 0x01U, 	0x66U, 0x1bU, 0x01U, 	0x67U, 0x1bU, 0x01U,	// 0x0184
	0x20U, 0x98U, 0x02U, 	0x63U, 0x65U, 0x02U, 	0x77U, 0xa1U, 0x02U, 	0x5fU, 0x2dU, 0x01U,	// 0x0190
	0x63U, 0x09U, 0x01U, 	0x66U, 0x33U, 0x01U, 	0x6eU, 0x02U, 0x02U, 	0x6bU, 0xd2U, 0x01U,	// 0x019c
	0x31U, 0xf3U, 0x01U, 	0x65U, 0xaeU, 0x01U, 	0x72U, 0x1bU, 0x01U, 	0x73U, 0x1bU, 0x01U,	// 0x01a8
	0x0dU, 0x50U, 0x02U, 	0x6fU, 0x6cU, 0x00U, 	0x0dU, 0x9fU, 0x01U, 	0x74U, 0x6bU, 0x02U,	// 0x01b4
	0x69U, 0x03U, 0x01U, 	0x0dU, 0xefU, 0x02U, 	0x74U, 0x0fU, 0x01U, 	0x75U, 0x65U, 0x02U,	// 0x01c0
	0x66U, 0xf2U, 0x02U, 	0x68U, 0x9cU, 0x01U, 	0x6eU, 0xc0U, 0x01U, 	0x61U, 0x8aU, 0x01U,	// 0x01cc
	0x6bU, 0x15U, 0x01U, 	0x75U, 0x2aU, 0x01U, 	0x70U, 0x4dU, 0x02U, 	0x64U, 0xdbU, 0x01U,	// 0x01d8
	0x72U, 0xe1U, 0x01U, 	0x61U, 0xd4U, 0x02U, 	0x65U, 0xdaU, 0x02U, 	0x65U, 0x84U, 0x01U,	// 0x01e4
	0x77U, 0xfeU, 0x02U, 	0x20U, 0x4aU, 0x02U, 	0x6fU, 0x90U, 0x01U, 	0x61U, 0x72U, 0x01U,	// 0x01f0
	0x63U, 0x29U, 0x02U, 	0x6eU, 0x96U, 0x01U, 	0x6fU, 0x35U, 0x02U, 	0x5fU, 0x11U, 0x02U,	// 0x01fc
	0x74U, 0x3bU, 0x02U, 	0x64U, 0xc2U, 0x02U, 	0x65U, 0x17U, 0x02U, 	0x74U, 0xe0U, 0x02U,	// 0x0208
	0x6fU, 0xd4U, 0x02U, 	0x65U, 0xbcU, 0x02U, 	0x5fU, 0x5aU, 0x01U, 	0x0dU, 0x5dU, 0x01U,	// 0x0214
	0x65U, 0x72U, 0x01U, 	0x72U, 0xceU, 0x02U, 	0x61U, 0x54U, 0x01U, 	0x70U, 0xc8U, 0x02U,	// 0x0220
	0x63U, 0xffU, 0x01U, 	0x64U, 0xaaU, 0x02U, 	0x68U, 0x05U, 0x02U, 	0x20U, 0xe9U, 0x02U,	// 0x022c
	0x61U, 0xe4U, 0x01U, 	0x65U, 0x4eU, 0x01U, 	0x74U, 0x38U, 0x02U, 	0x65U, 0x3eU, 0x02U,	// 0x0238
	0x20U, 0x41U, 0x02U, 	0x72U, 0x0bU, 0x02U, 	0x31U, 0x87U, 0x01U, 	0x62U, 0xe7U, 0x01U,	// 0x0244
	0x66U, 0x42U, 0x01U, 	0x0dU, 0x8fU, 0x02U, 	0x65U, 0x92U, 0x02U, 	0x0dU, 0x39U, 0x01U,	// 0x0250
	0x76U, 0x05U, 0x02U, 	0x6eU, 0x48U, 0x01U, 	0x0dU, 0xdeU, 0x01U, 	0x20U, 0x63U, 0x01U,	// 0x025c
	0x0dU, 0x95U, 0x02U, 	0x63U, 0xd5U, 0x01U, 	0x73U, 0x9eU, 0x02U, 	0x5fU, 0x1eU, 0x01U,	// 0x0268
	0x65U, 0x80U, 0x02U, 	0x0dU, 0x27U, 0x01U, 	0x76U, 0xf9U, 0x01U, 	0x65U, 0x7aU, 0x02U,	// 0x0274
	0x76U, 0xedU, 0x01U, 	0x0dU, 0x7dU, 0x02U, 	0x63U, 0xc9U, 0x01U, 	0x6eU, 0x86U, 0x02U,	// 0x0280
	0x65U, 0x6eU, 0x02U, 	0x74U, 0x8cU, 0x02U, 	0x64U, 0x12U, 0x01U, 	0x72U, 0x92U, 0x02U,	// 0x028c
	0x6fU, 0xcfU, 0x01U, 	0x61U, 0xb1U, 0x01U, 	0x65U, 0x0cU, 0x01U, 	0x6fU, 0x92U, 0x02U,	// 0x0298
	0x0dU, 0xc6U, 0x01U, 	0x65U, 0x5cU, 0x02U, 	0x6fU, 0x6dU, 0x00U, 	0x65U, 0xb1U, 0x01U,	// 0x02a4
	0x20U, 0xccU, 0x01U, 	0x6fU, 0xbdU, 0x01U, 	0x0dU, 0xf8U, 0x02U, 	0x75U, 0x74U, 0x02U,	// 0x02b0
	0x6cU, 0xb1U, 0x01U, 	0x64U, 0xf5U, 0x02U, 	0x61U, 0x12U, 0x01U, 	0x72U, 0xb7U, 0x01U,	// 0x02bc
	0x75U, 0x12U, 0x01U, 	0x0dU, 0x47U, 0x02U, 	0x64U, 0x99U, 0x01U, 	0x61U, 0xe3U, 0x02U,	// 0x02c8
	0x20U, 0x5fU, 0x02U, 	0x61U, 0x99U, 0x01U, 	0x73U, 0xabU, 0x01U, 	0x5fU, 0x81U, 0x01U,	// 0x02d4
	0x65U, 0x8dU, 0x01U, 	0x76U, 0x56U, 0x02U, 	0x20U, 0xa8U, 0x01U, 	0x72U, 0xfbU, 0x02U,	// 0x02e0
	0x0dU, 0x7eU, 0x01U, 	0x61U, 0x26U, 0x02U, 	0x6eU, 0x93U, 0x01U, 	0x64U, 0xd1U, 0x02U,	// 0x02ec
	0x65U, 0x7bU, 0x01U, 	0x64U, 0xd7U, 0x02U, 	0x61U, 0xc5U, 0x02U
};

const PROGMEM uint16_t CMD_TABLE[] = {
	0x0000U, 0x02b6U, 0x02ecU, 0x0253U, 0x02e6U, 0x021dU, 0x0277U, 0x01b4U,
	0x0118U, 0x0283U, 0x0259U, 0x01c3U, 0x0262U, 0x0169U, 0x02b0U, 0x0268U,
	0x0178U, 0x02a4U, 0x02cbU, 0x0130U, 0x01baU, 0x0100U, 0x0121U, 0x0244U
};

/* end of code */
