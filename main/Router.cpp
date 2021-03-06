
#include <esp_log.h>
#include "BTSender.h"
#include <string>
#include "sdkconfig.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include <freertos/semphr.h>
#include "RingBufCPP.h"
#include <logdef.h>
#include "Switch.h"
#include "sensor.h"
#include "Router.h"
#include "Protocols.h"
#include "Flarm.h"

RingBufCPP<SString, QUEUE_SIZE> wl_vario_tx_q;
RingBufCPP<SString, QUEUE_SIZE> wl_flarm_tx_q;
RingBufCPP<SString, QUEUE_SIZE> wl_aux_tx_q;
RingBufCPP<SString, QUEUE_SIZE> wl_vario_rx_q;
RingBufCPP<SString, QUEUE_SIZE> wl_flarm_rx_q;
RingBufCPP<SString, QUEUE_SIZE> wl_aux_rx_q;

RingBufCPP<SString, QUEUE_SIZE> bt_tx_q;
RingBufCPP<SString, QUEUE_SIZE> bt_rx_q;

RingBufCPP<SString, QUEUE_SIZE> s1_tx_q;
RingBufCPP<SString, QUEUE_SIZE> s2_tx_q;

RingBufCPP<SString, QUEUE_SIZE> s1_rx_q;
RingBufCPP<SString, QUEUE_SIZE> s2_rx_q;

RingBufCPP<SString, QUEUE_SIZE> xcv_rx_q;
// RingBufCPP<SString, QUEUE_SIZE> xcv_tx_q;

portMUX_TYPE btmux = portMUX_INITIALIZER_UNLOCKED;

// Utility methods to push and pull data into/from queues

// checks if Queue is full, otherwise appends SString
bool Router::forwardMsg( SString &s, RingBufCPP<SString, QUEUE_SIZE>& q ){
	if( !q.isFull() ) {
		portENTER_CRITICAL_ISR(&btmux);
		q.add( s );
		portEXIT_CRITICAL_ISR(&btmux);
		return true;
	}
	return false;
}

// gets last message from ringbuffer FIFO
bool Router::pullMsg( RingBufCPP<SString, QUEUE_SIZE>& q, SString& s ){
	if( !q.isEmpty() ){
		portENTER_CRITICAL_ISR(&btmux);
		q.pull( &s );
		portEXIT_CRITICAL_ISR(&btmux);
		return true;
	}
	return false;
}

// XCVario Router  ALL NMEA goes to s1_rx_q, from there its distributed further
void Router::sendNMEA(char * s){
	// ESP_LOGV( FNAME,"XCVario message %s",s);
	SString xcv( s );
	if( forwardMsg( xcv, s1_rx_q ) )
		ESP_LOGV(FNAME,"Forward %d bytes from NMEA", xcv.length() );
	else
		ESP_LOGW(FNAME,"Dropped %d bytes from NMEA", xcv.length() );
}


// Route messages from serial interface S1
void Router::routeS1(){
	SString s1;
	if( pullMsg( s1_rx_q, s1) ){
		ESP_LOGV(FNAME,"S1 RX len: %d bytes, BT-Q full:%d", s1.length(), bt_tx_q.isFull() );
		// ESP_LOG_BUFFER_HEXDUMP(FNAME,s1.c_str(),s1.length(), ESP_LOG_DEBUG);
		// Protocols::parseNMEA( s1.c_str() );

		if( serial1_route.get() & RT_WIRELESS )
		{
			// ESP_LOGI(FNAME,"WIRELESS enabled");
			if( blue_enable.get() == WL_WLAN ){
				if( forwardMsg( s1, wl_vario_tx_q ))
					ESP_LOGV(FNAME,"S1 RX bytes %d forward to wl_vario_tx_q port 8880", s1.length() );
			}
			else if( blue_enable.get() == WL_BLUETOOTH ){
				ESP_LOGV(FNAME,"WL_BLUETOOTH enabled send %d", s1.length()  );
				if( forwardMsg( s1, bt_tx_q ))
					ESP_LOGV(FNAME,"S1 RX bytes %d forward to bt_tx_q", s1.length() );
				else
					ESP_LOGW(FNAME,"Dropped bytes %d forward to bt_tx_q", s1.length() );
			}
		}
		if( serial1_route.get() & RT_SERIAL ){
			if( forwardMsg( s1, s1_tx_q ))
				ESP_LOGV(FNAME,"S1 RX bytes %d forward to s1_tx_q", s1.length() );
		}
	}
}

// Route messages from serial interface S2
void Router::routeS2(){
	SString s2;
	if( pullMsg( s2_rx_q, s2) ){
		ESP_LOGD(FNAME,"S2 RX len: %d bytes, Q full:%d", s2.length(), bt_tx_q.isFull() );
		ESP_LOG_BUFFER_HEXDUMP(FNAME,s2.c_str(),s2.length(), ESP_LOG_DEBUG);
		Protocols::parseNMEA( s2.c_str() );

		/*
		if( serial2_route.get() & RT_WIRELESS )
		{
			// ESP_LOGI(FNAME,"S2 WIRELESS enabled");
			if( blue_enable.get() == WL_WLAN){
				// ESP_LOGI(FNAME,"WL_WLAN enabled");
				if( forwardMsg( s2, wl_flarm_tx_q ))
					ESP_LOGI(FNAME,"S2 RX bytes %d forward to wl_flarm_tx_q port 8881", s2.length() );
			}
			else if( blue_enable.get() == WL_BLUETOOTH ){
				// ESP_LOGI(FNAME,"WL_BLUETOOTH enabled");
				if( forwardMsg( s2, bt_tx_q ))
					ESP_LOGI(FNAME,"S2 RX bytes %d forward to bt_tx_q", s2.length() );
			}
		}
		*/
		if( serial2_route.get() & RT_SERIAL ){
			if( forwardMsg( s2, s1_tx_q ))
				ESP_LOGV(FNAME,"S2 RX bytes %d forward to s1_tx_q", s2.length() );
		}
	}
}

// route messages from WLAN
void Router::routeWLAN(){
	if( blue_enable.get() != WL_WLAN )
		return;
	// Route received data from WLAN ports
	SString wlmsg;
	if( pullMsg( wl_vario_rx_q, wlmsg) ){
		if( strncmp( wlmsg.c_str(), "!", 1 )  == 0 ) {
			ESP_LOGV(FNAME,"From WLAN port 8880 RX matched a Borgelt command %s", wlmsg.c_str() );
			Protocols::parseNMEA( wlmsg.c_str() );
		}
	}
	if( pullMsg( wl_flarm_rx_q, wlmsg ) ){
		Flarm::parsePFLAX( wlmsg );
		if( serial1_route.get() & RT_WIRELESS )
			if( forwardMsg( wlmsg, s1_tx_q ) )
				ESP_LOGV(FNAME,"Send to  device, TCP port 8881 received %d bytes", wlmsg.length() );
		if( serial2_route.get() & RT_WIRELESS )
			if( forwardMsg( wlmsg, s2_tx_q ) )
				ESP_LOGV(FNAME,"Send to S2 device, TCP port 8881 received %d bytes", wlmsg.length() );
	}
	if( pullMsg( wl_aux_rx_q, wlmsg ) ){
		Flarm::parsePFLAX( wlmsg );
		if( serial1_route.get() & RT_WIRELESS )
			if( forwardMsg( wlmsg, s1_tx_q ) )
				ESP_LOGV(FNAME,"Send to  device, TCP port 8882 received %d bytes", wlmsg.length() );
		if( serial2_route.get() & RT_WIRELESS )
			if( forwardMsg( wlmsg, s2_tx_q ) )
				ESP_LOGV(FNAME,"Send to S2 device, TCP port 8882 received %d bytes", wlmsg.length() );
	}
}

// route messages from Bluetooth
void Router::routeBT(){
	if( blue_enable.get() != WL_BLUETOOTH )
		return;
	SString bt;
	if( pullMsg( bt_rx_q, bt ) ){
		Flarm::parsePFLAX( bt );
		if( serial1_route.get() & RT_WIRELESS )  // Serial data TX from bluetooth enabled ?
			if( forwardMsg( bt, s1_tx_q ) )
				ESP_LOGV(FNAME,"Send to S1 device, BT received %d bytes", bt.length() );
		if( serial2_route.get() & RT_WIRELESS )  // Serial data TX from bluetooth enabled ?
			if( forwardMsg( bt, s2_tx_q ) )
				ESP_LOGV(FNAME,"Send to S2 device, BT received %d bytes", bt.length() );
		// always check if it is a command to ourselves
		if( strncmp( bt.c_str(), "!g,", 3 )  == 0 ) {
			ESP_LOGV(FNAME,"BT RX Matched a Borgelt command %s", bt.c_str() );
			Protocols::parseNMEA( bt.c_str() );
		}
	}
}


