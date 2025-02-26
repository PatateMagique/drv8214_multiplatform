/*
 * Copyright (c) 2025 Théo Heng
 *
 * This file is part of the drv8214_multiplatform library.
 *
 * Licensed under the MIT License. See the LICENSE file in the project root for full license information.
 */

#ifndef DRV8214_H
#define DRV8214_H

#include "Platform.h"

#ifdef DRV8214_PLATFORM_ARDUINO
    #include <Arduino.h>
    #include "I2C.h"
#endif

#ifdef DRV8214_PLATFORM_STM32
    #include "stm32fxxx_hal.h"  // Replace xx with your actual MCU family
    #include "stm32_hal.h"      // This should make the process automatic
    #include "i2c.h"           // Generated by STM32CubeMX for I2C initialization
#endif

// I2C Address (depends on A0, A1 pin settings)
#define DRV8214_I2C_ADDR_00  0x60  // A1 = 0, A0 = 0 
#define DRV8214_I2C_ADDR_0Z  0x62  // A1 = 0, A0 = High-Z
#define DRV8214_I2C_ADDR_01  0x64  // A1 = 0, A0 = 1
#define DRV8214_I2C_ADDR_Z0  0x66  // A1 = High-Z, A0 = 0
#define DRV8214_I2C_ADDR_ZZ  0x68  // A1 = High-Z, A0 = High-Z
#define DRV8214_I2C_ADDR_Z1  0x6A  // A1 = High-Z, A0 = 1
#define DRV8214_I2C_ADDR_10  0x6C  // A1 = 1, A0 = 0
#define DRV8214_I2C_ADDR_1Z  0x6E  // A1 = 1, A0 = High-Z
#define DRV8214_I2C_ADDR_11  0x70  // A1 = 1, A0 = 1

// --- STATUS REGISTERS (Read-Only) ---
#define DRV8214_FAULT        0x00  // Fault Status Register 
#define DRV8214_RC_STATUS1   0x01  // Motor speed estimated by the ripple counting algorithm. 
#define DRV8214_RC_STATUS2   0x02  // Lower half 8-bit output out of the 16-bit output of the ripple counter corresponding to the number of current ripples.
#define DRV8214_RC_STATUS3   0x03  // Upper half 8-bit output out of the 16-bit output of the ripple counter corresponding to the number of current ripples.
#define DRV8214_REG_STATUS1  0x04  // Outputs the voltage across the motor terminals, maximum value FFh. 00h corresponds to 0 V and B0h corresponds to 11 V.
#define DRV8214_REG_STATUS2  0x05  // Outputs the current flowing through the motor. 00h corresponds to 0 A and C0h corresponds to the maximum value set by the CS_GAIN_SEL bits.
#define DRV8214_REG_STATUS3  0x06  // Bridge control duty cycle generated by an internal regulation logic. The range of duty cycle is 0% (000000b) to 100% (111111b)

// --- CONFIGURATION REGISTERS (Read/Write) ---
#define DRV8214_CONFIG0      0x09  // General Configuration Register
#define DRV8214_CONFIG1      0x0A  // Inrush Time Low Byte - Sets the amount of time for which the stall detection scheme ignores motor inrush current.
#define DRV8214_CONFIG2      0x0B  // Inrush Time High Byte - Sets the amount of time for which the stall detection scheme ignores motor inrush current.
#define DRV8214_CONFIG3      0x0C  // Current Regulation, Stall Detection, and Protection 
#define DRV8214_CONFIG4      0x0D  // Control Mode and I2C Settings 
#define DRV8214_REG_CTRL0    0x0E  // Control Register 0: Controls soft start, PWM frequency, and scaling
#define DRV8214_REG_CTRL1    0x0F  // Control Register 1: Sets output voltage threshold: 00h corresponds to 0 rad/s and FFh corresponds to the maximum speed allowable by W_SCALE
#define DRV8214_REG_CTRL2    0x10  // Control Register 2: External duty cycle and output filter settings
#define DRV8214_RC_CTRL0     0x11  // Ripple Count Control 0: Enables ripple counting, error correction, and other settings
#define DRV8214_RC_CTRL1     0x12  // Ripple Count Control 1: Sets ripple count threshold
#define DRV8214_RC_CTRL2     0x13  // Ripple Count Control 2: Inversion scale, threshold scaling, and additional parameters
#define DRV8214_RC_CTRL3     0x14  // Ripple Count Control 3: Inversion settings
#define DRV8214_RC_CTRL4     0x15  // Ripple Count Control 4: KMC configuration
#define DRV8214_RC_CTRL5     0x16  // Ripple Count Control 5: Filter coefficient (FLT_K)
#define DRV8214_RC_CTRL6     0x17  // Ripple Count Control 6: Mechanical fault settings, error correction pulse disable
#define DRV8214_RC_CTRL7     0x18  // Ripple Count Control 7: Proportional gain divisor for control loop
#define DRV8214_RC_CTRL8     0x19  // Ripple Count Control 8: Integral gain divisor for control loop

// --- BIT MASKS FOR CONTROL REGISTERS ---

// FAULT REGISTER (0x00) - Read Only
#define FAULT_FAULT       0x80  // Bit 7 - General fault detected - nFAULT pin is pulled down when FAULT bit is set
#define FAULT_RSVD        0x40  // Bit 6 - Reserved
#define FAULT_STALL       0x20  // Bit 5 - Stall detected (active high)
#define FAULT_OCP         0x10  // Bit 4 - Overcurrent protection triggered (active high)
#define FAULT_OVP         0x08  // Bit 3 - Overvoltage protection triggered (active high)
#define FAULT_TSD         0x04  // Bit 2 - Thermal shutdown triggered (active high)
#define FAULT_NPOR        0x02  // Bit 1 - Power-on reset occurred 
#define FAULT_CNT_DONE    0x01  // Bit 0 - Set to 1 when RC_CNT exceeds the ripple counting threshold - can be cleared by CLR_CNT command

// REG STATUS REGISTER3 (0x06) - Read Only
#define REG_STATUS3_RSVD       0xC0  // Bits 7-6 - Reserved
#define REG_STATUS3_IN_DUTY    0x3F  // Bit 0-5 - Bridge control duty cycle (6-bit)

// CONFIG0 REGISTER (0x09) - Read/Write
#define CONFIG0_EN_OUT       0x80  // Bit 7 - 0b: All driver FETs are Hi-Z. 1b: Enables the driver outputs.
#define CONFIG0_EN_OVP       0x40  // Bit 6 - Enable overvoltage protection, 1b by default
#define CONFIG0_EN_STALL     0x20  // Bit 5 - Enable stall detection
#define CONFIG0_VSNS_SEL     0x10  // Bit 4 - Voltage sensing selection ob is recommanded(*)
#define CONFIG0_VM_GAIN_SEL  0x08  // Bit 3 - Motor voltage gain selection 0b: Voltage range is 0V-15.7V - 1b: Voltage range is 0V-3.92V (*)
#define CONFIG0_CLR_CNT      0x04  // Bit 2 - Resets the ripple counter to 0, and resets CNT_DONE. Also releases nFAULT when RC_REP = 10b. CLR_CNT is automatically reset.
#define CONFIG0_CLR_FLT      0x02  // Bit 1 - Clear all fault flags, automatically reset after read
#define CONFIG0_DUTY_CTRL    0x01  // Bit 0 - Enable duty cycle control mode (*)

// CONFIG3 REGISTER (0x0C) - Read/Write
#define CONFIG3_IMODE        0xC0  // Bits 7-6 - Current regulation mode [1:0] (*)
#define CONFIG3_SMODE        0x20  // Bit 5 - Stall mode setting  (*)
#define CONFIG3_INT_VREF     0x10  // Bit 4 - Internal voltage reference enable 1b: sets VREF voltage to 500mV internally - 0b: Voltage is not fixed (*)
#define CONFIG3_TBLANK       0x08  // Bit 3 - Current blanking time selection 0b, tBLANK=1.8µs - tBLANK=1.0µs (*)
#define CONFIG3_TDEG         0x04  // Bit 2 - Deglitch time selection 0b, tDEG=2µs - 1b, tDEG=1µs (*)
#define CONFIG3_OCP_MODE     0x02  // Bit 1 - Overcurrent protection mode 0b: device is latched off (Can be cleared using CLR_FLT) 1b: auto-retry (*)
#define CONFIG3_TSD_MODE     0x01  // Bit 0 - Thermal shutdown mode (*)

// CONFIG4 REGISTER (0x0D) - Read/Write
#define CONFIG4_RC_REP       0xC0  // Bits 7-6 - Ripple count reporting mode [1:0]
#define CONFIG4_STALL_REP    0x20  // Bit 5 - Stall reporting enable 0b: stall is not reported on nFAULT output - 1b: nFAULT is pulled low when stall is detected
#define CONFIG4_CBC_REP      0x10  // Bit 4 - Cycle-by-cycle current regulation reporting
#define CONFIG4_PMODE        0x08  // Bit 3 - 0b: PH/EN - 1b: PWM (*)
#define CONFIG4_I2C_BC       0x04  // Bit 2 - Decides the H-Bridge Control Interface. 0b: Bridge control configured by INx pins. 1b: Bridge control configured by I2C bits I2C_EN_IN1 and I2C_PH_IN2. (*)
#define CONFIG4_I2C_EN_IN1   0x02  // Bit 1 - I2C enable for IN1
#define CONFIG4_I2C_PH_IN2   0x01  // Bit 0 - I2C enable for IN2

// REG_CTRL0 (0x0E) Bit Masks - Read/Write
#define REG_CTRL0_RSVD       0xC0  // Bits 7-6 - Reserved
#define REG_CTRL0_EN_SS      0x20  // Bit 5 - 0b: Soft-Start/Stop disabled - 1b: Soft-Start/Stop enabled
#define REG_CTRL0_REG_CTRL   0x18  // Bits 4-3 - Regulation Control Mode [1:0] (*)
#define REG_CTRL0_PWM_FREQ   0x04  // Bit 2 - PWM Frequency Selection 0b: 50kHz - 1b: 25kHz
#define REG_CTRL0_W_SCALE    0x03  // Bits 1-0 - Scaling Factor for PWM Width [1:0]

// REG_CTRL2 (0x10) Bit Masks - Read/Write
#define REG_CTRL2_OUT_FLT     0xC0  // Bits 7-6 - Output Filter Mode [1:0]
#define REG_CTRL2_EXT_DUTY    0x3F  // Bits 5-0 - External Duty Cycle Control [5:0]

// RC_CTRL0 (0x11) Bit Masks - Read/Write
#define RC_CTRL0_EN_RC        0x80  // Bit 7 - Enable Ripple Counting (Active High)
#define RC_CTRL0_DIS_EC       0x40  // Bit 6 - Disable Error Correction (Active High)
#define RC_CTRL0_RC_HIZ       0x20  // Bit 5 - 0b: H-bridge stays enabled when RC_CNT exceeds threshold - 1b: H-bridge is disabled (High-Z) when RC_CNT exceeds threshold
#define RC_CTRL0_FLT_GAIN_SEL 0x18  // Bits 4-3 - Filter Gain Selection [1:0]
#define RC_CTRL0_CS_GAIN_SEL  0x07  // Bits 2-0 - Current Sense Gain Selection [2:0]

// RC_CTRL2 (0x13) Bit Masks - Read/Write
#define RC_CTRL2_INV_R_SCALE  0xC0  // Bits 7-6 - Inversion Scale Factor [1:0]
#define RC_CTRL2_KMC_SCALE    0x30  // Bits 5-4 - KMC Scaling Factor
#define RC_CTRL2_RC_THR_SCALE 0x18  // Bits 3-2 - Ripple Count Threshold Scaling [1:0]
#define RC_CTRL2_RC_THR_HIGH  0x03  // Bits 1-0 - Ripple Count Threshold [9:8]

// RC_CTRL5 (0x16) Bit Masks - Read/Write
#define RC_CTRL5_FLT_K        0xF0  // Bits 7-4 - Filter Coefficient for Ripple Count [3:0]
#define RC_CTRL5_FLT_RSVD     0x0F  // Bits 3-0 - Reserved

// RC_CTRL6 (0x17) Bit Masks - Read/Write
#define RC_CTRL6_EC_PULSE_DIS 0x80  // Bit 7 - Disable Error Correction Pulse
#define RC_CTRL6_T_MECH_FLT   0x70  // Bits 6-4 - Mechanical Fault Detection
#define RC_CTRL6_EC_FALSE_PER 0x0C  // Bits 3-2 - Error Correction False Period
#define RC_CTRL6_EC_MISS_PER  0x03  // Bits 1-0 - Error Correction Miss Period

// RC_CTRL7 (0x18) Bit Masks - Read/Write
#define RC_CTRL7_KP_DIV       0xE0  // Bits 7-5 - Proportional Gain Divisor for Control Loop [2:0]
#define RC_CTRL7_KP           0x1F  // Bits 4-0 - Proportional Gain Value [4:0]

// RC_CTRL8 (0x19) Bit Masks - Read/Write
#define RC_CTRL8_KI_DIV       0xE0  // Bits 7-5 - Integral Gain Divisor for Control Loop [2:0]
#define RC_CTRL8_KI           0x1F  // Bits 4-0 - Integral Gain Value [4:0]

enum ControlMode { PWM, PH_EN };
enum RegulationMode { CURRENT_FIXED, CURRENT_CYCLES, SPEED, VOLTAGE };
// using I2C control, the speed/voltage cannot be controlled if using the CURRENT_FIXED or CURRENT_CYCLES regulation mode

struct DRV8214_Config {
    bool I2CControlled = true;  // I2C Control of the driver (0: disabled, 1: enabled)
    ControlMode control_mode = PWM;  // Control mode of the driver (PWM, PH_EN)
    RegulationMode regulation_mode = SPEED;  // Control mode of the driver (CURRENT_FIXED, CURRENT_CYCLES, SPEED, VOLTAGE)
    bool voltage_range = true;  // Expected applied supply voltage range to the attached motor (0: 0V-15.7V, 1: 0V-3.92V)
    float Vref = 0.5f;  // Voltage reference for current regulation. Can be internal (fixed @500mV) or external.
    bool stall_enabled = true;  // Stall detection (0: disabled, 1: enabled)
    bool ovp_enabled = true;  // Overvoltage protection (0: disabled, 1: enabled)
    bool stall_behavior = false;  // Stall behavior of the driver (0: outputs disable, 1: outputs continue to drive current)
    bool bridge_behavior_thr_reached = false;  // Bridge behavior when ripple threshold is reached (0: H-bridge stays enabled, 1: H-bridge is disabled)
    uint8_t current_reg_mode = 0;  // Stall mode of the driver (0: no current regulation, 1: current regulation during inrush, 2: current regulation at all times, 3: current regulation at all times)
    uint8_t Aipropri = 0;  // Value of the current mirror gain in μA/A
    float Itrip = 0.0f;  // Value of the trip current in A (current target for regulation mode CURRENT_FIXED & CURRENT_CYCLES)
    bool verbose = false;  // Print information about the driver configuration
};

class DRV8214 {

    private:
        // Hardware and driver-specific settings
        uint8_t address;     // I2C address of the driver (depends on A0, A1 pin settings, 9 possible addresses)
        uint8_t driver_ID;   // ID of the driver if multiple drivers are used
        uint8_t Ripropri;    // Value in Ohms of the resistor connected to IPROPI pin
        uint8_t ripples_per_revolution;  // Number of ripples per revolution

        // Configuration settings, all in a single struct
        DRV8214_Config config;

    public:
        // Constructor
        DRV8214(uint8_t addr, uint8_t id, uint8_t sense_resistor, uint8_t ripples) : address(addr), driver_ID(id), Ripropri(sense_resistor), ripples_per_revolution(ripples) {}
    
        // Initialization
        void init(const DRV8214_Config& config);

        // --- Helper Functions ---
        uint8_t getDriverAdress();
        uint8_t getDriverID();
        uint8_t getSenseResistor();
        uint8_t getRipplesPerRevolution();
        uint8_t getFaultStatus();
        uint8_t getMotorSpeed();
        uint16_t getRippleCount();
        uint8_t getMotorCurrent();
        uint8_t getMotorVoltage();
        uint8_t getDutyCycle();

        // --- Control Functions ---
        void enableHbridge();
        void disableHbridge();
        void setStallDetection(bool stall_en);
        void setVoltageRange(bool range);
        void setOvervoltageProtection(bool OVP);
        void resetRippleCounter();
        void resetFaultFlags();
        void enableDutyCycleControl();
        void disableDutyCycleControl();

        void setInrushDuration(uint16_t threshold);
        void setCurrentRegMode(uint8_t mode);
        void setStallBehavior(bool behavior);
        void setInternalVoltageReference(float reference_voltage);
        void configureConfig3(uint8_t config3);

        void setI2CControl(bool I2CControl);
        void enablePWMControl();
        void enablePHENControl();

        void enableStallInterrupt();
        void disableStallInterrupt();
        void enableCountThresholdInterrupt();
        void disableCountThresholdInterrupt();

        void setBridgeBehaviorThresholdReached(bool stops);
        void configureControl0(uint8_t control0);
        void setRippleSpeed(uint8_t speed);
        void setVoltageSpeed(float voltage);
        void setRegulationAndStallCurrent(float requested_current);
        void configureControl2(uint8_t control2);
        void enableRippleCount();
        void configureRippleCount0(uint8_t ripple0);
        void setRippleCountThreshold(uint16_t threshold);
        void configureRippleCount2(uint8_t ripple2);
        void setMotorInverseResistance(uint8_t resistance);
        void setKMCScalingFactor(uint8_t factor);
        void setFilterDamping(uint8_t damping);
        void configureRippleCount6(uint8_t ripple6);
        void configureRippleCount7(uint8_t ripple7);
        void configureRippleCount8(uint8_t ripple8);

        // --- Motor Control Functions ---
        void setControlMode(ControlMode mode, bool I2CControl);
        void setRegulationMode(RegulationMode regulation);
        void turnForward(uint8_t speed = 0, float voltage = 0, float requested_current = 0);
        void turnReverse(uint8_t speed = 0, float voltage = 0, float requested_current = 0);
        void brakeMotor();
        void coastMotor();
        void turnXRipples(uint8_t ripples_target, bool stops = true, bool direction = true, uint8_t speed = 0, float voltage = 0, float current = 0);
        void turnXRevolutions(uint8_t revolutions_target, bool stops = true, bool direction = true, uint8_t speed = 0, float voltage = 0, float current = 0);

        // --- Other Functions ---
        void printMotorConfig(bool initial_config = false);
        void drvPrint(const char* message);
};

#endif // DRV8214_H