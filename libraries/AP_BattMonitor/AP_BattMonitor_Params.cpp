#include <AP_Common/AP_Common.h>
#include <AP_Vehicle/AP_Vehicle_Type.h>
#include "AP_BattMonitor_Params.h"
#include "AP_BattMonitor_Analog.h"

#if APM_BUILD_TYPE(APM_BUILD_ArduCopter)
  #define DEFAULT_LOW_BATTERY_VOLTAGE 10.5f
#else
  #define DEFAULT_LOW_BATTERY_VOLTAGE 0.0f
#endif // APM_BUILD_TYPE(APM_BUILD_ArduCopter)

const AP_Param::GroupInfo AP_BattMonitor_Params::var_info[] = {
    // @Param: MONITOR
    // @DisplayName: Battery monitoring
    // @Description: Controls enabling monitoring of the battery's voltage and current
    // @Values: 0:Disabled,3:Analog Voltage Only,4:Analog Voltage and Current,5:Solo,6:Bebop,7:SMBus-Maxell,8:UAVCAN-BatteryInfo,9:BLHeli ESC
    // @User: Standard
    // @RebootRequired: True
    AP_GROUPINFO_FLAGS("MONITOR", 1, AP_BattMonitor_Params, _type, BattMonitor_TYPE_NONE, AP_PARAM_FLAG_ENABLE),

    // @Param: VOLT_PIN
    // @DisplayName: Battery Voltage sensing pin
    // @Description: Setting this to 0 ~ 13 will enable battery voltage sensing on pins A0 ~ A13. On the PX4-v1 it should be set to 100. On the Pixhawk, Pixracer and NAVIO boards it should be set to 2, Pixhawk2 Power2 is 13.
    // @Values: -1:Disabled, 0:A0, 1:A1, 2:Pixhawk/Pixracer/Navio2/Pixhawk2_PM1, 13:Pixhawk2_PM2, 100:PX4-v1
    // @User: Standard
    // @RebootRequired: True
    AP_GROUPINFO("VOLT_PIN", 2, AP_BattMonitor_Params, _volt_pin, AP_BATT_VOLT_PIN),

    // @Param: CURR_PIN
    // @DisplayName: Battery Current sensing pin
    // @Description: Setting this to 0 ~ 13 will enable battery current sensing on pins A0 ~ A13. On the PX4-v1 it should be set to 101. On the Pixhawk, Pixracer and NAVIO boards it should be set to 3, Pixhawk2 Power2 is 14.
    // @Values: -1:Disabled, 1:A1, 2:A2, 3:Pixhawk/Pixracer/Navio2/Pixhawk2_PM1, 14:Pixhawk2_PM2, 101:PX4-v1
    // @User: Standard
    // @RebootRequired: True
    AP_GROUPINFO("CURR_PIN", 3, AP_BattMonitor_Params, _curr_pin, AP_BATT_CURR_PIN),

    // @Param: VOLT_MULT
    // @DisplayName: Voltage Multiplier
    // @Description: Used to convert the voltage of the voltage sensing pin (@PREFIX@VOLT_PIN) to the actual battery's voltage (pin_voltage * VOLT_MULT). For the 3DR Power brick on APM2 or Pixhawk, this should be set to 10.1. For the Pixhawk with the 3DR 4in1 ESC this should be 12.02. For the PX using the PX4IO power supply this should be set to 1.
    // @User: Advanced
    AP_GROUPINFO("VOLT_MULT", 4, AP_BattMonitor_Params, _volt_multiplier, AP_BATT_VOLTDIVIDER_DEFAULT),

    // @Param: AMP_PERVLT
    // @DisplayName: Amps per volt
    // @Description: Number of amps that a 1V reading on the current sensor corresponds to. On the APM2 or Pixhawk using the 3DR Power brick this should be set to 17. For the Pixhawk with the 3DR 4in1 ESC this should be 17.
    // @Units: A/V
    // @User: Standard
    AP_GROUPINFO("AMP_PERVLT", 5, AP_BattMonitor_Params, _curr_amp_per_volt, AP_BATT_CURR_AMP_PERVOLT_DEFAULT),

    // @Param: AMP_OFFSET
    // @DisplayName: AMP offset
    // @Description: Voltage offset at zero current on current sensor
    // @Units: V
    // @User: Standard
    AP_GROUPINFO("AMP_OFFSET", 6, AP_BattMonitor_Params, _curr_amp_offset, 0),

    // @Param: CAPACITY
    // @DisplayName: Battery capacity
    // @Description: Capacity of the battery in mAh when full
    // @Units: mAh
    // @Increment: 50
    // @User: Standard
    AP_GROUPINFO("CAPACITY", 7, AP_BattMonitor_Params, _pack_capacity, 3300),

    // @Param: WATT_MAX
    // @DisplayName: Maximum allowed power (Watts)
    // @Description: If battery wattage (voltage * current) exceeds this value then the system will reduce max throttle (THR_MAX, TKOFF_THR_MAX and THR_MIN for reverse thrust) to satisfy this limit. This helps limit high current to low C rated batteries regardless of battery voltage. The max throttle will slowly grow back to THR_MAX (or TKOFF_THR_MAX ) and THR_MIN if demanding the current max and under the watt max. Use 0 to disable.
    // @Units: W
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO_FRAME("WATT_MAX", 8, AP_BattMonitor_Params, _watt_max, 0, AP_PARAM_FRAME_PLANE),

    // @Param: SERIAL_NUM
    // @DisplayName: Battery serial number
    // @Description: Battery serial number, automatically filled in for SMBus batteries, otherwise will be -1. With UAVCAN it is the battery_id.
    // @User: Advanced
    AP_GROUPINFO("SERIAL_NUM", 9, AP_BattMonitor_Params, _serial_number, AP_BATT_SERIAL_NUMBER_DEFAULT),

    // @Param: LOW_TIMER
    // @DisplayName: Low voltage timeout
    // @Description: This is the timeout in seconds before a low voltage event will be triggered. For aircraft with low C batteries it may be necessary to raise this in order to cope with low voltage on long takeoffs. A value of zero disables low voltage errors.
    // @Units: s
    // @Increment: 1
    // @Range: 0 120
    // @User: Advanced
    AP_GROUPINFO("LOW_TIMER", 10, AP_BattMonitor_Params, _low_voltage_timeout, 10),

    // @Param: FS_VOLTSRC
    // @DisplayName: Failsafe voltage source
    // @Description: Voltage type used for detection of low voltage event
    // @Values: 0:Raw Voltage, 1:Sag Compensated Voltage
    // @User: Advanced
    AP_GROUPINFO("FS_VOLTSRC", 11, AP_BattMonitor_Params, _failsafe_voltage_source, BattMonitor_LowVoltageSource_Raw),

    // @Param: LOW_VOLT
    // @DisplayName: Low battery voltage
    // @Description: Battery voltage that triggers a low battery failsafe. Set to 0 to disable. If the battery voltage drops below this voltage continuously for more then the period specified by the @PREFIX@LOW_TIMER parameter then the vehicle will perform the failsafe specified by the @PREFIX@FS_LOW_ACT parameter.
    // @Units: V
    // @Increment: 0.1
    // @User: Standard
    AP_GROUPINFO("LOW_VOLT", 12, AP_BattMonitor_Params, _low_voltage, DEFAULT_LOW_BATTERY_VOLTAGE),

    // @Param: LOW_MAH
    // @DisplayName: Low battery capacity
    // @Description: Battery capacity at which the low battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the @PREFIX@FS_LOW_ACT parameter.
    // @Units: mAh
    // @Increment: 50
    // @User: Standard
    AP_GROUPINFO("LOW_MAH", 13, AP_BattMonitor_Params, _low_capacity, 0),

    // @Param: CRT_VOLT
    // @DisplayName: Critical battery voltage
    // @Description: Battery voltage that triggers a critical battery failsafe. Set to 0 to disable. If the battery voltage drops below this voltage continuously for more then the period specified by the @PREFIX@LOW_TIMER parameter then the vehicle will perform the failsafe specified by the @PREFIX@FS_CRT_ACT parameter.
    // @Units: V
    // @Increment: 0.1
    // @User: Standard
    AP_GROUPINFO("CRT_VOLT", 14, AP_BattMonitor_Params, _critical_voltage, 0),

    // @Param: CRT_MAH
    // @DisplayName: Battery critical capacity
    // @Description: Battery capacity at which the critical battery failsafe is triggered. Set to 0 to disable battery remaining failsafe. If the battery capacity drops below this level the vehicle will perform the failsafe specified by the @PREFIX@_FS_CRT_ACT parameter.
    // @Units: mAh
    // @Increment: 50
    // @User: Standard
    AP_GROUPINFO("CRT_MAH", 15, AP_BattMonitor_Params, _critical_capacity, 0),

    // @Param: FS_LOW_ACT
    // @DisplayName: Low battery failsafe action
    // @Description: What action the vehicle should perform if it hits a low battery failsafe
    // @Values{Plane}: 0:None,1:RTL,2:Land,3:Terminate
    // @Values{Copter}: 0:None,1:Land,2:RTL,3:SmartRTL,4:SmartRTL or Land,5:Terminate
    // @Values{Sub}: 0:None,2:Disarm,3:Enter surface mode
    // @Values{Rover}: 0:None,1:RTL,2:Hold,3:SmartRTL,4:SmartRTL or Hold,5:Terminate
    // @Values{Tracker}: 0:None
    // @User: Standard
    AP_GROUPINFO("FS_LOW_ACT", 16, AP_BattMonitor_Params, _failsafe_low_action, 0),

    // @Param: FS_CRT_ACT
    // @DisplayName: Critical battery failsafe action
    // @Description: What action the vehicle should perform if it hits a critical battery failsafe
    // @Values{Plane}: 0:None,1:RTL,2:Land,3:Terminate
    // @Values{Copter}: 0:None,1:Land,2:RTL,3:SmartRTL,4:SmartRTL or Land,5:Terminate
    // @Values{Sub}: 0:None,2:Disarm,3:Enter surface mode
    // @Values{Rover}: 0:None,1:RTL,2:Hold,3:SmartRTL,4:SmartRTL or Hold,5:Terminate
    // @Values{Tracker}: 0:None
    // @User: Standard
    AP_GROUPINFO("FS_CRT_ACT", 17, AP_BattMonitor_Params, _failsafe_critical_action, 0),

    AP_GROUPEND

};

AP_BattMonitor_Params::AP_BattMonitor_Params(void) {
    AP_Param::setup_object_defaults(this, var_info);
}