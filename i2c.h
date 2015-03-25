/* i2c.h
 * Copyright 2015 Ben Dooks <ben@fluff.org>
*/

extern void i2c_init(void);

extern int i2c_send_bytes(unsigned char addr, unsigned size,
			  const unsigned char *data);

extern int i2c_receive_byte(unsigned char addr);

extern int i2c_receive_bytes(unsigned char addr, unsigned size,
			     unsigned char *data);

