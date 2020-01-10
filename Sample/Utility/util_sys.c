
/*******************************************************************************
	- Chip		: MG2470B
	- Vendor		: RadioPulse Inc, 2012.
	- Date		: 2012-04-25
	- Version		: V2.0

	[2012-04-25] V2.0
	- Initial Version for MG2470B(ChipID=0x71)
*******************************************************************************/

#include "include_top.h"
#include "util_sys.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Converts a measured ED(Energy Detection) or RSSI value in LQI to the value in dBm.
///	The relation between both units is linear.
///		\n	LQI = 0		--> 	dBm = -128
///		\n	LQI = 255	--> 	dBm = 0
///
///	@param	u8LQI	: Value to be converted in LQI.
///	@return	INT8. Converted value in dBm.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
INT8 SYS_LQITodBm(UINT8 u8LQI)
{
	UINT16	u16Ratio;
	INT8		i8dBm;

	u16Ratio = 100 * (255 - u8LQI);
	u16Ratio = u16Ratio / 255;

	i8dBm = 0 - u16Ratio;
	
	return i8dBm;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Converts a measured ED(Energy Detection) or RSSI value in dBm to the value in LQI.
///	The relation between both units is linear.
///		\n	dBm = -128	--> LQI = 0
///		\n	dBm = 0		--> LQI = 255
///
///	@param	i8dBm	: Value to be converted in dBm.
///	@return	UINT8. Converted value in LQI.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_dBmToLQI(INT8 i8dBm)
{
	UINT16	u16Ratio;
	UINT8	u8LQI;

	// RSSI : LQI type, 0~255, 255=high power
	if(i8dBm > 0)
	{
		u8LQI = 255;
	}
	else if(i8dBm > -100)
	{
		u16Ratio = 100 + i8dBm;
		u16Ratio *= 255;
		u16Ratio /= 100;
		u8LQI = u16Ratio;
	}
	else
	{
		u8LQI = 0;
	}
	
	return u8LQI;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global array of structure variable which contains information of received packets.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
tPACKET_INFO	gatRxQPacket[RF_RXBUF_SIZE];

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates the number of structure variable being occupied by packets.
///	When this is not 0, the received packet is present.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8		gu8RxQUsed = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates the index of "gatRxQMacPkt" to be written.
///	When a packet is received, it is written to "gatRxQMacPkt[gu8RxQWr]".
///	After writing, this is increased by 1. 
///	Note that if the increased value is equal to "RF_RXBUF_SIZE", it is cleared to 0.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8		gu8RxQWr = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which indicates the index of "gatRxQMacPkt" to be read.
///	When "gu8RxQUsed" is not 0, the oldest received packet is contained in "gatRxQMacPkt[gu8RxQRd]".
///	After processing the packet, this is increased by 1. 
///	Note that if the increased value is equal to "RF_RXBUF_SIZE", it is cleared to 0.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8		gu8RxQRd = 0;	

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Checks that a new received packet is present.
///
///	@param	void
///	@return	Pointer to the structure variable for the packet.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
tPACKET_INFO* SYS_RxQueueCheck(void)
{
	tPACKET_INFO*	ptRxPkt;
	
	ptRxPkt = 0;
	if(gu8RxQUsed)
	{
		ptRxPkt = &gatRxQPacket[gu8RxQRd];
	}
	
	return	ptRxPkt;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Updates variables for checking packets after processing the received packet.
///	"gu8RxQRd" is updated and "gu8RxQUsed" is decreased by 1.
///
///	@param	void
///	@return	void
///	
///////////////////////////////////////////////////////////////////////////////////////////////
void SYS_RxQueueClear(void)
{
	UINT8	u8PHYIE;

	u8PHYIE = PHYIE;
	PHYIE = 0;

	gu8RxQRd++;
	if(gu8RxQRd == RF_RXBUF_SIZE)	gu8RxQRd = 0;
	gu8RxQUsed--;

	PHYIE = u8PHYIE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which specifies whether MAC filtering is enabled, in other words, whether the information 
///	of the last received packet is contained. If this is 0, filtering is not worked. And, when a packet is received
///	filtering starts working.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8MacFilterEna = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which contains DSN field of the last received packet.
///	This is used for checking whether a new received packet is duplicated.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8MacFilterDSN;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which contains Source Address Mode field of the last received packet.
///	This is used for checking whether a new received packet is duplicated.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8	gu8MacFilterSrcMode;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Global variable which contains Source Address field of the last received packet.
///	This is used for checking whether a new received packet is duplicated.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
uMAC_ADDR	guMacFilterSrcAddr;

///////////////////////////////////////////////////////////////////////////////////////////////
///	
/// 	Writes information of the received packet to the structure variable after parsing.
///	Furthermore, specifies whether the packet is duplicated.
///
///	@param	ptInPkt	: Input. Pointer to the structure of a packet.
///	@param	ptOutMac	: Output. Pointer to the structure of a MAC frame.
///	@return	UINT8. Status. 0=Success, 1=Packet is broken, 2=Packet is duplicated.
///	
///////////////////////////////////////////////////////////////////////////////////////////////
UINT8 SYS_RxQueueParse(tPACKET_INFO* ptInPkt, tMAC_INFO *ptOutMac)
{
	UINT8	BuffIndex;

	ptOutMac->i8RSSI = ptInPkt->i8RSSI;
	ptOutMac->u8Correlation = ptInPkt->u8Correlation;
	
	ptOutMac->u8PhyPayLen = ptInPkt->au8Packet[0];
	ptOutMac->uFC_L.u8All = ptInPkt->au8Packet[1];
	ptOutMac->uFC_H.u8All = ptInPkt->au8Packet[2];
	ptOutMac->u8OobCode = (ptOutMac->uFC_H.bBit.u8RsvH << 2) | ptOutMac->uFC_H.bBit.u8RsvL;	
	ptOutMac->u8SeqNum = ptInPkt->au8Packet[3];

	if(ptOutMac->uFC_H.bBit.u8DstMode == maccAddrMode_Rsv)	return 1;
	if(ptOutMac->uFC_H.bBit.u8SrcMode == maccAddrMode_Rsv)	return 1;

	BuffIndex = 4;
	// Destination Address Field
	if(ptOutMac->uFC_H.bBit.u8DstMode == maccAddrMode_16bit)
	{	
		ptOutMac->u16DstPanID = (ptInPkt->au8Packet[5] << 8) | ptInPkt->au8Packet[4];
		ptOutMac->uDstAddr.u16Short = (ptInPkt->au8Packet[7] << 8) | ptInPkt->au8Packet[6];
		BuffIndex += 4;
	}
	else if(ptOutMac->uFC_H.bBit.u8DstMode == maccAddrMode_64bit)
	{	
		ptOutMac->u16DstPanID = (ptInPkt->au8Packet[5] << 8) | ptInPkt->au8Packet[4];
		rpmemcpy(ptOutMac->uDstAddr.au8Ext, &ptInPkt->au8Packet[6], 8);
		BuffIndex += 10;
	}	

	// Source Address Field
	if(ptOutMac->uFC_H.bBit.u8SrcMode == maccAddrMode_16bit)
	{
		if(ptOutMac->uFC_H.bBit.u8DstMode && ptOutMac->uFC_L.bBit.u8IP)
		{
			ptOutMac->u16SrcPanID = ptOutMac->u16DstPanID;
			ptOutMac->uSrcAddr.u16Short = (ptInPkt->au8Packet[BuffIndex+1] << 8) | ptInPkt->au8Packet[BuffIndex+0];
			BuffIndex += 2;
		}
		else
		{
			ptOutMac->u16SrcPanID = (ptInPkt->au8Packet[BuffIndex+1] << 8) | ptInPkt->au8Packet[BuffIndex+0];
			ptOutMac->uSrcAddr.u16Short = (ptInPkt->au8Packet[BuffIndex+3] << 8) | ptInPkt->au8Packet[BuffIndex+2];
			BuffIndex += 4;
		}	
	}
	else if(ptOutMac->uFC_H.bBit.u8SrcMode == maccAddrMode_64bit)
	{
		if(ptOutMac->uFC_H.bBit.u8DstMode && ptOutMac->uFC_L.bBit.u8IP)
		{
			ptOutMac->u16SrcPanID = ptOutMac->u16DstPanID;
			rpmemcpy(ptOutMac->uSrcAddr.au8Ext, &ptInPkt->au8Packet[BuffIndex+0], 8);
			BuffIndex += 8;
		}
		else
		{
			ptOutMac->u16SrcPanID = (ptInPkt->au8Packet[BuffIndex+1] << 8) | ptInPkt->au8Packet[BuffIndex+0];
			rpmemcpy(ptOutMac->uSrcAddr.au8Ext, &ptInPkt->au8Packet[BuffIndex+2], 8);
			BuffIndex += 10;
		}			
	}
	ptOutMac->u8MacHeadLen = BuffIndex - 1;

	// If (MAC Header's length + 2) is more than PHY payload length, this packet is not correct.
	// Here, 2 means the legnth of FCS field.
	if( (ptOutMac->u8MacHeadLen + 2) > ptOutMac->u8PhyPayLen)
	{	
		return	1;
	}
	else
	{
		// Discard duplicated packets. 
		// For more powerful filtering, the information of the received packets should be 
		// maintained for some time using timers.
		if(gu8MacFilterEna)
		{
			if( (ptOutMac->uFC_H.bBit.u8SrcMode == gu8MacFilterSrcMode)
			&& (ptOutMac->u8SeqNum == gu8MacFilterDSN) )
			{		
				if(ptOutMac->uFC_H.bBit.u8SrcMode == maccAddrMode_16bit)
				{
					if(ptOutMac->uSrcAddr.u16Short == guMacFilterSrcAddr.u16Short)
					{
						return 2;
					}
				}
				else if(ptOutMac->uFC_H.bBit.u8SrcMode == maccAddrMode_64bit)
				{
					if( memcmp(ptOutMac->uSrcAddr.au8Ext, guMacFilterSrcAddr.au8Ext, 8) == 0)
					{
						return 2;
					}
				}
			}
		}

		// Maintain information of the last received packet.
		gu8MacFilterEna = 1;
		gu8MacFilterSrcMode = ptOutMac->uFC_H.bBit.u8SrcMode;
		gu8MacFilterDSN = ptOutMac->u8SeqNum;
		if(ptOutMac->uFC_H.bBit.u8SrcMode == maccAddrMode_16bit)
		{
			guMacFilterSrcAddr.u16Short = ptOutMac->uSrcAddr.u16Short;
		}
		else if(ptOutMac->uFC_H.bBit.u8SrcMode == maccAddrMode_64bit)
		{
			rpmemcpy(guMacFilterSrcAddr.au8Ext, ptOutMac->uSrcAddr.au8Ext, 8);
		}

		// MAC payload length
		ptOutMac->u8MacPayLen = ptOutMac->u8PhyPayLen - ptOutMac->u8MacHeadLen - 2;
		// MAC payload(MSDU)
		ptOutMac->pu8MacPayload = &ptInPkt->au8Packet[BuffIndex];

		// MAC Footer(FCS)
		ptOutMac->u8FcsLow = ptInPkt->au8Packet[ptOutMac->u8PhyPayLen - 1];
		ptOutMac->u8FcsHigh = ptInPkt->au8Packet[ptOutMac->u8PhyPayLen];
		
		return	0;
	}	
}
