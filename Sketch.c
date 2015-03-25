/*
             LUFA Library
     Copyright (C) Dean Camera, 2014.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2014  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the VirtualSerial demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include "Sketch.h"
#include "i2c.h"

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = INTERFACE_ID_CDC_CCI,
				.DataINEndpoint           =
					{
						.Address          = CDC_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};

/** Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 *  used like any regular character stream in the C APIs.
 */
static FILE USBSerialStream;


#define ACC_ADDR (0x68 >> 0)

static int acc_rd(unsigned reg, uint8_t *rb)
{
	uint8_t cmd[1];
	uint8_t resp[2];
	int result;

	cmd[0] = reg;

	if (0) {
		i2c_send_bytes(ACC_ADDR, 1, cmd);
		i2c_receive_bytes(ACC_ADDR, 2, resp);
	} else {
		i2c_send_bytes(ACC_ADDR, 1, cmd);
		//_delay_us(10);
		resp[0] = i2c_receive_byte(ACC_ADDR);
		//_delay_us(10);
		cmd[0] = reg + 1;
		i2c_send_bytes(ACC_ADDR, 1, cmd);
		//_delay_us(10);
		resp[1] = i2c_receive_byte(ACC_ADDR);
	}
	
	result = (unsigned int)resp[1];
	result |= ((unsigned int)resp[0]) << 8;

	if (resp[0] & 0x80)
		result = -((65536 - result) + 1);

	if (rb) {
		rb[0] = resp[0];
		rb[1] = resp[1];
	}

	_delay_us(10);
	return result;
}

void get_accelerometer_data(int *x, int *y, int *z, int *t, uint8_t *rb)
{
	*t = acc_rd(0x41, rb);
	*x = acc_rd(0x3b, NULL);
	*y = acc_rd(0x3d, NULL);
	*z = acc_rd(0x3f, NULL);
	//*x = *y = *z = 0;
}

void accelerometer_init(void)
{
	uint8_t buff[2];

	buff[0] = 0x6b;		/* power management control */
	buff[1] = 0x0;		/* ensure device is out of sleep mode */

	i2c_send_bytes(ACC_ADDR, 2, buff);
}

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	int x, y, z, t;
	int count = 1;

	SetupHardware();
	i2c_init();
	accelerometer_init();

	/* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);

	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	GlobalInterruptEnable();

	if (0) {
		uint8_t buff[1];

		buff[0] = 0x75;	/* whoami register */
		i2c_send_bytes(ACC_ADDR, 1, buff);
		i2c_receive_bytes(ACC_ADDR, 1, buff);
		fprintf(&USBSerialStream, "WHOAMI=%x\n", buff[0]);
	}
	
	for (;;)
	{
	    _delay_ms(1);
	    
	    char* ReportString  = NULL;
	
	    uint8_t ButtonStatus_LCL = Buttons_GetStatus();
        Encoder1Status_LCL = Encoder_1_GetStatus();
	
	    if (Encoder1Status_LCL == 1)
		    ReportString = "U\n";
	    else if (Encoder1Status_LCL == 255)
		    ReportString = "D\n";

	    Encoder2Status_LCL = Encoder_2_GetStatus();
	    if (Encoder2Status_LCL == 1)
		    ReportString = "L\n";
	    else if (Encoder2Status_LCL == 255)
		    ReportString = "R\n";

	    if (ButtonStatus_LCL & BUTTONS_BUTTON1)
		    ReportString = "S\n";
		    
		if (ReportString != NULL)
	    {
		    fputs(ReportString, &USBSerialStream);
	    }

		if (--count == 0) {
			uint8_t rb[2];
	
			get_accelerometer_data(&x, &y, &z, &t, rb);
			fprintf(&USBSerialStream, "X:%d Y:%d Z:%d\nT=%d (%x %x)\n",
				x, y, z, t, rb[0], rb[1]);
			count = 10;
		}
		
		/* Must throw away unused bytes from the host, or it will lock up while waiting for the device */
		CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);

		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{

	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
	Buttons_Init();
	Encoders_Init();
	LEDs_Init();
	USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

