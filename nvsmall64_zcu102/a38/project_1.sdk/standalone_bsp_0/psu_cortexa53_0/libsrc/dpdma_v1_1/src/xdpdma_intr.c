/*******************************************************************************
 *
 * Copyright (C) 2017 Xilinx, Inc.  All rights reserved.
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
*******************************************************************************/
/******************************************************************************/
/**
 *
 * @file xdppsu_intr.c
 *
 * This file contains functions related to XDpPsu interrupt handling.
 *
 * @note	None.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- -----------------------------------------------
 * 1.0   aad  01/17/17 Initial release.
 * </pre>
 *
*******************************************************************************/

/******************************* Include Files ********************************/
#include "xdpdma.h"


/*************************************************************************/
/**
 *
 * This function enables the interrupts that are required.
 *
 * @param    InstancePtr is pointer to the instance of DPDMA
 *
 * @return   None.
 *
 * @note     None.
 *
 * **************************************************************************/
void XDpDma_InterruptEnable(XDpDma *InstancePtr, u32 Mask)
{
	XDpDma_WriteReg(InstancePtr->Config.BaseAddr, XDPDMA_IEN, Mask);
}

/*************************************************************************/
/**
 *
 * This function handles the interrupts generated by DPDMA
 *
 * @param    InstancePtr is pointer to the instance of the DPDMA
 *
 * @return   None.
 *
 * @note     None.
 *
 * **************************************************************************/
void XDpDma_InterruptHandler(XDpDma *InstancePtr)
{
	u32 RegVal;
	RegVal = XDpDma_ReadReg(InstancePtr->Config.BaseAddr,
				XDPDMA_ISR);
	if(RegVal & XDPDMA_ISR_VSYNC_INT_MASK) {
		XDpDma_VSyncHandler(InstancePtr);
	}

	if(RegVal & XDPDMA_ISR_DSCR_DONE4_MASK) {
		XDpDma_SetChannelState(InstancePtr, AudioChan0, XDPDMA_DISABLE);
		InstancePtr->Audio[0].Current = NULL;
		XDpDma_WriteReg(InstancePtr->Config.BaseAddr, XDPDMA_ISR,
				XDPDMA_ISR_DSCR_DONE4_MASK);
	}

	if(RegVal & XDPDMA_ISR_DSCR_DONE5_MASK) {
		XDpDma_SetChannelState(InstancePtr, AudioChan1, XDPDMA_DISABLE);
		InstancePtr->Audio[1].Current = NULL;
		XDpDma_WriteReg(InstancePtr->Config.BaseAddr, XDPDMA_ISR,
				XDPDMA_ISR_DSCR_DONE5_MASK);
	}
}

/*************************************************************************/
/**
 *
 * This function handles frame new frames on VSync
 *
 * @param    InstancePtr is pointer to the instance of the driver.
 *
 * @return   None.
 *
 * @note     None.
 *
 * **************************************************************************/
void XDpDma_VSyncHandler(XDpDma *InstancePtr)
{
	Xil_AssertVoid(InstancePtr != NULL);

	/* Video Channel Trigger/Retrigger Handler */
	if(InstancePtr->Video.TriggerStatus == XDPDMA_TRIGGER_EN) {
		XDpDma_SetupChannel(InstancePtr, VideoChan);
		XDpDma_SetChannelState(InstancePtr, VideoChan,
				       XDPDMA_ENABLE);
		XDpDma_Trigger(InstancePtr, VideoChan);
	}
	else if(InstancePtr->Video.TriggerStatus == XDPDMA_RETRIGGER_EN) {
		XDpDma_SetupChannel(InstancePtr, VideoChan);
		XDpDma_ReTrigger(InstancePtr, VideoChan);
	}

	/* Graphics Channel Trigger/Retrigger Handler */
	if(InstancePtr->Gfx.TriggerStatus == XDPDMA_TRIGGER_EN) {
		XDpDma_SetupChannel(InstancePtr, GraphicsChan);
		XDpDma_SetChannelState(InstancePtr, GraphicsChan,
				       XDPDMA_ENABLE);
		XDpDma_Trigger(InstancePtr, GraphicsChan);
	}
	else if(InstancePtr->Gfx.TriggerStatus == XDPDMA_RETRIGGER_EN) {
		XDpDma_SetupChannel(InstancePtr, GraphicsChan);
		XDpDma_ReTrigger(InstancePtr, GraphicsChan);
	}

	/* Audio Channel 0 Trigger Handler */
	if(InstancePtr->Audio[0].TriggerStatus == XDPDMA_TRIGGER_EN) {
		XDpDma_SetupChannel(InstancePtr, AudioChan0);
		XDpDma_SetChannelState(InstancePtr, AudioChan0,
				       XDPDMA_ENABLE);
		XDpDma_Trigger(InstancePtr, AudioChan0);
	}

	/* Audio Channel 1 Trigger Handler */
	if(InstancePtr->Audio[1].TriggerStatus == XDPDMA_TRIGGER_EN) {
		XDpDma_SetupChannel(InstancePtr, AudioChan1);
		XDpDma_SetChannelState(InstancePtr, AudioChan1,
				       XDPDMA_ENABLE);
		XDpDma_Trigger(InstancePtr, AudioChan1);
	}
	/* Clear VSync Interrupt */
	XDpDma_WriteReg(InstancePtr->Config.BaseAddr, XDPDMA_ISR,
			XDPDMA_ISR_VSYNC_INT_MASK);
}
