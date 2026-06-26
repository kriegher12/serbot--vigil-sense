# L293D Motor Driver Pin Configuration Reference

## L293D Chip Overview
The L293D is a dual H-bridge motor driver that can control **2 DC motors** with one chip.

### For 4 Motors, you need:
- **Option 1**: 2 L293D chips (recommended)
- **Option 2**: 1 L293D + another motor driver

## Single L293D Pin Layout (for 2 motors)

### Control Inputs (from Arduino)
- **IN1** → Motor 1 direction control (pin 2)
- **IN2** → Motor 1 direction control (pin 7)
- **ENA** → Motor 1 enable/speed (PWM pin 1)
- **IN3** → Motor 2 direction control (pin 10)
- **IN4** → Motor 2 direction control (pin 15)
- **ENB** → Motor 2 enable/speed (PWM pin 9)

### Motor Outputs (to Motors)
- **Out1** → Motor 1 terminal 1
- **Out2** → Motor 1 terminal 2
- **Out3** → Motor 2 terminal 1
- **Out4** → Motor 2 terminal 2

### Power Pins
- **VCC** (pin 16) → Arduino 5V (logic power)
- **VS** (pin 8) → External 5-12V (motor power)
- **GND** (pins 4, 5, 12, 13) → Ground

## Current Code Configuration (4 Motors)

The code assumes **2 L293D chips** or a motor driver shield:

### Motor 1 (Left Front)
- **IN1** → Arduino D2
- **IN2** → Arduino D3
- **ENA** → Arduino D5 (PWM)

### Motor 2 (Right Front)
- **IN3** → Arduino D4
- **IN4** → Arduino D7
- **ENB** → Arduino D6 (PWM)

### Motor 3 (Left Rear)
- **IN1** → Arduino D8
- **IN2** → Arduino D9
- **ENA** → Arduino D10 (PWM)

### Motor 4 (Right Rear)
- **IN3** → Arduino D11
- **IN4** → Arduino D12
- **ENB** → Arduino D13 (PWM)

## Wiring Diagram

```
Arduino          L293D Chip 1          Motor 1
D2 (IN1)  ──────> IN1 (pin 2)          
D3 (IN2)  ──────> IN2 (pin 7)          
D5 (ENA)  ──────> ENA (pin 1)  ──────> Out1 ────> Motor 1
                                          Out2 ────> Motor 1

D4 (IN3)  ──────> IN3 (pin 10)         
D7 (IN4)  ──────> IN4 (pin 15)         
D6 (ENB)  ──────> ENB (pin 9)  ──────> Out3 ────> Motor 2
                                          Out4 ────> Motor 2

Arduino          L293D Chip 2          Motor 3
D8 (IN1)  ──────> IN1 (pin 2)          
D9 (IN2)  ──────> IN2 (pin 7)          
D10(ENA)  ──────> ENA (pin 1)  ──────> Out1 ────> Motor 3
                                          Out2 ────> Motor 3

D11(IN3)  ──────> IN3 (pin 10)         
D12(IN4)  ──────> IN4 (pin 15)         
D13(ENB)  ──────> ENB (pin 9)  ──────> Out3 ────> Motor 4
                                          Out4 ────> Motor 4
```

## Motor Direction Logic

### Forward (Motor 1 example):
- IN1 = HIGH, IN2 = LOW → Motor rotates forward
- ENA = PWM value (speed control)

### Backward (Motor 1 example):
- IN1 = LOW, IN2 = HIGH → Motor rotates backward
- ENA = PWM value (speed control)

### Stop:
- ENA = 0 (or both IN1 and IN2 same state)

## Important Notes

1. **Power Supply**: 
   - Use separate power supply for VS (motor power) if motors are high current
   - Connect VS to 5-12V depending on motor rating
   - Connect all GNDs together (common ground)

2. **PWM Pins**: 
   - ENA and ENB must be PWM-capable pins (marked with ~ on Arduino)
   - Speed is controlled by PWM value (0-255)

3. **Current Rating**:
   - L293D can handle up to 600mA per channel
   - For higher current motors, use external drivers or higher-rated chips

4. **Heat Sinking**:
   - L293D can get hot under load
   - Consider adding a heat sink for continuous operation

## Verification Checklist

- [ ] All IN pins connected to Arduino digital pins
- [ ] All ENA/ENB pins connected to Arduino PWM pins
- [ ] Motor outputs (Out1-4) connected to motors
- [ ] VCC connected to Arduino 5V
- [ ] VS connected to motor power supply (5-12V)
- [ ] All GNDs connected together
- [ ] Power supply rated for motor current
- [ ] Capacitor (100µF) between VS and GND (recommended)

## Troubleshooting

**Motor not moving:**
- Check ENA/ENB pins are set to PWM (not 0)
- Verify motor connections to Out pins
- Check power supply to VS pin

**Motor running in wrong direction:**
- Swap Out1/Out2 connections
- Or swap IN1/IN2 logic in code

**Motors running but weak:**
- Check VS voltage (should match motor rating)
- Verify power supply can provide enough current
- Check for loose connections

