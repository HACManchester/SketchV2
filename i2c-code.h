
static inline void i2c_delay(void)
{
	// this gives us a bus speed of around 1kHz
	// too much faster and the pic tends to start nacking data
	// 10 gives us around 40kHz - needs PIC running around 16MHZ
	// 1 give us around the 110kHz rate
	_delay_loop_2(2);
}

static void wait_scl(void)
{
	unsigned int timeout = 0x2000;

	for (; timeout > 0; timeout--) {
		if (i2c_get_scl() != 0)
			return;
		_delay_loop_2(1);
	}

	printf_P(PSTR("timeout waiting for scl, SCL=%d, SDA=%d\n"),
		 i2c_get_scl(), i2c_get_sda());
}

static void i2c_start(void)
{
	wait_scl();

	sda_high();
	i2c_delay();
	scl_high();
	i2c_delay();
	sda_low();
	i2c_delay();
	scl_low();
	i2c_delay();
}

static void i2c_stop(void)
{
	sda_low();
	i2c_delay();
	scl_high();
	i2c_delay();
	sda_high();
	i2c_delay();	
}

static int i2c_tx(unsigned char byte)
{
	unsigned bit;

	//printf_P(PSTR("tx(%x)\n"), byte);

	for (bit = 8; bit > 0; bit--) {
		//printf_P(PSTR("send bit (%d)\n"), bit);
		if (byte & (0x80))
			sda_high();
		else
			sda_low();

		i2c_delay();

		scl_high();
		wait_scl();

		byte <<= 1;
		i2c_delay();

		scl_low();
		i2c_delay();
	}

	//printf_P(PSTR("await ack\n"));
	sda_high();
	scl_high();
	wait_scl();

	i2c_delay();
	
	byte = i2c_get_sda();
	scl_low();
	i2c_delay();

	//printf_P(PSTR("done byte"));
	return byte ? 1 : 0;
}

static void i2c_write_bit(unsigned bit)
{
	if (bit)
		sda_high();
	else
		sda_low();

	i2c_delay();
	scl_high();
	wait_scl();

	i2c_delay();
	scl_low();
	i2c_delay();
}

static unsigned char i2c_rx(unsigned ack)
{
	unsigned bit;
	unsigned char result = 0;

	/* ensure we are not driving sda before we start receiving */
	sda_high();
	i2c_delay();

	for (bit = 8; bit > 0; bit--) {
		//printf_P(PSTR("reading bit %d\n"), bit);
		scl_high();
		wait_scl();
		i2c_delay();

		result <<= 1;
		result |= i2c_get_sda() ? 1 : 0;
		
		scl_low();
		i2c_delay();
	}

	i2c_write_bit(ack);
	i2c_delay();

	return result;
}

int iic_send_bytes(unsigned char addr, unsigned size,
		   const unsigned char *data)
{
	unsigned char ret;

	i2c_start();
	ret = i2c_tx(addr << 1);
	if (ret) {
		//printf_P(PSTR("tx: nack addr\n"));
		goto err;
	}

	for (; size > 0; size--, data++) {
		ret = i2c_tx(*data);
		if (ret) {
			printf_P(PSTR("tx: nacked byte\n"));
			goto err;
		}
	}

	i2c_stop();
	return 0;

err:
	i2c_stop();
	return 1;
}

int iic_receive_byte(unsigned char addr)
{
	unsigned char result;
	unsigned char ret;

	i2c_start();

	//printf_P(PSTR("sending addr %x\n"), ((addr << 1) | 1));
	ret = i2c_tx((addr << 1) | 1);
	if (ret) {
		i2c_stop();
		return -1;
	}

	//printf_P(PSTR("reading result\n"));
	result = i2c_rx(1);

	i2c_stop();
	return result;
}

int iic_receive_bytes(unsigned char addr, unsigned size,
		     uint8_t *data)
{
	unsigned char ret;
	int i;

	i2c_start();

	//printf_P(PSTR("sending addr %x\n"), ((addr << 1) | 1));
	ret = i2c_tx((addr << 1) | 1);
	if (ret) {
		i2c_stop();
		return -1;
	}

	for (i = 0; i < size; i++) {
		data[i] = i2c_rx(0);
	}

	i2c_stop();
	return 0;
}
