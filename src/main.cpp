
#include "ui.h"					 // Define UI header
#include <Arduino_GFX_Library.h> // Define Arduino GFX library header
#include "pins_info.h"			 // Define Pin define header
#include <ezButton.h>			 // Define buttons hearder
#include "MCP4725.h"			 // MCP4725 library
#include "Wire.h"				 //I2C bus for MCP

MCP4725 MCP(0x60); // Initializing MCP Library with address bus 0x60

// IO Pin Assign - Digital OUTPUTS

// Set these values in library files for esp32 s2 segment
// Display Pin Setting
#define DF_GFX_RST_CUST 9				 // SPI RESET Pin
#define DF_GFX_CS_CUST 10				 // SPI CS Pin, Chip Select,	Selects the SPI device to communicate with
#define DF_GFX_MOSI_CUST 11				 // SPI MOSI Pin; Master Out, Slave In;	Data sent from ESP32 (Master) to the display (Slave)
#define DF_GFX_SCK_CUST 12				 // SPI SCK Pin ,Serial Clock, Synchronizes data transfer
#define DF_GFX_MISO_CUST GFX_NOT_DEFINED // SPI MISO not required as touch is not being used. Master In, Slave Out,	Data sent from the display (Slave) to ESP32 (Master) (Not used in this case)
#define DF_GFX_DC_CUST 14				 // SPI Command Pin ,Data/Command,	Distinguishes data from command signals
#define GFX_BL_CUST GFX_NOT_DEFINED		 // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

// Check of display dev kit is defined
#if defined(DISPLAY_DEV_KIT)
Arduino_GFX *gfx = create_default_Arduino_GFX();
#else /*!defined(DISPLAY_DEV_KIT) */

// Configure an SPI bus for display
Arduino_DataBus *bus = new Arduino_ESP32SPI(DF_GFX_DC_CUST, DF_GFX_CS_CUST, DF_GFX_SCK_CUST, DF_GFX_MOSI_CUST, DF_GFX_MISO_CUST);
// Arduino_GFX *gfx = new Arduino_ILI9341(bus, DF_GFX_RST_CUST, 3 /*rotation */, false /*IPS */); // LCD bus, rotation &IPS library definition
Arduino_GFX *gfx = new Arduino_ILI9341(bus, DF_GFX_RST_CUST, 3 /*rotation */, false /*IPS */); // Initialize the display driver
#endif

// AP ID and Password
//  const char* ssid = "Iphone";
//  const char* password = "12344321";

// IPAddress staticIP(192, 168, 68, 11);
// IPAddress gateway(192, 168, 68, 1);   // Replace this with your gateway IP Addess
// IPAddress subnet(255, 255, 0, 0);  // Replace this with your Subnet Mask
// IPAddress dns(0, 0, 0, 0);   // Replace this with your DNS

/*Change to your screen resolution */
static uint32_t screenWidth = 240;
static uint32_t screenHeight = 320;
static lv_disp_draw_buf_t draw_buf; // Allocated a drawing buffer for LVGL rendering
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv; // Defines a display driver structure

/*Button setup*/
ezButton mAs_Button(Button_mAs_Pin);		   // Up Button Pin definition
ezButton kV_Button(Button_kV_Pin);			   // Down Button Pin definition
ezButton READY_Button(Button_Ready_Pin);	   // Ready for Exposure Button Pin definition
ezButton EXPOSURE_Button(Button_Exposure_Pin); // Exposure button Pin definition
ezButton LEFT_BUTTON(Button_LEFT_Pin);		   // LEFT Button pin definition
ezButton RIGHT_BUTTON(Button_RIGHT_Pin);	   // RIGHT Button pin definition
ezButton POWER_BUTTON(Button_POWER_Pin);	   // SHUTDOWN Button pin definition

/*Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
	uint32_t w = (area->x2 - area->x1 + 1);
	uint32_t h = (area->y2 - area->y1 + 1);
	gfx->invertDisplay(1);
	if (LV_COLOR_16_SWAP != 0)
		gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
	else
		gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
	lv_disp_flush_ready(disp);
}

// Hide all the UI features
void setup_initial_display() // Display startup initial state definition
{
	lv_obj_add_flag(ui_radiationDanger, LV_OBJ_FLAG_HIDDEN); // Display radiation symbol
	lv_obj_add_flag(ui_eXPOSURE, LV_OBJ_FLAG_HIDDEN);		 // Display Danger symbol
	lv_obj_add_flag(ui_CoolDown, LV_OBJ_FLAG_HIDDEN);		 // Display cooldown symbol
	lv_label_set_text(ui_labelTimer, String(mAs).c_str());	 // Display Timer
	// 		String ss = String(kvInitial);
	// 		String sj = ss + "kV";
	String mA_string = String(mA) + "mA";
	String kV_string = String(kv) + "kV";
	lv_label_set_text(ui_labelCurrent, String(mA_string).c_str());
	lv_label_set_text(ui_labelKv, String(kV_string).c_str());
}

void setup_display() // Display UI configuration setup
{
#ifdef GFX_EXTRA_PRE_INIT
	GFX_EXTRA_PRE_INIT();
#endif

	// Init Display
	gfx->begin();
	gfx->fillScreen(BLACK);

#ifdef GFX_BL
	pinMode(GFX_BL, OUTPUT);
	digitalWrite(GFX_BL, HIGH);
#endif

	lv_init();
	screenWidth = gfx->width();
	screenHeight = gfx->height();

#ifdef ESP32
	disp_draw_buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * screenWidth * 32, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	disp_draw_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * screenWidth * 32);

#endif
	if (!disp_draw_buf)
	{
		Serial.println("LVGL disp_draw_buf allocate failed!");
	}
	else
	{
		lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * 32);

		/*Initialize the display */
		lv_disp_drv_init(&disp_drv);
		/*Change the following line to your display resolution */
		disp_drv.hor_res = 320;
		disp_drv.ver_res = 240;
		disp_drv.flush_cb = my_disp_flush;
		disp_drv.draw_buf = &draw_buf;
		lv_disp_drv_register(&disp_drv);

		/*Initialize the (dummy) input device driver */
		static lv_indev_drv_t indev_drv;
		lv_indev_drv_init(&indev_drv);
		indev_drv.type = LV_INDEV_TYPE_POINTER;
		lv_indev_drv_register(&indev_drv);

		ui_init();

		Serial.println("Setup done");
	}
}

// assign all Enable pins as output
void setup_output_pins()
{
	pinMode(LED, OUTPUT);
	pinMode(LASER_EN, OUTPUT);
	pinMode(PRECHARGE_EN, OUTPUT);
	pinMode(DAC_LS_EN, OUTPUT);
	pinMode(LDO_3V3_EN, OUTPUT);
	pinMode(EXPOSURE_EN, OUTPUT);
	pinMode(SHUTDOWN_EN, OUTPUT);
	pinMode(LDO_5V_EN, OUTPUT);
	pinMode(FIL_LS_EN, OUTPUT);
	pinMode(HV_LS_EN, OUTPUT);
}

void set_outputPin()
{
	digitalWrite(LED, HIGH); // ON-Board LED turns ON
	// digitalWrite(DAC_LS_EN, (READY || EXPOSURE)); 					// Pull the Level shifter Enable HIGH enabling the voltage translator to the DAC
	digitalWrite(DAC_LS_EN, (HIGH));			  // Pull the Level shifter Enable HIGH enabling the voltage translator to the DAC
	digitalWrite(LASER_EN, !(READY || EXPOSURE)); // Pull Laser Enable Pin HIGH to turn off the Laser
	// digitalWrite(PRECHARGE_EN, (READY && EXPOSURE)); 	// Pull Precharge Enable HIGH if ready mode is ON and exposure mode is on
	digitalWrite(PRECHARGE_EN, (READY || EXPOSURE));		   // Pull Precharge Enable HIGH if ready mode is ON and exposure mode is on
	digitalWrite(LDO_3V3_EN, HIGH);							   // LDO turn on for 3V3 supply
	digitalWrite(LDO_5V_EN, (HIGH));						   // Pull 5V LDO Enable Pin HIGH
	digitalWrite(EXPOSURE_EN, (READY && EXPOSURE));			   // Pull Exposure Enable Pin HIGH if ready and exposure both are true
	digitalWrite(FIL_LS_EN, !(READY || EXPOSURE || COOLDOWN)); // Pull Filament Level Shifter Enable HIGH in ready or exposure states
	digitalWrite(HV_LS_EN, !(READY && EXPOSURE));			   // Pull HV Level Shifter Enable HIGH if ready and exposure both true
}

// Input Pins debounce introduction
void setup_input_pin()
{
	mAs_Button.setDebounceTime(50);		 // set debounce time to 50 milliseconds
	kV_Button.setDebounceTime(50);		 // set debounce time to 50 milliseconds
	LEFT_BUTTON.setDebounceTime(50);	 // set debounce time to 50 milliseconds
	RIGHT_BUTTON.setDebounceTime(50);	 // set debounce time to 50 milliseconds
	READY_Button.setDebounceTime(50);	 // set debounce time to 50 milliseconds
	EXPOSURE_Button.setDebounceTime(50); // set debounce time to 50 milliseconds
	POWER_BUTTON.setDebounceTime(50);	 // set debounce time to 50 milliseconds

	pinMode(NTC_T1, INPUT);				// Input define thermistor Pin
	pinMode(NTC_T2, INPUT);				// Input define thermistor Pin
	pinMode(HV_FEEDBACK, INPUT);		// Input define HV feedback IO Pin
	pinMode(BATTERY_VOLTAGE, INPUT);	// Input define Battery Voltage sensing Pin
	pinMode(Thermal_Cutoff_Pin, INPUT); // Input define Thermal cut-off Pin
}

// Get duty cycle based on the set kV value. Based on lookup table
float getHVDutyCycle(uint16_t kV_output)
{
	switch (kV_output)
	{
	case 70:
		return 48.0;
	case 65:
		return 46.0;
	case 60:
		return 44.0;
	case 55:
		return 42.0;
	case 50:
		return 40.0;
	case 5:
		return 22.0;
	case 10:
		return 24.0;
	case 15:
		return 26.0;
	case 20:
		return 28.0;
	case 25:
		return 30.0;
	case 30:
		return 32.0;
	case 35:
		return 34.0;
	case 40:
		return 36.0;
	case 45:
		return 38.0;
	default:
		return 0.0; // invalid or unsupported input
	}
}

void batteryLogic(int randNumber, long timec)
{
	if (millis() - timec > 500)
	{
		/*Compare battery voltage */
		if (randNumber < 61)
		{
			lv_obj_add_flag(ui_battery25, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_battery50, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_Image75, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_battery100, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(ui_battery0, LV_OBJ_FLAG_HIDDEN);
		}
		else if (randNumber < 121)
		{
			lv_obj_add_flag(ui_battery50, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_Image75, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_battery100, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_battery0, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(ui_battery25, LV_OBJ_FLAG_HIDDEN);
		}
		else if (randNumber < 181)
		{
			lv_obj_add_flag(ui_battery0, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_Image75, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_battery100, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_battery25, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(ui_battery50, LV_OBJ_FLAG_HIDDEN);
		}
		else if (randNumber < 241)
		{
			lv_obj_add_flag(ui_battery0, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_battery100, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_battery25, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_battery50, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(ui_Image75, LV_OBJ_FLAG_HIDDEN);
		}
		else
		{
			lv_obj_add_flag(ui_battery0, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_battery25, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_battery50, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_Image75, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(ui_battery100, LV_OBJ_FLAG_HIDDEN);
		}

		timec = millis();
	}
}

void setup()
{
	Serial.begin(115200);
	Serial.println("LVGL Widgets Demo");

	randomSeed(analogRead(BATTERY_VOLTAGE));

	setup_display();

	setup_output_pins(); // Initialize output pins
	setup_input_pin();	 // Initialize input pins

	setup_initial_display(); // Set display to initial setting

	set_outputPin(); // Set output pin states

	Wire.begin(20, 19); // Setup I2C communication with DAC

	MCP.begin(); // Initialize the MCP

	MCP.setMaxVoltage(2.5); // Setting maximum voltage to 2.5 to get max 50% duty cycle
	Serial.print("last write data (from setup): ");
	Serial.println(MCP.getLastWriteEEPROM());
	MCP.setVoltage(0);
}

void loop()
{

	int randNumber = 200;

	lv_timer_handler(); /*let the GUI do its work */ // Call this to update LVGL and process timers

	mAs_Button.loop();		// Check for "mAs" button pressed
	kV_Button.loop();		// check for "kV" button pressed
	READY_Button.loop();	// check for "ready for exposure" button pressed
	EXPOSURE_Button.loop(); // check for  "exposure" button pressed
	POWER_BUTTON.loop();	// for checking if the shutdown button is pressed
	LEFT_BUTTON.loop();		// for changing the kV output
	RIGHT_BUTTON.loop();	// for changing the kV output

	// int posButtonState = mAs_Button.getState();						// Read "Up" button state
	// int negButtonState = kV_Button.getState();						// Read "Down" button state
	// int readyForExposureButtonState = READY_Button.getState();		// Read "Ready for Exposure" button state
	// int exposureButtonState = EXPOSURE_Button.getState();			// Read "Exposure" button state
	// int PowerOffState = EXPOSURE_Button.getState();					// Read "power button" state - to detect if power off signal needs to be generated

	set_outputPin();
	batteryLogic(randNumber, timec); // Update battery charge bar state symbol

	/*Check for Cool down period and update on display*/
	if (POWER_BUTTON.isPressed())
	{
		Serial.println("Power button is pressed");
		digitalWrite(SHUTDOWN_EN, HIGH); // Initialize power off sequence
	}

	/*Compares &update if Up button is pressed at standby mode &update display*/
	if (mAs_Button.isPressed() && STANDBY && !EXPOSURE && !READY && !COOLDOWN)
	{
		Serial.println("The button 1 is pressed");
		if (mAs < mAs_max)
		{
			mAs += mAs_increment; // Increase mAs by mAs increment set
		}
		else
		{
			mAs = mAs_min; // Set mAs to mAs_min if mAs = mAs_mAx
		}
		lv_label_set_text(ui_labelTimer, String(mAs).c_str());
		Serial.print("mAs: ");
		Serial.println(mAs);
	}

	if (kV_Button.isPressed() && STANDBY && !EXPOSURE && !READY && !COOLDOWN)
	{
		Serial.println("The kV Button is pressed");
		if (kv < kv_max)
		{
			kv += kv_increment; // Increment in kV
								// String sj = ss + "kV";
								// timerInS -= 0.1;
								//  lv_label_set_text(ui_labelTimer, String(("%0.1f", timerInMs / 1000)).c_str());
		}
		else
		{
			kv = kv_min; // Set kV = kv min in all other instances
		}
		String ss = String(kv) + "kV";
		lv_label_set_text(ui_labelKv, String(ss).c_str());

		Serial.print("kV"); //
		Serial.println(kv);
	}

	if (READY_Button.isPressed()) // Check if "Ready for Exposure" is pressed
	{
		// DAC Code inside this Function below
		Serial.println("The button 3 is pressed");

		Serial.println(analogRead(7)); // Reading HV Feedback
		// Serial.println(analogRead(18)); // Read Battery(divided) Voltage from voltage divider
		Serial.println(analogRead(8)); // Read Battery(divided) Voltage from voltage divider
		if (STANDBY == true && READY == false && EXPOSURE == false && COOLDOWN == false)
		{
			STANDBY = false;
			EXPOSURE = false;
			READY = true;
			readySeconds = 0;
			// readyTimerStart = millis();
			lastReadyUpdate = millis();
			readyStartTime = millis(); // Record READY mode start time

			Serial.println("Ready mode operational");
			Serial.println(READY && EXPOSURE);
			lv_obj_clear_flag(ui_radiationDanger, LV_OBJ_FLAG_HIDDEN);
			lv_label_set_text(ui_Labelstatus, "READY");
			// slowstart(50, 2);
			Serial.println("Is ready for Exposure mode on");
			Serial.println("STANDBY: ");
			Serial.println(STANDBY);
			Serial.println("EXPOSURE: ");
			Serial.println(EXPOSURE);
			Serial.println("READY: ");
			Serial.println(READY);
		}
		else if (STANDBY == false && READY == true && EXPOSURE == false && COOLDOWN == false)
		{
			STANDBY = true;
			READY = false;
			lv_obj_add_flag(ui_radiationDanger, LV_OBJ_FLAG_HIDDEN);
			lv_label_set_text(ui_Labelstatus, "STANDBY");
			Serial.println("Is ready for Exposure mode off.");
			Serial.println("STANDBY: ");
			Serial.println(STANDBY);
			Serial.println("EXPOSURE: ");
			Serial.println(EXPOSURE);
			Serial.println("READY: ");
			Serial.println(READY);
		}

		set_outputPin(); // for all other operations
	}

	// Countdown Timer

	if (READY == true && EXPOSURE == false && COOLDOWN == false)
	{
		if (millis() - lastReadyUpdate >= 1000) // 1 second passed
		{
			readySeconds++;				// increase counter
			lastReadyUpdate = millis(); // reset timer
			if (readySeconds < 15)
			{
				String readyText = "READY " + String(readySeconds) + "s";
				lv_label_set_text(ui_Labelstatus, readyText.c_str());
				Serial.println(readyText);
				lv_timer_handler(); // let GUI update
			}
			// after 15 sec msg should display "READY FOR EXPPOSURE"
			else
			{
				lv_label_set_text(ui_Labelstatus, "Ready for Exposure");
				Serial.println("Ready for Exposure");
				lv_timer_handler(); // let GUI update
			}

		}
	}

	// Exposure Button Warning Message
	if (EXPOSURE_Button.isPressed() && !COOLDOWN && !STANDBY) // Check if "Exposure" is pressed
	{

		if (READY == true && EXPOSURE == false)
		{
			if (millis() - readyStartTime < 15000) // Less than 15 seconds
			{
				lv_label_set_text(ui_Labelstatus, "Wait for 15 sec");
				lv_timer_handler(); /*let the GUI do its work */
				Serial.println("Exposure blocked. Wait for 15 sec.");
				lv_timer_handler(); /*let the GUI do its work */
				return;				// Don't allow exposure
			}

			if (millis() - readyStartTime >= 15000)
			{
				lv_label_set_text(ui_Labelstatus, "Ready for Expose");
				lv_timer_handler();
				// No return here – continue to Exposure logic
			}

			EXPOSURE = true;
			// lv_timer_handler();
			lv_label_set_text(ui_Labelstatus, "EXPOSURE");
			lv_obj_clear_flag(ui_eXPOSURE, LV_OBJ_FLAG_HIDDEN);
			lv_timer_handler(); /*let the GUI do its work */
			// delay(delayTime);

			set_outputPin();

			Serial.println("begining loop");

			et = mAs / mA; // set exposure time
			Serial.println("et");
			Serial.println(et);
			// lv_label_set_text(ui_Labelstatus, String(et).c_str());
			float test1 = 0;
			// long int finalTime = micros() + ((et)+(delayTime))*1000000;	//Setting end time for the exposure
			hv_dc = kv;

			Serial.println("******PRE loop EXP*******");
			Serial.println("STANDBY: ");
			Serial.println(STANDBY);
			Serial.println("EXPOSURE: ");
			Serial.println(EXPOSURE);
			Serial.println("READY: ");
			Serial.println(READY);
			double update_interval = 1e-2; // Update interval equal to 1 millisecond
			double time_step = 0;
			int DAC_input_data = 0;
			long int currentTime = micros();
			long int finalTime = micros() + ((et) + (delayTime)) * 1000000;

			while (micros() < finalTime)
			{
				Serial.println(time_step);

				if (digitalRead(Button_Exposure_Pin) == 0)
				{

					set_outputPin(); // Set enable pins state (LOW) for exposure
					lv_timer_handler();

					dac_volt_target = (ref_voltage / 100) * hv_dc;
					// lv_label_set_text(ui_Labelstatus, String(dac_volt_target).c_str());

					DAC_input_data = (4095 * dac_volt_target / ref_voltage) * (1 - (pow(2, -(3 * (time_step) / risetime)))); // to be used when duty cycle is being controlled
					// DAC_input_data = (4095*kv/100)*(1-(pow(2,-(3*(test1)/risetime)))); // to be used when duty cycle is being controlled
					MCP.setValue(DAC_input_data);
					// lv_label_set_text(ui_Labelstatus, String(DAC_input_data).c_str());
					//  time_step +=1;
					//  mcp_set_voltage = dac_volt_target - (dac_volt_target)*(exp(-1*2*time_step/risetime));
					//  MCP.setVoltage(mcp_set_voltage);
					time_step = time_step + update_interval;
					// test1+=1;
				}
				else
				{
					Serial.println("switching");
					// READY = false;		  // Exit Ready for exposure state
					// EXPOSURE = false;	  // Exit Exposure state
					// STANDBY = false;		//System needs to go in cooldown
					// COOLDOWN = true;
					// set_outputPin();
					lv_label_set_text(ui_Labelstatus, "ABORT");
					lv_timer_handler();
					delay(100);
					break;
				}
			}
			MCP.setVoltage(0);

			READY = false;
			EXPOSURE = false;
			STANDBY = false;
			COOLDOWN = true;
			set_outputPin();
			// lv_timer_handler();
			lv_obj_add_flag(ui_radiationDanger, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(ui_eXPOSURE, LV_OBJ_FLAG_HIDDEN);
			lv_timer_handler();

			// Serial.println("released");

			Serial.println(millis());

			if (COOLDOWN)
			{
				// lv_timer_handler();
				lv_obj_clear_flag(ui_CoolDown, LV_OBJ_FLAG_HIDDEN);
				lv_label_set_text(ui_Labelstatus, "COOLDOWN");
				lv_timer_handler();
				cooldownTime = et * xrt_duty * 1000000; // Cooldown time in microseconds
				fil_off_delay = et * 5 * 1000000;
				// cooldownStartTime = micros();
				cooldownTimer = 0;
				cooldownEndTime = micros() + cooldownTime;

				while (cooldownStartTime < cooldownEndTime)
				{
					cooldownStartTime = micros();
					cooldownTimer = cooldownTime - (micros() + cooldownTime - cooldownEndTime);
					String cl_tim_string = "COOLDOWN " + String(cooldownTimer / 1000000);
					lv_label_set_text(ui_Labelstatus, String(cl_tim_string).c_str());
					if (digitalRead(FIL_LS_EN) == LOW && cooldownTimer < (cooldownTime - fil_off_delay))
					{
						digitalWrite(FIL_LS_EN, HIGH);
						lv_label_set_text(ui_Labelstatus, (String(cl_tim_string) + "-").c_str());
					};
					lv_timer_handler();
				}

				COOLDOWN = false;
				// lv_timer_handler();
				lv_obj_add_flag(ui_CoolDown, LV_OBJ_FLAG_HIDDEN);
				lv_timer_handler();
			}

			READY = false;	  // Exit Ready for exposure state
			EXPOSURE = false; // Exit Exposure state
			COOLDOWN = false;
			STANDBY = true;	 // Back to Standby Mode
			set_outputPin(); // for exposure
			// lv_timer_handler();
			lv_label_set_text(ui_Labelstatus, "STANDBY");
			lv_timer_handler();

			Serial.println("******post loop EXP*******");
			Serial.println("STANDBY: ");
			Serial.println(STANDBY);
			Serial.println("EXPOSURE: ");
			Serial.println(EXPOSURE);
			Serial.println("READY: ");
			Serial.println(READY);
		}
	}

	/********************************************/
	/********************************************/
}
