<?php
/**
 *  API Info for video_embed_field module
 */
 
 /**
  *  Creates a hook that other modules can implement to get handlers - 
  *  hook_video_embed_handler_info
  *  Can be used to add more handlers if needed - from other modules and such
  *  Handler should be an array of the form
  *  array(
  *    'function' => 'function_name_to_call' //should be of the signature function_name($url, $teaser=FALSE) and should return the embed code
  *    'domain' => 'youtube.com' //the domain that this handler will create embed code for
  *    'form' => 'function_name_for_form' //function to create settings form (optional)
  *    'title' => 'Title' //The title of the handler - to be used as the field group header - will be wrapped with t()
  *  );
  */
 function hook_video_embed_handler_info(){
 
 }