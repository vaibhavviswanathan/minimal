/*
Minimal w/ battery and bluetooth
Created by Vaibhav Viswanathan
Open source
Feel free to modify, redistribute and do whatever you'd like with it :)

Based off Clean by Edwin Finch: https://github.com/edwinfinch/firstwatchface/blob/master/src/main.c
Trek font taken from PebbleTrek by Kyle Potts: https://github.com/kylepotts/pebbletrek
*/

#include <pebble.h>
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
  tick_timer_service_unsubscribe();
 	battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  window_destroy(window);                                               
}
 
int main(void)
{
	init();
	app_event_loop();
  deinit();
}