![Demo Image](http://img703.imageshack.us/img703/310/rctx.png)

This plugin allows you to render any HTML website or local HTML file structure to any surface / material in the engine. It also includes a full screen triangle renderer for rendering to screenspace, as opposed to world space.

It is based on the [Awesomium](http://awesomium.com) library which performs the heavy lifting for rendering the page. This plugin uses DirectX11 to render the image buffer to the screen, or to a CryEngine material. The engine can then use the material to render the page on to any object.

Rendering is done in realtime, and thus supports YouTube videos, and even online browser-based games. Yes, you could play a webGL game in your CryEngine game, on a wall.

This plugin has been completely rewritten since the original plugin was released by darman96.

Inspector
==========
You can access the inspector locally by navigating to [http://localhost:3000/](http://localhost:3000).

On default this is on all the time, however, a planned feature is to only enable this while in the game, and use a CVar to update the port / disable the inspector.

Note that the inspector only updates when the engine is rendering... So if you tab out of the engine, it will, on default, pause the rendering and the Inspector will not update until you tab back in again.

Coming Soon
===========
  - Keyboard and Mouse interaction with the views
  - LUA support
  - Console Variables
  - Disable / Enable Inspector

Install Guide / Wiki
==========
[Wiki Index](https://github.com/kidovate/AwesomiumCE3/wiki)

[Install Guide](https://github.com/kidovate/AwesomiumCE3/wiki/Install-Guide)

CVars
=====
Coming soon.

C++ Interface
=====
You will need to include the Awesomium headers, found at the plugin root and `\awesomium\include`.

Finally, you can use this example code:

```
AwesomiumPlugin::IAwesomiumPlugin plugin = PluginManager::safeGetPluginConcreteInterface<IPluginAwesomium*>( "Awesomium" );
AwesomiumPlugin::CAwesomiumSystem sys = plugin->GetSystem();

//Make a view
AwesomiumPlugin::CAwesomiumView* view = sys->CreateView( width, height, "Entity Name", "Full Material Path" );

//Go to YouTube
view->GetView()->LoadURL( WebURL( WSLit( "http://youtube.com/" ) ) );

//Refresh
view->GetView()->Reload(false);
```

The `view` is an Awesomium view, so you have every method [available in their documentation](http://princeofcode.com/software/awesomium/docs/class_awesomium_1_1_web_view.html).


Flownodes
=========
There is a single flow node for creating, controlling, and otherwise manipulating a view in CryEngine.

![Sample FlowGraph](http://img842.imageshack.us/img842/7114/tf60.png)
