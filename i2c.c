// PD4 = SDA
// PD5 = SCL
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

#include <util/delay.h>

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))


#define SDA	(1 << 6)
#define SCL	(1 << 4)

static inline void sda_high(void) { DDRC &= ~SDA; }
static inline void sda_low(void)  { DDRC |= SDA; }

// moved SCL to pin pc5 as debug

static inline void scl_high(void) { DDRC &= ~SCL; }
static inline void scl_low(void)  { DDRC |= SCL; }

static inline unsigned char i2c_get_scl(void) { return PINC & SCL; }
static inline unsigned char i2c_get_sda(void) { return PINC & SDA; }

#define iic_send_bytes i2c_send_bytes
#define iic_receive_byte i2c_receive_byte
#define iic_receive_bytes i2c_receive_bytes

#include "i2c-code.h"
#include "i2c.h"

void i2c_init(void)
{
	sda_high();     // default to input
	scl_high();     // default to input
	PORTC &= ~SDA;  // drive low when output
	PORTC &= ~SCL;  // drive low when output
}
