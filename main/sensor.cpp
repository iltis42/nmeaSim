#include <string>
#include "sdkconfig.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "string.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include <driver/adc.h>
#include <driver/gpio.h>
#include "ESP32NVS.h"
#include "BTSender.h"
#include "Setup.h"
#include "esp_sleep.h"
#include <esp_wifi.h>
#include "AnalogInput.h"
#include "sensor.h"
#include "Version.h"

#include <SPI.h>
#include <OTA.h>
#include "SetupNG.h"
#include <logdef.h>
#include "Switch.h"

#include "I2Cbus.hpp"
#include "WifiApp.h"
#include "WifiClient.h"
#include "Serial.h"
#include "Switch.h"
#include <esp32/rom/miniz.h>
#include "esp32-hal-adc.h" // needed for adc pin reset
#include "soc/sens_reg.h" // needed for adc pin reset
#include "driver/gpio.h"
#include "glider.h"
#include "nmeasim.h"
#include "spline.h"
#include <map>


OTA *ota = 0;
BTSender btsender;
Switch theButton;

// Sensor board init method. Herein all functions that make the XCVario are launched and tested.
void sensor(void *args){
	esp_wifi_set_mode(WIFI_MODE_NULL);
	// esp_log_level_set("*", ESP_LOG_INFO);
	ESP_LOGI( FNAME, "Log level set globally to INFO %d",  ESP_LOG_INFO);
	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);
	ESP_LOGI( FNAME,"This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
			chip_info.cores,
			(chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
					(chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
	ESP_LOGI( FNAME,"Silicon revision %d, ", chip_info.revision);
	ESP_LOGI( FNAME,"%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
			(chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

	NVS.begin();
	theButton.begin(GPIO_NUM_0);  // push Button

	bool doUpdate = false; //  = software_update.get();
	if( Switch::isClosed() ){
		doUpdate = true;
		ESP_LOGI(FNAME,"Rotary pressed: Do Software Update");
	}
	if( doUpdate ) {
		ota = new OTA();
		ota->begin();
		ota->doSoftwareUpdate();
	}
	String wireless_id;
	if( blue_enable.get() == WL_BLUETOOTH ) {
		btsender.begin();
	}
	wireless_id += SetupCommon::getID();
	Serial::begin();
	Serial::taskStart();

	if( blue_enable.get() == WL_BLUETOOTH ) {
		btsender.selfTest();
	}else if ( blue_enable.get() == WL_WLAN ){
		wifi_init_softap();
	}
	Version myVersion;
	ESP_LOGI(FNAME,"Program Version %s", myVersion.version() );

}

float randHeading(){
	return 360*((float)rand() / ((float)RAND_MAX + 1));
}

extern "C" void  app_main(void){
	ESP_LOGI(FNAME,"app_main" );
	ESP_LOGI(FNAME,"Now init all Setup elements");
	bool setupPresent;
	SetupCommon::initSetup( setupPresent );
	esp_log_level_set("*", ESP_LOG_INFO);
	sensor( 0 );

	std::vector<double> X;
	std::vector<double> Y;

	X = { -135,- 90, -45,  0, 45, 90, 135, 180, 225, 270,  315, 360, 405, 450 };
	Y = { -4.2,  -6,-4.2,  0, 4.2, 6, 4.2,   0,-4.2,  -6, -4.2,   0, 4.2,   6 };
	tk::spline dev(X,Y, tk::spline::cspline_hermite );

	float heading = 0;

	ESP_LOGI(FNAME,"0: %f", dev(0));
	ESP_LOGI(FNAME,"45: %f", dev(45));
	ESP_LOGI(FNAME,"90: %f", dev(90));
	ESP_LOGI(FNAME,"135: %f", dev(135));
	ESP_LOGI(FNAME,"180: %f", dev(180));
	ESP_LOGI(FNAME,"225: %f", dev(225));
	ESP_LOGI(FNAME,"270: %f", dev(270));


	while( 1 ){
		nmeasim( "cir", 120, 25, 90,   0,  0, 0  );
		heading = randHeading();
		nmeasim( "str_head", 60,  25, 90, heading,  0, dev( (double)heading )  );  // the fly N straight  min
		heading = randHeading();
		nmeasim( "str_head", 120, 25, 90, heading,  0, dev( (double)heading ) );  // the fly N straight  min
		heading = randHeading();
		nmeasim( "str_head", 60,  25, 90, heading,  0, dev( (double)heading ) );  // the fly N straight  min
		heading = randHeading();
		nmeasim( "str_head", 120, 25, 90, heading,  0, dev( (double)heading ) );  // the fly N straight  min
		heading = randHeading();
		nmeasim( "str_head", 60,  25, 90, heading,  0, dev( (double)heading ) );  // the fly N straight  min
		heading = randHeading();
		nmeasim( "str_head", 120, 25, 90, heading,  0, dev( (double)heading ) );  // the fly N straight  min
		heading = randHeading();
		nmeasim( "str_head", 60,  25, 90, heading,  0, dev( (double)heading ) );  // the fly N straight  min
		heading = randHeading();
		nmeasim( "str_head", 120, 25, 90, heading,  0, dev( (double)heading ) );  // the fly N straight  min

		/*
		nmeasim( "cir", 180, 25, 90,   0,  0, 0    );  // circle 3 min, no deviation
		nmeasim( "str_head", 120, 25, 90,  0, 0, 0    );  // the fly N straight 3 min
		nmeasim( "str_head", 120, 25, 90, 45, 0, 4.2  );  // the fly NE straight 3 min
		nmeasim( "str_head", 120, 25, 90, 90, 0, 6    );  // the fly E straight 3 min
		nmeasim( "str_head", 120, 25, 90,135, 0, 4.2  );  // the fly SE straight 3 min
		nmeasim( "str_head", 120, 25, 90,180, 0, 0    );  // the fly S straight 3 min
		nmeasim( "str_head", 120, 25, 90,225, 0, -4.2 );  // the fly SW straight 3 min
		nmeasim( "str_head", 120, 25, 90,270, 0, -6   );  // the fly W straight 3 min
		nmeasim( "str_head", 120, 25, 90,315, 0, -4.2 );  // the fly NW straight 3 min
		*/
	}



	vTaskDelete( NULL );
}



