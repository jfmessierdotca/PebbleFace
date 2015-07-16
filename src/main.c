#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  int min, heure, min_hex, min_uni16, heure_hex, heure_uni16, min_oct, min_uni8, heure_oct, heure_uni8 ;
  min = tick_time->tm_min;
  heure = tick_time->tm_hour;
    
    // Create a long-lived buffer
  static char buffer[] = "  00:00   00:00";
    
  // Check for 24/12 hour format
  if(clock_is_24h_style() == false) {
    //Use 12h hour format
    if (heure > 12) { heure -= 12; } 
  }

  heure_hex = heure / 16;
  heure_uni16 = heure % 16;
  min_hex = min / 16;
  min_uni16 = min % 16;

  heure_oct = heure / 8;
  heure_uni8 = heure % 8;
  min_oct = min / 8;
  min_uni8 = min % 8;
  buffer[0] = 'x';
  buffer[1] = ':';
  buffer[2] = ( heure_hex<10 ? '0' + heure_hex : 55 + heure_hex );
  buffer[3] = ( heure_uni16<10 ? '0' + heure_uni16 : 55 + heure_uni16 );
  buffer[4] = ':';
  buffer[5] = ( min_hex<10 ? '0' + min_hex : 55 + min_hex ); 
  buffer[6] = ( min_uni16<10 ? '0' + min_uni16 : 55 + min_uni16 ); 
  buffer[7] = 0x20; // a Space in HEX
  buffer[8] = 'o';
  buffer[9] = ':';  
  buffer[10] = '0' + heure_oct;
  buffer[11] = '0' + heure_uni8;
  buffer[12] = ':';
  buffer[13] = '0' + min_oct;
  buffer[14] = '0' + min_uni8;
  
  // Display this time on the TextLayer
  text_layer_set_overflow_mode(s_time_layer, GTextOverflowModeWordWrap);
  text_layer_set_text(s_time_layer, buffer);
  //text_layer_set_text(s_time_layer, buffer);
}

static void main_window_load(Window *window) {
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 25, 144, 100));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00 00:00");

  // Improve the layout to be more like a watchface
  GFont custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CHECK_38));
  text_layer_set_font(s_time_layer, custom_font);  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}


