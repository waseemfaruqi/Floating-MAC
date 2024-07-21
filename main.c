/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

#include "xparameters.h"
#include "../../design_1_wrapper_hw_platform_0/drivers/floating_MAC_v1_0/src/floating_MAC.h"
#include "xil_io.h"
#include "xil_types.h"

int main()
{
	int i;
	// test vectors in hexadecimal
	uint32_t a[5] = {0x40a00000, 0x41200000, 0x41c80000, 0x3f800000, 0x40400000};
	uint32_t b[5] = {0x42960000, 0x42c80000, 0x42fa0000, 0x42480000, 0x42b40000};
	uint32_t c[5] = {0x43bb8000, 0x44abe000, 0x458ca000, 0x458e3000, 0x4596a000};
	uint32_t z; 	// circuit output
	/* Test vectors in floating--point numbers
	a[5] = {5.0, 10.0, 25.0, 1.0, 3.0}
	b[5] = {75.0,100.0,125.0,50.0,90.0}
	c[5] = {375, 1375, 4500, 4550, 4820}
	 */

	init_platform();

	print("test float multiply accumulate\n\r");

	/* a => slv_reg0, b => slv_reg1, z => slv_reg2,
	* reset => slv_reg3(1), req => slv_reg3(0), rdy => slv_reg4(0),
	*/

	// reset and lower
	FLOATING_MAC_mWriteReg(XPAR_FLOATING_MAC_0_S00_AXI_BASEADDR, 12, 2);

	// lower reset
	FLOATING_MAC_mWriteReg(XPAR_FLOATING_MAC_0_S00_AXI_BASEADDR, 12, 0);
	// print slv_reg2, accumulator clear?

	z = FLOATING_MAC_mReadReg(XPAR_FLOATING_MAC_0_S00_AXI_BASEADDR, 8);
	xil_printf("initial value = %x \r\n", z);

	for (i=0;i<5;i++)
	{
		// write operands
		FLOATING_MAC_mWriteReg(XPAR_FLOATING_MAC_0_S00_AXI_BASEADDR, 0, a[i]);
		xil_printf("(%x * " ,FLOATING_MAC_mReadReg(XPAR_FLOATING_MAC_0_S00_AXI_BASEADDR, 0));

		FLOATING_MAC_mWriteReg(XPAR_FLOATING_MAC_0_S00_AXI_BASEADDR, 4, b[i]);
		xil_printf("%x) + %x = " ,FLOATING_MAC_mReadReg(XPAR_FLOATING_MAC_0_S00_AXI_BASEADDR, 4), z);

		// request req = 1
		FLOATING_MAC_mWriteReg(XPAR_FLOATING_MAC_0_S00_AXI_BASEADDR, 12, 1);
		// wait on rdy low
		while (FLOATING_MAC_mReadReg(XPAR_FLOATING_MAC_0_S00_AXI_BASEADDR, 16));
		// lower req= 0
		FLOATING_MAC_mWriteReg(XPAR_FLOATING_MAC_0_S00_AXI_BASEADDR, 12, 0);
		// wait on rdy high
		while (!FLOATING_MAC_mReadReg(XPAR_FLOATING_MAC_0_S00_AXI_BASEADDR, 16));
		z = FLOATING_MAC_mReadReg(XPAR_FLOATING_MAC_0_S00_AXI_BASEADDR, 8);
		//print answer
		xil_printf("%x\r\n",z);
		if (z != c[i]) print("error\r\n");
	}
	cleanup_platform();
	return 0;

}
