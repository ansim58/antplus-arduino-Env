#include "ANTPLUS_ProfileEnvironmentMonitor.h"
#include "ANTPLUS_BaseDataPage.h"
#include "ANTPLUS_BaseDataPageMsg.h"
#include "ANTPLUS_PrivateDefines.h"

#define ENVIRONMENTMONITOR_DATAPAGE1_NUMBER 0x01
#define ANTPLUS_ENVIRONMENTMONITOR_DATAPAGE_MANUFACTURERINFORMATION_NUMBER 0x50

ProfileEnvironmentMonitor::ProfileEnvironmentMonitor(uint16_t deviceNumber, uint8_t transmissionType) : 
    BaseMasterProfile(deviceNumber, transmissionType) {}

void ProfileEnvironmentMonitor::begin() {
    BaseMasterProfile::begin();
}

void ProfileEnvironmentMonitor::transmitNextDataPage() {
    if (_currentDataPage == 0) {
        sendManufacturerInfo();
    } else {
        sendDefaultDataPage();
    }
    _currentDataPage = (_currentDataPage + 1) % 2;
}

bool ProfileEnvironmentMonitor::isDataPageValid(uint8_t dataPage) {
    return dataPage == ENVIRONMENTMONITOR_DATAPAGE1_NUMBER || 
           dataPage == ANTPLUS_ENVIRONMENTMONITOR_DATAPAGE_MANUFACTURERINFORMATION_NUMBER;
}

void ProfileEnvironmentMonitor::sendDefaultDataPage() {
    EnvironmentMonitorDefaultMsg msg;
    if (_defaultMsgHandler) {
        _defaultMsgHandler(msg, _defaultMsgData);
    }

    BaseDataPageMsg<BroadcastDataMsg> dp;
    dp.setDataBuffer(_buffer);
    dp.set8BitValue(ENVIRONMENTMONITOR_DATAPAGE1_NUMBER, ANTPLUS_BASEDATAPAGE_DATAPAGE_BYTE);
    
    // Temperaturwerte nach ANT+ Spezifikation kodieren
    dp.set16BitValue(msg.currentTemperature, 1, LITTLE_ENDIAN);
    dp.set16BitValue(msg.maxTemperature, 3, LITTLE_ENDIAN);
    dp.set16BitValue(msg.minTemperature, 5, LITTLE_ENDIAN);
    
    // Reservierte Bytes setzen
    dp.set8BitValue(0xFF, 7);
    
    transmitMsg(dp);
}

void ProfileEnvironmentMonitor::sendManufacturerInfo() {
    EnvironmentMonitorManufacturerInformationMsg msg;
    if (_manufacturerInfoHandler) {
        _manufacturerInfoHandler(msg, _manufacturerInfoData);
    }

    BaseDataPageMsg<BroadcastDataMsg> dp;
    dp.setDataBuffer(_buffer);
    dp.set8BitValue(ANTPLUS_ENVIRONMENTMONITOR_DATAPAGE_MANUFACTURERINFORMATION_NUMBER, ANTPLUS_BASEDATAPAGE_DATAPAGE_BYTE);
    
    dp.set8BitValue(msg.manufacturerIdLsb, 1);
    dp.set16BitValue(msg.serialNumber, 2, LITTLE_ENDIAN);
    
    // Reservierte Bytes setzen
    dp.set32BitValue(0xFFFFFFFF, 4);
    
    transmitMsg(dp);
}

// Handler-Registrierungsmethoden
void ProfileEnvironmentMonitor::createEnvironmentMonitorDefaultMsg(void (*handler)(EnvironmentMonitorDefaultMsg&, uintptr_t), uintptr_t data) {
    _defaultMsgHandler = handler;
    _defaultMsgData = data;
}

void ProfileEnvironmentMonitor::createEnvironmentMonitorManufacturerInformationMsg(void (*handler)(EnvironmentMonitorManufacturerInformationMsg&, uintptr_t), uintptr_t data) {
    _manufacturerInfoHandler = handler;
    _manufacturerInfoData = data;
}
