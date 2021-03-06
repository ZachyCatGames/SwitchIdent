#include <stdio.h>
#include <switch.h>

#include "setcal.h"

static Result psmIsBatteryChargingEnabled(bool *out) {
    IpcCommand c;
    ipcInitialize(&c);
    
    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 4;
    
    Result rc = serviceIpcDispatch(psmGetServiceSession());
    
    if(R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);
        
        struct {
            u64 magic;
            u64 result;
            u8 enable;
        } *resp = r.Raw;
        
        rc = resp->result;
        
        if (R_SUCCEEDED(rc)) {
            *out = resp->enable;
        }
    }
    
    return rc;
}

static Result psmGetRawBatteryChargePercentage(u64 *out) {
    IpcCommand c;
    ipcInitialize(&c);
    
    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 13;
    
    Result rc = serviceIpcDispatch(psmGetServiceSession());
    
    if(R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);
        
        struct {
            u64 magic;
            u64 result;
            u64 age_percentage;
        } *resp = r.Raw;
        
        rc = resp->result;
        
        if (R_SUCCEEDED(rc)) {
            *out = resp->age_percentage;
        }
    }
    
    return rc;
}

static Result psmIsEnoughPowerSupplied(bool *out) {
    IpcCommand c;
    ipcInitialize(&c);
    
    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 14;
    
    Result rc = serviceIpcDispatch(psmGetServiceSession());
    
    if(R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);
        
        struct {
            u64 magic;
            u64 result;
            u8 power_supplied;
        } *resp = r.Raw;
        
        rc = resp->result;
        
        if (R_SUCCEEDED(rc)) {
            *out = resp->power_supplied;
        }
    }
    
    return rc;
}

static Result psmGetBatteryAgePercentage(u64 *out) {
    IpcCommand c;
    ipcInitialize(&c);
    
    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;
    
    raw = ipcPrepareHeader(&c, sizeof(*raw));
    
    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 15;
    
    Result rc = serviceIpcDispatch(psmGetServiceSession());
    
    if(R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);
        
        struct {
            u64 magic;
            u64 result;
            u64 age_percentage;
        } *resp = r.Raw;
        
        rc = resp->result;
        
        if (R_SUCCEEDED(rc)) {
            *out = resp->age_percentage;
        }
    }
    
    return rc;
}

u32 SwitchIdent_GetBatteryPercent(void) {
	Result ret = 0;
	u32 out = 0;

	if (R_FAILED(ret = psmGetBatteryChargePercentage(&out)))
		return -1;

	return out;
}

char *SwitchIdent_GetChargerType(void) {
	Result ret = 0;
	ChargerType charger_type;

	if (R_FAILED(ret = psmGetChargerType(&charger_type)))
		return NULL;

    if (charger_type == ChargerType_Charger)
        return "Official charger or dock";
    else if (charger_type == ChargerType_Usb)
        return "USB-C charger";
    else
        return "No charger connected";

    return NULL;
}

bool SwitchIdent_IsCharging(void) {
    Result ret = 0;
    ChargerType charger_type;

    if (R_FAILED(ret = psmGetChargerType(&charger_type)))
        return false;

    if ((charger_type == ChargerType_Charger) || (charger_type == ChargerType_Usb))
        return true;

    return false;
}

bool SwitchIdent_IsChargingEnabled(void) {
	Result ret = 0;
	bool out = 0;

	if (R_FAILED(ret = psmIsBatteryChargingEnabled(&out)))
		return -1;

	return out;
}

char *SwitchIdent_GetVoltageState(void) {
	Result ret = 0;
	u32 out = 0;

    char *states[]=
    {
        "Power state needs shutdown",
        "Power state needs sleep",
        "Performance boost cannot be entered",
        "Normal",
        "Unknown"
    };

    if (R_SUCCEEDED(ret = psmGetBatteryVoltageState(&out))) {
        if (out < 4)
            return states[out];
    }

    printf("psmGetBatteryVoltageState() failed: 0x%x.\n\n", ret);
    return states[4];
}

u64 SwitchIdent_GetRawBatteryChargePercentage(void) {
	Result ret = 0;
	u64 out = 0;

	if (R_FAILED(ret = psmGetRawBatteryChargePercentage(&out)))
		return -1;

	return out;
}

bool SwitchIdent_IsEnoughPowerSupplied(void) {
	Result ret = 0;
	bool out = 0;

	if (R_FAILED(ret = psmIsEnoughPowerSupplied(&out)))
		return -1;

	return out;
}

u64 SwitchIdent_GetBatteryAgePercent(void) {
	Result ret = 0;
	u64 out = 0;

	if (R_FAILED(ret = psmGetBatteryAgePercentage(&out)))
		return -1;

	return out;
}

char *SwitchIdent_GetBatteryLot(void) {
    Result ret = 0;
    static char battery_lot[0x13];

    if (R_FAILED(ret = setcalGetBatteryLot(battery_lot))) {
        printf("setcalGetBatteryLot() failed: 0x%x.\n\n", ret);
        return NULL;
    }

    return battery_lot;
}
