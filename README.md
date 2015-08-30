Video Embed Field
-----

Video Embed field creates a simple field type called Video Embed that allows you to embed videos from YouTube and Vimeo or show their thumbnail previews simply by entering the video's url.

Configuration
----

There are multiple options to configure the way that the embedded video will act, based on the provider of the video. This is configured by setting up Video Styles which can then be applied as a formatter setting. You can access these settings in your site at admin/config/media/vef_video_styles.
  * The admin/config/media/vef_video_styles is not currently working.  I will file a bug in the issue queue.
  
The module also provides support for retrieving and displaying thumbnails for each video via a field formatter. This way you can configure your detail page to show the video, while any listings simply show the thumbnail. This is setup via field formatter, so it will work through display modes and views equally well. The thumbnail is retrieved automatically based on the video's url.

For Developers
----

This module provides a hook system should you need to add other providers. See the video_embed_field.api.php file for more information. Video Embed Styles are ctools exportables for your convenience.

Why another video related module?
----

This module provides a very simple solution to a simple problem : 'User needs to be able to embed videos from youtube or vimeo' . It does not store any video files on the site or manage media types of any sort - it simply stores a url and prepares it into embed code.

This module is a nice complement to the Video module in that video allows you to store videos locally on your site while this module allows you to reference remote videos from Youtube and Vimeo.


Original Drupal development was sponsored by Phase2 Technology
----

Since January 2014, this module is maintained by plopesc
Originally maintained by jec006

Supporting organizations: 
Bluespark
Maintenance support

Project Information
----

Ported to Backdrop by serundeputy

Looking for maintainer(s) 