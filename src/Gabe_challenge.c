/*****************************************************************************
* Copyright (C) 2019 by Jon Warriner
*
* Redistribution, modification or use of this software in source or binary
* forms is permitted as long as the files maintain this copyright. Users are
* permitted to modify this and use it to learn about the field of embedded
* software. Jon Warriner and the University of Colorado are not liable for
* any misuse of this material.
*
*****************************************************************************/
/**
* @file Gabe_challenge.c
* @brief main file for Gabe's Physics/Math/Coding challenge.
*
* This source file provides the main() function for Gabe's Physics/Math/Coding challenge
*
* @author Jon Warriner
* @date May 9, 2019
* @version 1.0
*
*/
 
#include <stdio.h>
#include "clock_config.h"
#include "uart.h"
#include "i2c.h"
#include "MMA8451Q.h"
#include "led.h"
#include "ring.h"
#include "disp.h"
#include "angles.h"
#include "MKL25Z4.h"

//#define PART_2
//#define PART_3
//#define PART_4
#define PART_5

#define TX_BUF_SIZE	32

ring_t *tx_buf = 0;

disp_t disp = {0};

I2C_Packet gPacket = {0};

MMA8451Q accel = {0};

ANGLE_DATA angles = {0};

/*
 * @brief   Application entry point.
 */
int main(void) {
  	/* Init board hardware. */
    BOARD_InitBootClocks();

    //Inialize the GPIO for LED blinking
    LED_init();

    tx_buf = ring_init(TX_BUF_SIZE);
    //Initialize the display module
    disp_init(&disp, tx_buf, &UART_EN_TX_INT);

    //Initialize UART0
    UART_init();

    //Initialize the I2C module
    I2C_init(&gPacket);

    //just letting this print to the console window to let me know the code started.
    printf("Hello World\n");

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;

    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++;
        Display_task(&disp);
        Display_New_Val(&disp,accel.data.data.x_data);
       	Update_MMA8451Q(&accel, &gPacket);
        I2C_POLL(&gPacket);
        Check_I2C_Callback(&gPacket, (void *)&accel);

        Calc_angles(&accel.data.data, &angles);
    }
    return 0 ;
}

void UART0_DriverIRQHandler(void)
{
char temp;

	//if the UART is ready to transmit, and we still have data in the buffer, then grab the next character and transmit it.
	if(UART_TX_rdy())
	{
		if(entries(tx_buf) != 0)
		{
			extract(tx_buf, &temp);
			UART_TX(temp);
		}
		else
		{
			UART_DIS_TX_INT();
		}
	}
}
