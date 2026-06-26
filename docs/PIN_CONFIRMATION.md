# ✅ Pin Configuration Confirmation

## Current Configuration (CONFIRMED ✓)

### Motor 1 (Left Front)
- **IN1** → Arduino D2
- **IN2** → Arduino D3
- **ENA** → Arduino D5 (PWM)
- **Output**: M1 (Out1, Out2 on L293D)

### Motor 2 (Right Front)
- **IN3** → Arduino D4
- **IN4** → Arduino D7
- **ENB** → Arduino D6 (PWM)
- **Output**: M2 (Out3, Out4 on L293D)

### Motor 3 (Left Rear)
- **IN1** → Arduino D8
- **IN2** → Arduino D9
- **ENA** → Arduino D10 (PWM)
- **Output**: M3 (Out1, Out2 on L293D Chip 2)

### Motor 4 (Right Rear)
- **IN3** → Arduino D11
- **IN4** → Arduino D12
- **ENB** → Arduino D13 (PWM)
- **Output**: M4 (Out3, Out4 on L293D Chip 2)

## L293D Pin Mapping

### For Each L293D Chip:

**Control Inputs (from Arduino):**
- **IN1** → Controls direction for Motor 1 (or Motor 3)
- **IN2** → Controls direction for Motor 1 (or Motor 3)
- **ENA** → Enables and controls speed (PWM) for Motor 1 (or Motor 3)
- **IN3** → Controls direction for Motor 2 (or Motor 4)
- **IN4** → Controls direction for Motor 2 (or Motor 4)
- **ENB** → Enables and controls speed (PWM) for Motor 2 (or Motor 4)

**Motor Outputs (to Motors):**
- **Out1** → Motor terminal 1 (M1 or M3)
- **Out2** → Motor terminal 2 (M1 or M3)
- **Out3** → Motor terminal 1 (M2 or M4)
- **Out4** → Motor terminal 2 (M2 or M4)

## Hardware Setup Summary

**2 L293D Chips Required:**
- **L293D Chip 1**: Controls Motor 1 and Motor 2
- **L293D Chip 2**: Controls Motor 3 and Motor 4

**Power Connections:**
- **VCC** (pin 16) → Arduino 5V (logic power)
- **VS** (pin 8) → External 5-12V (motor power)
- **GND** (pins 4, 5, 12, 13) → Common ground

## Code Verification

The code correctly maps:
- ✅ IN1, IN2, ENA for Motor 1
- ✅ IN3, IN4, ENB for Motor 2
- ✅ IN1, IN2, ENA for Motor 3 (second chip)
- ✅ IN3, IN4, ENB for Motor 4 (second chip)

**All configurations are CORRECT and ready to use!** ✓

## Quick Reference

| Motor | IN1 | IN2 | IN3 | IN4 | ENA | ENB | Arduino Pins |
|-------|-----|-----|-----|-----|-----|-----|--------------|
| M1    | ✓   | ✓   | -   | -   | ✓   | -   | D2, D3, D5   |
| M2    | -   | -   | ✓   | ✓   | -   | ✓   | D4, D7, D6   |
| M3    | ✓   | ✓   | -   | -   | ✓   | -   | D8, D9, D10  |
| M4    | -   | -   | ✓   | ✓   | -   | ✓   | D11, D12, D13|

