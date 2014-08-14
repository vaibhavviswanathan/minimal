/*
Minimal w/ battery and bluetooth
Created by Vaibhav Viswanathan
Open source
Feel free to modify, redistribute and do whatever you'd like with it :)

Based off Clean by Edwin Finch: https://github.com/edwinfinch/firstwatchface/blob/master/src/main.c
Trek font taken from PebbleTrek by Kyle Potts: https://github.com/kylepotts/pebbletrek
*/

#include <pebble.h>
#include "QTPlus.h"
#define NUM_ANIMATION_PKEY 1
#define NUM_ANIMATION_DEFAULT 1

Window* window;
TextLayer *text_layer, *battery_text_layer, *date_text_layer, *update_at_a_glance;
InverterLayer *invert_layer;
static GFont trek20;
static GFont gotham34;

GBitmap* bt_connected;
BitmapLayer* bt_connected_layer;

char timeBuffer[] = "00:00";
char dateBuffer[] = "February 31";

bool booted = false;
bool did58Animate = false;
int animationNumber = 0;

//Define our variables for persistent storage
static int num_animation = NUM_ANIMATION_PKEY;


static TextLayer* textLayerInit(GRect location, GColor colour, GColor background, const char *res_id, GTextAlignment alignment)
{
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, colour);
	text_layer_set_background_color(layer, background);
	text_layer_set_font(layer, fonts_get_system_font(res_id));
	text_layer_set_text_alignment(layer, alignment);

	return layer;
}

void on_animation_stopped(Animation *anim, bool finished, void *context)
{
    //Free the memoery used by the Animation
    property_animation_destroy((PropertyAnimation*) anim);
	animationNumber++;
	if(animationNumber > 3){
		booted = true;
		animationNumber = 0;
	}
}
 
void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay)
{
    //Declare animation
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
     
    //Set characteristics
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);
     
    //Set stopped handler to free memory
    AnimationHandlers handlers = {
        //The reference to the stopped handler is the only one in the array
        .stopped = (AnimationStoppedHandler) on_animation_stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
     
    //Start animation
    animation_schedule((Animation*) anim);
}

//Custom function which takes a few variables pushes either a vibration, text update, or both to watch
void glance_this(const char *updateText, bool vibrate, int vibrateNum, int animationLength){
	//if the watchface has already initialized 
	if(booted == true){
		//Update the text layer to the char provided by function call
		text_layer_set_text(update_at_a_glance, updateText);
		//if there's a vibration request,
		if(vibrate == true){
			//check what number it is and fufill accordingly.
			if(vibrateNum == 1){
				vibes_short_pulse();
		  }
			else if(vibrateNum == 2){
				vibes_double_pulse();
		  }
			else if(vibrateNum == 3){
				vibes_long_pulse();
		  }
	  }
		//Animate update_at_a_glance layer for fancy effect
		GRect start01 = GRect(0, -100, 140, 168);
		GRect finish01 = GRect(0, 10, 140, 168);
		GRect start02 = GRect(0, 10, 140, 168);
		GRect finish02 = GRect(0, -100, 140, 168);
		animate_layer(text_layer_get_layer(update_at_a_glance), &start01, &finish01, 1000, 0);
		animate_layer(text_layer_get_layer(update_at_a_glance), &start02, &finish02, 1000, animationLength);
	}
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  //Format the Time buffer string using tick_time as the time source
  strftime(timeBuffer, sizeof("00:00"), "%H:%M", tick_time);
  //Format dateBuffer using tick_time as source
  strftime(dateBuffer, sizeof(dateBuffer), "%B %e", tick_time);

	if(clock_is_24h_style()){
      strftime(timeBuffer, sizeof(timeBuffer), "%H:%M", tick_time);
    }
    else{
      strftime(timeBuffer,sizeof(timeBuffer),"%I:%M",tick_time);
    }

  //Change the TextLayer(s) text to show the new time and date
  text_layer_set_text(text_layer, timeBuffer);
  text_layer_set_text(date_text_layer, dateBuffer);

  int seconds = tick_time->tm_sec;
        
	if(seconds == 58 && booted == true)
        {
		  srand(time(NULL));
		  int whichAnimationTime = rand() % 5;
		  did58Animate = true;

		if(whichAnimationTime == 1){
          //Slide offscreen downward
          GRect start = GRect(0, 53, 140, 168);
          GRect finish = GRect(0, 168, 140, 168);
          animate_layer(text_layer_get_layer(text_layer), &start, &finish, 500, 800);
		}
		else if(whichAnimationTime == 2){
			//Slide offscreen upward
			GRect start = GRect(0, 53, 140, 168);
          	GRect finish = GRect(0, -168, 140, 168);
          	animate_layer(text_layer_get_layer(text_layer), &start, &finish, 500, 800);
		}
		else if(whichAnimationTime == 3){
			//Slide off to the left
		    GRect start = GRect(0, 53, 140, 168);
            GRect finish = GRect(-200, 53, 140, 168);
            animate_layer(text_layer_get_layer(text_layer), &start, &finish, 500, 800);
		}
		else if(whichAnimationTime == 4){
			//Slide off to the right
			GRect start = GRect(0, 53, 140, 168);
			GRect finish = GRect(300, 53, 140, 168);
            animate_layer(text_layer_get_layer(text_layer), &start, &finish, 500, 800);
		}
		else if(whichAnimationTime == 5){
			//Slide out diagonally 
			GRect start = GRect(0, 53, 140, 168);
			GRect finish = GRect(-200, -153, 140, 168);
            animate_layer(text_layer_get_layer(text_layer), &start, &finish, 500, 800);
		}
		else{
			//Slide out diagonally (this is incase rand() bugs out)
			GRect start = GRect(0, 53, 140, 168);
			GRect finish = GRect(300, 350, 140, 168);
            animate_layer(text_layer_get_layer(text_layer), &start, &finish, 500, 800);
		}

      }
        
    else if(seconds == 0 && booted == true && did58Animate == true)
        {
          text_layer_set_text(text_layer, timeBuffer);

		srand(time(NULL));
		  int whichAnimationTime = rand() % 3;

		if(whichAnimationTime == 1){
	      //Slide onscreen from the left
          GRect start = GRect(-144, 53, 140, 168);
          GRect finish = GRect(0, 53, 140, 168);
          animate_layer(text_layer_get_layer(text_layer), &start, &finish, 500, 800);
		}
		else if(whichAnimationTime == 2){
			//Slide onscreen from the right
		  GRect start = GRect(200, 53, 140, 168);
          GRect finish = GRect(0, 53, 140, 168);
          animate_layer(text_layer_get_layer(text_layer), &start, &finish, 500, 800);
		}
		else if(whichAnimationTime == 3){
		   //Slide in from top
		  GRect start = GRect(0, -70, 140, 168);
          GRect finish = GRect(0, 53, 140, 168);
          animate_layer(text_layer_get_layer(text_layer), &start, &finish, 500, 800);
		}
		else{
			//Slide in diagonally
		  GRect start = GRect(-100, -100, 140, 168);
          GRect finish = GRect(0, 53, 140, 168);
          animate_layer(text_layer_get_layer(text_layer), &start, &finish, 500, 800);
		}
      }
        
    else
        {
          //Change the TextLayer text to show the new time
          text_layer_set_text(text_layer, timeBuffer);
		if(seconds == 2 && did58Animate == true){
			did58Animate = false;
		}
      }
}

void handle_bt(bool connected){
	if(connected == 0){
		bt_connected = gbitmap_create_with_resource(RESOURCE_ID_bt_disconnected);  
		glance_this("Bluetooth disconnected", 1, 3, 5000);
  }
  	if(connected == 1){
  	  bt_connected = gbitmap_create_with_resource(RESOURCE_ID_bt_connected);
	    glance_this("Bluetooth connected", 1, 2, 3000);
  }
	bitmap_layer_set_bitmap(bt_connected_layer,bt_connected);
}


void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "";

  if (charge_state.is_charging) {
      snprintf(battery_text, sizeof(battery_text), "chg");
    } else if(charge_state.charge_percent <= 30) {
        snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
      } else {
				snprintf(battery_text, sizeof(battery_text), " ");
	}
  text_layer_set_text(battery_text_layer, battery_text);
}

void window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  trek20 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_impactfont_20));
	gotham34 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_gotham_light_34));

  //Bluetooth layer
	bt_connected = gbitmap_create_with_resource(RESOURCE_ID_bt_disconnected);
  bt_connected_layer = bitmap_layer_create(GRect(110,-5,40,40)); //old value of Y is 135
  bitmap_layer_set_background_color(bt_connected_layer,GColorClear);
	//layer_add_child(window_layer,bitmap_layer_get_layer(bt_connected_layer));
  
	//Time layer
  //GRect location, GColor colour, GColor background, const char *res_id, GTextAlignment alignment
  text_layer = textLayerInit(GRect(0, 53, 140, 168), GColorBlack, GColorClear, FONT_KEY_BITHAM_42_LIGHT, GTextAlignmentCenter);
	//text_layer_set_font(battery_text_layer,gotham34);
  layer_add_child(window_get_root_layer(window), (Layer*) text_layer);

  //Quick update text layer
  update_at_a_glance = textLayerInit(GRect(0, -50, 140, 168), GColorBlack, GColorClear, FONT_KEY_GOTHIC_18, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer*) update_at_a_glance);


  //Date layer
  date_text_layer = textLayerInit(GRect(0, 97, 140, 168), GColorBlack, GColorClear, FONT_KEY_GOTHIC_18, GTextAlignmentCenter);
  //text_layer_set_font(date_text_layer,trek20);
	layer_add_child(window_layer, text_layer_get_layer(date_text_layer));

  //Battery text layer
  battery_text_layer = textLayerInit(GRect(25, 5, 40, 40), GColorBlack, GColorClear, FONT_KEY_ROBOTO_CONDENSED_21, GTextAlignmentCenter); //old value of Y is 145	
  text_layer_set_font(battery_text_layer,trek20);
  layer_add_child(window_layer, text_layer_get_layer(battery_text_layer));

  //Invert all pixels
  //Will now be white on black
  //To make black on white remove these lines
  
  invert_layer = inverter_layer_create(GRect(0, 0, 144, 168));
  layer_add_child(window_layer, (Layer*) invert_layer);
  


  //Get a time structure so that it doesn't start blank
  struct tm *t;
  time_t temp;        
  temp = time(NULL);        
  t = localtime(&temp);        
        
  //Manually call the tick handler when the window is loading
  tick_handler(t, MINUTE_UNIT);

  //Battery update handler
  BatteryChargeState btchg = battery_state_service_peek();
  handle_battery(btchg);

  //Bluetooth update handler
  bool connected = bluetooth_connection_service_peek();
  handle_bt(connected);

}
 
void window_unload(Window *window)
{
  //Destroy elements
	glance_this("one", 1, 3, 5000);
  text_layer_destroy(text_layer);
  text_layer_destroy(date_text_layer);
  text_layer_destroy(battery_text_layer);
  text_layer_destroy(update_at_a_glance);
  fonts_unload_custom_font(trek20);
  bitmap_layer_destroy(bt_connected_layer);
  inverter_layer_destroy(invert_layer);
  gbitmap_destroy(bt_connected);
}

// Beginning of quicktap plus functions
void qtp_setup() {
	qtp_is_showing = false;
	accel_tap_service_subscribe(&qtp_tap_handler);
	qtp_battery_images[0] = gbitmap_create_with_resource(RESOURCE_ID_battery_empty);
	qtp_battery_images[1] = gbitmap_create_with_resource(RESOURCE_ID_battery_partial);
	qtp_battery_images[2] = gbitmap_create_with_resource(RESOURCE_ID_battery);
	qtp_battery_image = qtp_battery_images[0];
	qtp_bluetooth_image = gbitmap_create_with_resource(RESOURCE_ID_bluetooth);

	if (qtp_conf & QTP_K_SUBSCRIBE) {
		qtp_bluetooth_status = bluetooth_connection_service_peek();
		bluetooth_connection_service_subscribe( qtp_bluetooth_callback );
	}
	
	if (qtp_is_show_weather()) {
		qtp_setup_app_message();
	}
}

/* Handle taps from the hardware */
void qtp_tap_handler(AccelAxisType axis, int32_t direction) {
	if (qtp_is_showing) {
		qtp_hide();
	} else {
		qtp_show();
	}
	qtp_is_showing = !qtp_is_showing;
}

/* Subscribe to taps and pass them to the handler */
void qtp_click_config_provider(Window *window) {
	window_single_click_subscribe(BUTTON_ID_BACK, qtp_back_click_responder);
}

/* Unusued. Subscribe to back button to exit */
void qtp_back_click_responder(ClickRecognizerRef recognizer, void *context) {
	qtp_hide();
}

/* Update the text layer for the battery status */
void qtp_update_battery_status(bool mark_dirty) {
	BatteryChargeState charge_state = battery_state_service_peek();
	static char battery_text[] = "100%";
	snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);

	if (charge_state.charge_percent >= 85) {
		qtp_battery_image = qtp_battery_images[2];
	} else if (charge_state.charge_percent <= 30) {
		qtp_battery_image = qtp_battery_images[0];
	} else {
		qtp_battery_image = qtp_battery_images[1];
	}
	bitmap_layer_set_bitmap(qtp_battery_image_layer, qtp_battery_image);

	text_layer_set_text(qtp_battery_text_layer, battery_text);
	if (mark_dirty) {
		layer_mark_dirty(text_layer_get_layer(qtp_battery_text_layer));
		layer_mark_dirty(bitmap_layer_get_layer(qtp_battery_image_layer));
	}
}

/* Update the weather icon. Destroy the current one if necessary */
void qtp_update_weather_icon(int icon_index, bool remove_old, bool mark_dirty) {
	const int icon_id = QTP_WEATHER_ICONS[icon_index];
	qtp_weather_icon = gbitmap_create_with_resource(icon_id);
	bitmap_layer_set_bitmap(qtp_weather_icon_layer, qtp_weather_icon);
	if (remove_old) {
		gbitmap_destroy(qtp_weather_icon);
	}
	if (mark_dirty) {
		layer_mark_dirty(bitmap_layer_get_layer(qtp_weather_icon_layer));
	}
}

/* Update the text layer for the bluetooth status */
void qtp_update_bluetooth_status(bool mark_dirty) {
	static char bluetooth_text[] = "Not Paired";

	if (qtp_bluetooth_status) {
		snprintf(bluetooth_text, sizeof(bluetooth_text), "Paired");
	} else {
		snprintf(bluetooth_text, sizeof(bluetooth_text), "Not Paired");
	}

	text_layer_set_text(qtp_bluetooth_text_layer, bluetooth_text);
	if (mark_dirty) {
		layer_mark_dirty(text_layer_get_layer(qtp_bluetooth_text_layer));
	}
}

/* Update the text layer for the clock */
void qtp_update_time(bool mark_dirty) {
	static char time_text[10];
	clock_copy_time_string(time_text, sizeof(time_text));
	text_layer_set_text(qtp_time_layer, time_text);

	if (mark_dirty) {
		layer_mark_dirty(text_layer_get_layer(qtp_time_layer));
	}
}


/* Setup app message callbacks for weather */
void qtp_setup_app_message() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "QTP: setting app message for weather");

	const int inbound_size = 100;
	const int outbound_size = 100;
	app_message_open(inbound_size, outbound_size);
	Tuplet initial_values[] = {
		TupletInteger(QTP_WEATHER_ICON_KEY, (uint8_t) 8),
		TupletCString(QTP_WEATHER_TEMP_F_KEY, "---\u00B0F"),
		TupletCString(QTP_WEATHER_TEMP_C_KEY, "---\u00B0F"),
		TupletCString(QTP_WEATHER_CITY_KEY, "Atlanta      "),
		TupletCString(QTP_WEATHER_DESC_KEY, "                       ")
	};
	APP_LOG(APP_LOG_LEVEL_DEBUG, "QTP: weather tuples intialized");

	app_sync_init(&qtp_sync, qtp_sync_buffer, sizeof(qtp_sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
	  qtp_sync_changed_callback, qtp_sync_error_callback, NULL);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "QTP: weather app message initialized");

}

/* Handle incoming data from Javascript and update the view accordingly */
static void qtp_sync_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
	
	switch (key) {
		case QTP_WEATHER_TEMP_F_KEY:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "QTP: weather temp f received");
			if (qtp_is_showing && qtp_is_degrees_f()) {
				text_layer_set_text(qtp_temp_layer, new_tuple->value->cstring);
			}
			break;
		case QTP_WEATHER_TEMP_C_KEY:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "QTP: weather temp c received");
			if (qtp_is_showing && !qtp_is_degrees_f()) {
				text_layer_set_text(qtp_temp_layer, new_tuple->value->cstring);
			}
			break;
		case QTP_WEATHER_DESC_KEY:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "QTP: weather desc received: %s", new_tuple->value->cstring);
			if (qtp_is_showing) {
				text_layer_set_text(qtp_weather_desc_layer, new_tuple->value->cstring);
			}
			break;
		case QTP_WEATHER_ICON_KEY:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "QTP: weather icon received: %d", new_tuple->value->uint8);
			if (qtp_is_showing) {
				qtp_update_weather_icon(new_tuple->value->uint8, true, true);
			}
			break;

	}

}

void qtp_bluetooth_callback(bool connected) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "QTP: bluetooth status callback: %d", connected);
	if (qtp_should_vibrate()) {
		if (connected) {
			vibes_short_pulse();
		} else {
			vibes_double_pulse();
		}
	}

	qtp_bluetooth_status = connected;
	if (qtp_is_showing) {
		qtp_update_bluetooth_status(false);
	}
}

/* Clear out the display on failure */
static void qtp_sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "QTP: weather app message error occurred: %d, %d", dict_error, app_message_error);
	if (DICT_NOT_ENOUGH_STORAGE == dict_error) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Not enough storage");
	}

	static char placeholder[] = "--\u00B0F";
	text_layer_set_text(qtp_temp_layer, placeholder);
}


/* Auto-hide the window after a certain time */
void qtp_timeout() {
	qtp_hide();
	qtp_is_showing = false;
}

/* Create the QTPlus Window and initialize the layres */
void qtp_init() {
	qtp_window = window_create();

	/* Time Layer */
	if (qtp_is_show_time()) {

		GRect time_frame = GRect( QTP_PADDING_X, QTP_PADDING_Y, QTP_SCREEN_WIDTH - QTP_PADDING_X, QTP_TIME_HEIGHT );
		qtp_time_layer = text_layer_create(time_frame);
		qtp_update_time(false);
		text_layer_set_text_alignment(qtp_time_layer, GTextAlignmentCenter);
		text_layer_set_font(qtp_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
		layer_add_child(window_get_root_layer(qtp_window), text_layer_get_layer(qtp_time_layer));
	}

	/* Setup weather if it is enabled */
	if (qtp_is_show_weather()) {

		/* Weather description layer */
		GRect desc_frame = GRect( QTP_PADDING_X + QTP_WEATHER_SIZE + 5, qtp_weather_y() + QTP_WEATHER_SIZE, QTP_SCREEN_WIDTH - QTP_PADDING_X, QTP_WEATHER_SIZE);
		qtp_weather_desc_layer = text_layer_create(desc_frame);
		text_layer_set_font(qtp_weather_desc_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
		text_layer_set_text_alignment(qtp_weather_desc_layer, GTextAlignmentLeft);
		const Tuple *desc_tuple = app_sync_get(&qtp_sync, QTP_WEATHER_DESC_KEY);
		if (desc_tuple != NULL) {
			text_layer_set_text(qtp_weather_desc_layer, desc_tuple->value->cstring);
		}
		layer_add_child(window_get_root_layer(qtp_window), text_layer_get_layer(qtp_weather_desc_layer));


		/* Temperature description layer */
		GRect temp_frame = GRect( QTP_PADDING_X + QTP_WEATHER_SIZE + 5, qtp_weather_y(), QTP_SCREEN_WIDTH, QTP_WEATHER_SIZE);
		qtp_temp_layer = text_layer_create(temp_frame);
		text_layer_set_text_alignment(qtp_temp_layer, GTextAlignmentLeft);
		const Tuple *temp_tuple;
		if (qtp_is_degrees_f()) {
			temp_tuple = app_sync_get(&qtp_sync, QTP_WEATHER_TEMP_F_KEY);
		} else {
			temp_tuple = app_sync_get(&qtp_sync, QTP_WEATHER_TEMP_C_KEY);
		}
		if (temp_tuple != NULL) {
			text_layer_set_text(qtp_temp_layer, temp_tuple->value->cstring);
		}
		text_layer_set_font(qtp_temp_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
		layer_add_child(window_get_root_layer(qtp_window), text_layer_get_layer(qtp_temp_layer));

		/* Weather icon layer */
		GRect weather_icon_frame = GRect( QTP_PADDING_X, qtp_weather_y(), QTP_WEATHER_SIZE, QTP_WEATHER_SIZE );
		qtp_weather_icon_layer = bitmap_layer_create(weather_icon_frame);
		bitmap_layer_set_alignment(qtp_weather_icon_layer, GAlignCenter);
		const Tuple *icon_tuple = app_sync_get(&qtp_sync, QTP_WEATHER_ICON_KEY);
		qtp_update_weather_icon(icon_tuple->value->uint8, false, false);
		layer_add_child(window_get_root_layer(qtp_window), bitmap_layer_get_layer(qtp_weather_icon_layer)); 

	}

	/* Battery Logo layer */
	GRect battery_logo_frame = GRect( QTP_PADDING_X, qtp_battery_y(), QTP_BAT_ICON_SIZE, QTP_BAT_ICON_SIZE );
	qtp_battery_image_layer = bitmap_layer_create(battery_logo_frame);
	bitmap_layer_set_bitmap(qtp_battery_image_layer, qtp_battery_image);
	bitmap_layer_set_alignment(qtp_battery_image_layer, GAlignCenter);
	layer_add_child(window_get_root_layer(qtp_window), bitmap_layer_get_layer(qtp_battery_image_layer)); 

	/* Battery Status text layer */
	GRect battery_frame = GRect( 40, qtp_battery_y(), QTP_SCREEN_WIDTH - QTP_BAT_ICON_SIZE, QTP_BAT_ICON_SIZE );
	qtp_battery_text_layer =  text_layer_create(battery_frame);
	text_layer_set_font(qtp_battery_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	qtp_update_battery_status(false);
	layer_add_child(window_get_root_layer(qtp_window), text_layer_get_layer(qtp_battery_text_layer));

	/* Bluetooth Logo layer */
	GRect bluetooth_logo_frame = GRect(QTP_PADDING_X, qtp_bluetooth_y(), QTP_BT_ICON_SIZE, QTP_BT_ICON_SIZE);
	qtp_bluetooth_image_layer = bitmap_layer_create(bluetooth_logo_frame);
	bitmap_layer_set_bitmap(qtp_bluetooth_image_layer, qtp_bluetooth_image);
	bitmap_layer_set_alignment(qtp_bluetooth_image_layer, GAlignCenter);
	layer_add_child(window_get_root_layer(qtp_window), bitmap_layer_get_layer(qtp_bluetooth_image_layer)); 


	/* Bluetooth Status text layer */

	GRect bluetooth_frame = GRect(40,qtp_bluetooth_y(), QTP_SCREEN_WIDTH - QTP_BT_ICON_SIZE, QTP_BT_ICON_SIZE);
	qtp_bluetooth_text_layer =  text_layer_create(bluetooth_frame);
	text_layer_set_font(qtp_bluetooth_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	qtp_update_bluetooth_status(false);
	layer_add_child(window_get_root_layer(qtp_window), text_layer_get_layer(qtp_bluetooth_text_layer));

	/* Invert the screen */
	if (qtp_is_invert()) {
		GRect inverter_frame = GRect(0,0, QTP_SCREEN_WIDTH, QTP_SCREEN_HEIGHT);
		qtp_inverter_layer = inverter_layer_create(inverter_frame);
		layer_add_child(window_get_root_layer(qtp_window), inverter_layer_get_layer(qtp_inverter_layer));
	}

	/* Register for back button */
	//window_set_click_config_provider(qtp_window, (ClickConfigProvider)qtp_click_config_provider);

}


/* Deallocate QTPlus items when window is hidden */
void qtp_deinit() {
	text_layer_destroy(qtp_battery_text_layer);
	text_layer_destroy(qtp_bluetooth_text_layer);
	bitmap_layer_destroy(qtp_bluetooth_image_layer);
	bitmap_layer_destroy(qtp_battery_image_layer);
	if (qtp_is_show_time()) {
		text_layer_destroy(qtp_time_layer);
	}
	if (qtp_is_show_weather()) {
		text_layer_destroy(qtp_temp_layer);
		text_layer_destroy(qtp_weather_desc_layer);
		bitmap_layer_destroy(qtp_weather_icon_layer);
		gbitmap_destroy(qtp_weather_icon);
	}
	if (qtp_is_invert()) {
		inverter_layer_destroy(qtp_inverter_layer);
	}
	window_destroy(qtp_window);
	if (qtp_is_autohide()) {
		app_timer_cancel(qtp_hide_timer);
	}
}

/* Deallocate persistent QTPlus items when watchface exits */
void qtp_app_deinit() {
	if (qtp_conf & QTP_K_SUBSCRIBE) {
		bluetooth_connection_service_unsubscribe();
	}
	gbitmap_destroy(qtp_battery_images[0]);
	gbitmap_destroy(qtp_battery_images[1]);
	gbitmap_destroy(qtp_battery_images[2]);
	gbitmap_destroy(qtp_bluetooth_image);
	app_sync_deinit(&qtp_sync);

}

/* Create window, layers, text. Display QTPlus */
void qtp_show() {
	qtp_init();
	window_stack_push(qtp_window, true);
	if (qtp_is_autohide()) {
		qtp_hide_timer = app_timer_register(QTP_WINDOW_TIMEOUT, qtp_timeout, NULL);
	}
}

/* Hide QTPlus. Free memory */
void qtp_hide() {
	window_stack_pop(true);
	qtp_deinit();
}


bool qtp_is_show_time() {
	return (qtp_conf & QTP_K_SHOW_TIME) == QTP_K_SHOW_TIME;
}
bool qtp_is_show_weather() {
	return (qtp_conf & QTP_K_SHOW_WEATHER) == QTP_K_SHOW_WEATHER;
}
bool qtp_is_autohide() {
	return (qtp_conf & QTP_K_AUTOHIDE) == QTP_K_AUTOHIDE;
}
bool qtp_is_degrees_f() {
	return (qtp_conf & QTP_K_DEGREES_F) == QTP_K_DEGREES_F;
}

bool qtp_is_invert() {
	return (qtp_conf & QTP_K_INVERT) == QTP_K_INVERT;
}

bool qtp_should_vibrate() {
	return (qtp_conf & QTP_K_VIBRATE) == QTP_K_VIBRATE;
}

int qtp_battery_y() {
	if (qtp_is_show_time()) {
		return QTP_BATTERY_BASE_Y + QTP_TIME_HEIGHT + QTP_PADDING_Y;
	} else {
		return QTP_BATTERY_BASE_Y + QTP_PADDING_Y;
	}
}

int qtp_bluetooth_y() {
	if (qtp_is_show_time()) {
		return QTP_BLUETOOTH_BASE_Y + QTP_TIME_HEIGHT + QTP_PADDING_Y;
	} else {
		return QTP_BLUETOOTH_BASE_Y + QTP_PADDING_Y;
	}
}

int qtp_weather_y() {
	if (qtp_is_show_time()) {
		return QTP_WEATHER_BASE_Y + QTP_PADDING_Y + QTP_TIME_HEIGHT;

	} else {
		return QTP_WEATHER_BASE_Y + QTP_PADDING_Y;
	}
}

void qtp_set_config(int config) {
	qtp_conf = config;
}

void qtp_set_timeout(int timeout) {
	QTP_WINDOW_TIMEOUT = timeout;	
}

void qtp_init_bluetooth_status(bool status) {
	qtp_bluetooth_status = status;
}

void init()
{
 //Initialize the app elements
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
  .load = window_load,
  .unload = window_unload,
  });
        
  tick_timer_service_subscribe(SECOND_UNIT, (TickHandler) tick_handler);
  battery_state_service_subscribe(&handle_battery);
  window_stack_push(window, true);
  bluetooth_connection_service_subscribe(&handle_bt);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
  num_animation = persist_exists(NUM_ANIMATION_PKEY) ? persist_read_int(NUM_ANIMATION_PKEY) : NUM_ANIMATION_DEFAULT;
	//This is the old method of fetching an animation number
	//Now it goes in order
    if(num_animation > 5)
        num_animation = 1;        
    {
        GRect start_text,
			start_bt,
			start_date,
			start_batt,
			finish_text = GRect(0, 53, 140, 168),
			finish_date = GRect(0, 97, 150, 140),
			finish_batt = GRect(0, 5, 40, 40),
			finish_bt = GRect(110,-5,40,40);
        switch(num_animation){
        	case 1:{
        		//Slide in from bottom
        		start_text = GRect(0, 600, 140, 168);
          		start_date = GRect(25, 600, 140, 168);
          		start_batt = GRect(0, 600, 40, 40);
         		start_bt = GRect(115, 600, 40, 40);
                break;
        	}
        	case 2:{
        		//Slide in from top
        		start_text = GRect(0, -600, 140, 168);
          		start_date = GRect(0, -600, 140, 168);
          		start_batt = GRect(0, -600, 40, 40);
         		start_bt = GRect(115, -600, 40, 40);
                break;
        	}
        	//Mixed
        	case 3:{
        	//Condensed point slide in and expand from left
        		start_text = GRect(-250, 0, 140, 168);
          		start_date = GRect(-200, 0, 140, 168);
          		start_batt = GRect(-200, 0, 40, 40);
         		start_bt = GRect(-200, 0, 40, 40);
                break;
        	}
        	case 4:{
        	//Condensed point slide in and expand from right
        		start_text = GRect(300, 0, 140, 168);
          		start_date = GRect(300, 0, 140, 168);
          		start_batt = GRect(300, 0, 40, 40);
         		start_bt = GRect(300, 0, 40, 40);
                break;
        	}
        	case 5:{
        		start_text = GRect(300, 250, 140, 168);
          		start_date = GRect(-300, 250, 140, 168);
          		start_batt = GRect(300,-250, 40, 40);
         		start_bt = GRect(-300, -250, 40, 40);
                break;
        	}
        }
        animate_layer(text_layer_get_layer(text_layer), &start_text, &finish_text, 1200, 10);
        animate_layer(text_layer_get_layer(date_text_layer), &start_date, &finish_date, 1130, 10);
        animate_layer(text_layer_get_layer(battery_text_layer), &start_batt, &finish_batt, 1300, 10);
        animate_layer(bitmap_layer_get_layer(bt_connected_layer), &start_bt, &finish_bt, 1400, 10);
    }
	num_animation++;
}
 
void deinit()
{
  //De-initialize elements, saves memory
  //Write the last animation to storage
  persist_write_int(NUM_ANIMATION_PKEY, num_animation);
  animation_unschedule_all();
  qtp_app_deinit();
  tick_timer_service_unsubscribe();
 	battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  window_destroy(window);                                               
}
 
int main(void)
{
	qtp_conf = QTP_K_SHOW_TIME | QTP_K_AUTOHIDE | QTP_K_INVERT | QTP_K_SHOW_WEATHER | QTP_K_DEGREES_F | QTP_K_SUBSCRIBE;
	init();
	qtp_setup();
	app_event_loop();
  deinit();
}