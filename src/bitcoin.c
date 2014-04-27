#include <pebble.h>

#include "entry.h"

static Window *window;
static TextLayer *symbol_layer;
static TextLayer *price_layer;
static TextLayer *console_layer;
static TextLayer *title_layer;
static ActionBarLayer *action_bar;
static char rate[10];
static int symbol = 1;
static int error_cnt = 1;

static GBitmap *action_icon_up;
static GBitmap *action_icon_down;
static GBitmap *action_icon_select;

// This is a custom defined key for saving our symbol field
#define SYMBOL_KEY 1

        enum {
            AKEY_NUMBER = 1,
            AKEY_SYMBOL = 2,
            AKEY_TEXT,
        };

static void fetch_msg(void) {
	
 DictionaryIterator *iter;
 app_message_outbox_begin(&iter);
  	
 Tuplet rate_tuple = TupletInteger(AKEY_NUMBER, AKEY_TEXT);
if(symbol == 1)
{
 Tuplet symbol_tuple = TupletCString(AKEY_SYMBOL, "USD: ");
  dict_write_tuplet(iter, &symbol_tuple);
}
if(symbol == 2)
{
 Tuplet symbol_tuple = TupletCString(AKEY_SYMBOL, "EUR: ");	
  dict_write_tuplet(iter, &symbol_tuple);
}
if(symbol == 3)
{
 Tuplet symbol_tuple = TupletCString(AKEY_SYMBOL, "GBP: ");	
  dict_write_tuplet(iter, &symbol_tuple);
}
 dict_write_tuplet(iter, &rate_tuple);

 app_message_outbox_send();
 
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // refresh
  text_layer_set_text(console_layer, "Loading...");
  fetch_msg();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context)
{
  text_layer_set_text(console_layer, "Loading...");
  symbol = symbol - 1;
  if(symbol < 1)
  {
	  symbol = 3;
  }
  fetch_msg();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context)
{
  text_layer_set_text(console_layer, "Loading...");
  symbol = symbol + 1;
  if(symbol > 3)
  {
	  symbol = 1;
  }
  fetch_msg();
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  persist_write_int(SYMBOL_KEY, symbol);
  text_layer_set_text(console_layer, "Default Changed");
}


static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, select_long_click_handler, NULL);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {

  error_cnt = 1;	
	
  Tuple *rate_tuple = dict_find(iter, AKEY_NUMBER);
  Tuple *symbol_tuple = dict_find(iter, AKEY_SYMBOL);

  if (rate_tuple) {
 
 if(symbol == 1)
{
    strcpy(rate, "$");
    strcat(rate, rate_tuple->value->cstring);
    text_layer_set_text(symbol_layer, "USD: "); 
}
if(symbol == 2)
{
    strcpy(rate, "\u20AC");
    strcat(rate, rate_tuple->value->cstring);
    text_layer_set_text(symbol_layer, "EUR: "); 
}
if(symbol == 3)
{
	strcpy(rate, "\u00a3");
    strcat(rate, rate_tuple->value->cstring);
    text_layer_set_text(symbol_layer, "GBP: "); 
}

    
    text_layer_set_text(price_layer, rate);  
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "Text: %s", rate_tuple->value->cstring);
  }
  //else {
	  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Error: rate_tuple blank!");
  //}
    text_layer_set_text(console_layer, "");
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped!");
       psleep(500); 
  	   if (error_cnt < 4) 
       {
       fetch_msg();
       text_layer_set_text(console_layer, "");
       error_cnt = error_cnt + 1;
       }
       
       if (error_cnt == 4) 
       {
       text_layer_set_text(console_layer, "Connection Lost");
       }

}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Failed to Send!");
  psleep(500);
	   if (error_cnt < 4) 
       {
       fetch_msg();
       text_layer_set_text(console_layer, "");
       error_cnt = error_cnt + 1;
       }
       
       if (error_cnt == 4) 
       {
       text_layer_set_text(console_layer, "No Connection");
       }
}

static void app_message_init(void) {
  // Register message handlers
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_failed(out_failed_handler);
  // Init buffers
  app_message_open(64, 64);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  symbol_layer = text_layer_create(
      (GRect) { .origin = { 0, 10 }, .size = { bounds.size.w - ACTION_BAR_WIDTH, 50 } });
  text_layer_set_text(symbol_layer, "USD: "); text_layer_set_text_alignment(symbol_layer, GTextAlignmentCenter); text_layer_set_font(symbol_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(symbol_layer));
  
        title_layer = text_layer_create(
      (GRect) { .origin = { 0, 125}, .size = { bounds.size.w - ACTION_BAR_WIDTH, 15 } });
  text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
  text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(window_layer, text_layer_get_layer(title_layer));
  text_layer_set_text(title_layer, "Powered by CoinDesk");

  
    console_layer = text_layer_create(
      (GRect) { .origin = { 0, 95 }, .size = { bounds.size.w - ACTION_BAR_WIDTH, 30 } });
  text_layer_set_text(console_layer, "Loading...");
  text_layer_set_text_alignment(console_layer, GTextAlignmentCenter);
  text_layer_set_font(console_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(console_layer));

  price_layer = text_layer_create(
      (GRect) { .origin = { 0, 60 }, .size = { bounds.size.w - ACTION_BAR_WIDTH, 30 } });
  text_layer_set_text(price_layer, "$0000.00");
  text_layer_set_text_alignment(price_layer, GTextAlignmentCenter);
  text_layer_set_font(price_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  layer_add_child(window_layer, text_layer_get_layer(price_layer));
  
  action_icon_up = gbitmap_create_with_resource(RESOURCE_ID_UP_ICON);
  action_icon_down = gbitmap_create_with_resource(RESOURCE_ID_DOWN_ICON);
  action_icon_select = gbitmap_create_with_resource(RESOURCE_ID_REFRESH_ICON); 
  
  action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar, window);
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider);

  action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, action_icon_up);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, action_icon_down);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, action_icon_select);

}

static void window_unload(Window *window) {
  text_layer_destroy(symbol_layer);
  text_layer_destroy(price_layer);
  text_layer_destroy(console_layer);
  text_layer_destroy(title_layer);
  
  action_bar_layer_destroy(action_bar);
}

static void init(void) {
  window = window_create();
  app_message_init();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
  if (persist_exists(SYMBOL_KEY)) 
  {
     symbol = persist_read_int(SYMBOL_KEY);
       if (symbol > 3 || symbol < 1) 
       {
       symbol = 1;
       }
  }
  psleep(100);
}

static void deinit(void) {
  window_destroy(window);
  
  gbitmap_destroy(action_icon_up);
  gbitmap_destroy(action_icon_down);
  gbitmap_destroy(action_icon_select);
}

int main(void) {
  init();

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
  fetch_msg();
  app_event_loop();
  deinit();
}
