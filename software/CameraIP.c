
/***************************** Include Files *******************************/
#include "CameraIP.h"

u32 OV7670_BaseAddress;

/************************** Function Definitions ***************************/

int OV7670_initialize(u32 BaseAddress)
{
	OV7670_BaseAddress = BaseAddress;
	//return CAMERAIP_Reg_SelfTest(OV7670_BaseAddress);
	return 0;
}

void OV7670_setup(u16 data)
{
	u8 taken = 0;

	do {
		taken = CAMERAIP_mReadReg(OV7670_BaseAddress, CAMERAIP_S00_AXI_SLV_REG0_OFFSET);
	} while (taken == 0);

	CAMERAIP_mWriteReg(OV7670_BaseAddress, CAMERAIP_S00_AXI_SLV_REG1_OFFSET, data);
}
