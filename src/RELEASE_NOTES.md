Release Note – v0.1.0
Project Title: Embedded X-Ray Control Panel – ESP32 GUI System
Release Date: June 12, 2025
Author: Vedant Mishra

New Feature Added
Countdown Timer on READY Button Press:

Introduced a 15-second countdown that starts only when the READY button is pressed.

Displays "READY 1s", "READY 2s" … up to "READY 15s" on the GUI (ui_Labelstatus) updated every second.

After 15 seconds, the GUI shows the message: "Ready for Exposure".

This ensures users wait for a warm-up period before initiating exposure.

Purpose :-
Prevent premature exposure activation by enforcing a safe warm-up period.

Enhance clarity of system state with continuous live feedback on the GUI.

Prepare system status handling for future exposure and cooldown phases.

Technical Details :-
Timer logic based on millis():

Triggers GUI update every 1000 ms.

Uses readySeconds to track time elapsed.

GUI updates done using:


lv_label_set_text(ui_Labelstatus, readyText.c_str());
lv_timer_handler();
Serial debug prints added to monitor countdown in console.

Tested On :-
ESP32-S2 Saola v1.2 board

LVGL GUI library via PlatformIO

Basic button input simulation

Next Planned Update :-
Enable exposure logic only after "Ready for Exposure" is shown.

Add cooldown timer after exposure phase.

