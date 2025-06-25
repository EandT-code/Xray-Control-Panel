##  Release Note – v0.4.5 
**Project Title:** Embedded X-Ray Control Panel – ESP32 GUI System  
**Release Date:** June 12, 2025  
**Release Version:** v0.4.5  
**Author:** Vedant Mishra

# New Feature: Dual MCP4725 DAC Control via Separate I2C Buses
**Description:**
- Successfully added support for two independent MCP4725 DACs using different SDA/SCL pins on the ESP32-S2.
- DAC 1 (MCP1) at address 0x60 connected to SDA: 20, SCL: 19.
- DAC 2 (MCP2) at address 0x61 connected to SDA: 2, SCL: 18.
- Used TwoWire class to define two separate I2C buses: I2C1 and I2C2.
- This allows precise and isolated voltage control over two analog channels in parallel.

--- 

# Purpose:
- Drive multiple analog outputs independently for controlling different hardware sections (e.g., filament, grid, or focus).
- Prevent I2C address conflict and allow flexible pin mapping on ESP32 platforms.
- Eliminate the need for additional I2C multiplexer hardware.

---

##  Technical Details:
- Created two TwoWire instances:

        TwoWire I2C1 = TwoWire(0);
        TwoWire I2C2 = TwoWire(1);

- Assigned custom SDA/SCL pins using .begin():

        I2C1.begin(20, 19);  // MCP1
        I2C2.begin(2, 18);   // MCP2

- Passed the I2C instance in the constructor:

        MCP4725 MCP1(0x60, &I2C1);
        MCP4725 MCP2(0x61, &I2C2);

---

### Tested On:
**Board:** ESP32-S2 Saola v1.2
**Library Used:** RobTillaart/MCP4725@0.4.0 (unmodified – supports TwoWire* in constructor)
**Framework:** PlatformIO + Arduino
**Wiring Verified:** Both DACs produce separate voltage outputs. SDA/SCL lines scoped for I2C stability.


























##  Release Note – v0.4.4 
**Project Title:** Embedded X-Ray Control Panel – ESP32 GUI System  
**Release Date:** June 12, 2025  
**Release Version:** v0.4.4  
**Author:** Vedant Mishra  

# New Feature: Adjustable kV Range and Duty Cycle Mapping
**Description:**
Modified the system to support a broader and more flexible kV range, and updated the logic to map the duty cycle (hv_dc) directly from selected kV.

--- 

# Purpose:
- Allow fine-grained control over kV values (0 to 50 kV).
- Make the system suitable for low-voltage testing and future calibration work.
- Simplify the driver logic by directly mapping selected kV to the HV duty cycle.

---

# Behavior:
- Maximum kV changed from 70 to 50.
- Minimum kV changed from 5 to 0.
- Step size changed from 5 to 1, enabling fine adjustments.
- Duty cycle (hv_dc) is now directly set as:
hv_dc = kv;

---

### Logic Used:
- Duty cycle for high-voltage generator is set using hv_dc.
- Assigned directly: hv_dc = kv, making it linearly proportional.
- No conversion needed – GUI-selected kV = actual duty cycle.
- Simplifies calibration, testing, and code maintenance.

---

### Tested On:
**Board:** ESP32-S2 Saola v1.2

**Display:** LVGL GUI System (via PlatformIO)

**Inputs:** kV selection tested via Button_kV_Pin

**Output Verification:** Correct response verilifed in CRO


















##  Release Note – v0.4.3  
**Project Title:** Embedded X-Ray Control Panel – ESP32 GUI System  
**Release Date:** June 12, 2025  
**Release Version:** v0.4.3  
**Author:** Vedant Mishra  

---

###  New Feature: Display “Ready for Exposure” Every 15 Seconds

**Description:**  
Added functionality to automatically show the status `"Ready for Exposure"` after 15 seconds once the READY button is pressed and countdown begins.

---

###  Purpose:
- Reinforce system readiness to the user at fixed intervals.
- Provide periodic GUI feedback, especially during extended idle times before exposure.
- Improve usability by clearly indicating system readiness without requiring user interaction.

---

###  Behavior:
- After pressing the READY button, the system starts a second-wise countdown.
- Every time the total seconds reach a  15s,the GUI label displays:
Ready for Exposure


- Between these intervals, normal countdown messages like `"READY 1s"`, `"READY 2s"` continue to display.

---

###  Logic Used:

- Uses millis() to track time since READY was pressed.
- Checks if 15 seconds have passed: millis() - readyStartTime >= 15000.
- If true, shows: "Ready for Expose" on GUI.
- Does not stop or return — continues to exposure logic.
- Ensures exposure is only allowed after 15 sec, with smooth flow.

---


### Tested On:
**Board:** ESP32-S2 Saola v1.2

**Display:** LVGL GUI System (via PlatformIO)

**Inputs:** Simulated button press via digitalRead() logic


 **Output Verification:** GUI messages observed live on the physical display.





















## Release Note – v0.4.2
**Project Title**: Embedded X-Ray Control Panel – ESP32 GUI System  
**Release Date**: June 12, 2025  
**Author**: Vedant Mishra  

---

###  New Feature Added: Exposure Blocked for First 15 Seconds

After pressing the READY button, the system now **blocks the EXPOSURE button** for the first 15 seconds to ensure a safe warm-up time.

---

###  Purpose

- Prevent accidental or premature exposure
- Ensure system stabilization before high-voltage operation
- Enforce proper warm-up cycle

---

###  Technical Behavior

- A 15-second timer begins **after pressing the READY button**
- If the EXPOSURE button is pressed **before 15 seconds**:
  - GUI displays: `"Wait for 15 sec"`
  - Exposure logic is **aborted immediately**
  - Serial output logs the message: `"Exposure blocked. Wait for 15 sec."`

---

###  Logic Snippet
- When the user presses READY, we store the current millis() in readyStartTime.
- Every time the user tries to expose, we check if 15 seconds have passed.
- If not, we block exposure by showing a message and exiting the function using return.

---

### Affected Modules
- main.cpp / exposure_control.cpp
- ui_Labelstatus GUI component
- Serial debug log

---

### Tested On
**Board:** ESP32-S2 Saola v1.2

**GUI Framework:** LVGL via PlatformIO

**Buttons:** Simulated using physical inputs

**Output Verification:** GUI warning observed live on the physical display if Exposure Button is Pressed before 15 sec.

---

### Next Planned Update (v0.4.3)
Automatically show "Ready for Exposure" after 15 seconds when Ready Button is Pressed













# Release Note – v0.4.1  
**Project Title:** Embedded X-Ray Control Panel – ESP32 GUI System  
**Release Date:** June 12, 2025  
**Author:** Vedant Mishra  

---

##  New Feature: Countdown Timer on READY Button Press

###  Description:
Introduced a **15-second countdown** that starts **only when the READY button is pressed**.

- Displays dynamic messages like `"READY 1s"`, `"READY 2s"` …  on the GUI (`ui_Labelstatus`), updating **every second**.

This feature enhances user awareness and adds a safety delay before enabling the exposure functionality.

---

##  Purpose:
- Prevent premature or unsafe X-ray exposure by enforcing a minimum **warm-up period**.
- Provide clear and continuous **real-time status feedback** on the GUI.
- Prepare the system's internal state for smooth transitions into the **exposure** and **cooldown** phases.

---

##  Technical Details:
- Countdown logic implemented using `millis()` to track real time.
- Triggers GUI updates at **1-second intervals** 
- readySeconds variable keeps track of elapsed time.
- Serial debug prints help monitor status in the console.

---

### Tested On:
**Board:** ESP32-S2 Saola v1.2

**Framework:** LVGL GUI Library using PlatformIO

**Inputs:** Simulated button presses (READY) to trigger the countdown sequence.

**Output Verification:** GUI Message observed live on the physical display updating every second till the device is on.

---

### Next Planned Update (v0.4.2):
- Block exposure if the user tries to trigger it before 15 seconds.
- Show "Wait for 15 sec" message during this period.
- Allow exposure only after the countdown completes.

