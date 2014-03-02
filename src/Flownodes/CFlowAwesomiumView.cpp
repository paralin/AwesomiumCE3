/* D3D_Plugin - for licensing and copyright see license.txt */

#include <StdAfx.h>

#include <CPluginAwesomium.h>
#include <IPluginManager.h>
#include "../CAwesomiumSystem.h"
#include "../CAwesomiumView.h"
#include <Nodes/G2FlowBaseNode.h>
#include <stdio.h>
#include <sstream> //for string stuff

namespace AwesomiumPlugin
{
class CFlowAwesomiumView :
    public CFlowBaseNode
{
    enum EInputPorts
    {
        //Create the view and start updating the texture
        EIP_START = 0,
        //Delete the view and release the texture
        EIP_RELEASE,
        EIP_MATNAME,
        EIP_OBJNAME,
        EIP_WIDTH,
        EIP_HEIGHT,
        //Stop loading the page
        EIP_STOP,
        //Refresh the page
        EIP_REFRESH,
        //Go to a URL
        EIP_TOURL,
        EIP_GOBACK,
        EIP_GOFORWARD,
        EIP_RESIZE,
        EIP_ZOOMIN,
        EIP_ZOOMOUT,
        EIP_SETZOOM,
        EIP_UNDO,
        EIP_REDO,
        EIP_COPY,
        EIP_PASTE,
        EIP_SELECTALL,
        EIP_GETZOOM,
        EIP_GETTITLE,
        EIP_GETURL,
        EIP_GETLOADING
    };

    enum EOutputPorts
    {
        EOP_STARTED = 0,
        EOP_RELEASED,
        //On page load
        EOP_LOADED,
        EOP_URL,
        EOP_TITLE,
        EOP_LOADING,
        EOP_ZOOM
    };

public:
    virtual void GetMemoryUsage( ICrySizer* s ) const
    {
        s->Add( *this );
    }

    CFlowAwesomiumView( SActivationInfo* pActInfo )
    {
    }

    virtual void GetConfiguration( SFlowNodeConfig& config )
    {
        static const SInputPortConfig inputs[] =
        {
            InputPortConfig_Void( "Start", _HELP( "Create and begin updating the view." ) ),
            InputPortConfig_Void( "Release", _HELP( "Shutdown and release the view")),
            InputPortConfig<string>("MaterialName", _HELP("Name of the material")),
            InputPortConfig<string>("ObjectName", _HELP("Name of the object to render to")),
            InputPortConfig<int>("Width", _HELP("Width of the view")),
            InputPortConfig<int>("Height", _HELP("Height of the view")),
            InputPortConfig_Void( "Stop", _HELP("Stop page loading.")),
            InputPortConfig_Void( "Refresh", _HELP("Refresh the page")),
            InputPortConfig<string>("GoTo", _HELP("Go to a URL")),
            InputPortConfig_Void("Back", _HELP("Go to the last page")),
            InputPortConfig_Void("Forward", _HELP("Go to the next page")),
            InputPortConfig<vec3>("Resize", _HELP("Change the size of the view")),
            InputPortConfig_Void("Zoom In", _HELP("Zoom in the page")),
            InputPortConfig_Void("Zoom Out", _HELP("Zoom out the page")),
            InputPortConfig<int>("SetZoom", _HELP("Set the zoom level")),
            InputPortConfig_Void("Undo", _HELP("Perform the 'undo' action")),
            InputPortConfig_Void("Redo", _HELP("Redo a undone action.")),
            InputPortConfig_Void("Copy", _HELP("Copy selected text")),
            InputPortConfig_Void("Paste", _HELP("Paste copied text")),
            InputPortConfig_Void("Select All", _HELP("Select all text")),
            InputPortConfig_Void("GetZoom", _HELP("Output the zoom")),
            InputPortConfig_Void("GetTitle", _HELP("Output the title")),
            InputPortConfig_Void("GetURL", _HELP("Get the current URL")),
            InputPortConfig_Void("GetLoading", _HELP("Output if view is loading")),
            InputPortConfig_Null(),
        };

        static const SOutputPortConfig outputs[] =
        {
            OutputPortConfig<bool>( "Started", _HELP( "The view was created and is working properly." ) ),
            OutputPortConfig<bool>( "Released", _HELP( "The view was shut down and released." ) ),
            OutputPortConfig<bool>( "OnLoaded", _HELP( "Called when the view has finished loading." ) ),
            OutputPortConfig<string>( "URL", _HELP( "Current URL" ) ),
            OutputPortConfig<string>( "Title", _HELP( "Current title" ) ),
            OutputPortConfig<bool>( "Loading", _HELP( "Is the view loading?" ) ),
            OutputPortConfig<int>( "Zoom", _HELP( "Current zoom level" ) ),
            OutputPortConfig_Null(),
        };

        config.pInputPorts = inputs;
        config.pOutputPorts = outputs;
        config.sDescription = _HELP( "Instance of a WebView" );

        config.SetCategory( EFLN_APPROVED );
    }

    virtual void ProcessEvent( EFlowEvent evt, SActivationInfo* pActInfo )
    {
        this->awesomium = PluginManager::safeGetPluginConcreteInterface<IPluginAwesomium*>( PLUGIN_NAME );
        if(awesomium == NULL)
          return;

        this->sys = this->awesomium->GetSystem();
        switch ( evt )
        {
        case eFE_Activate:

            if ( IsPortActive( pActInfo, EIP_START ) )
            {
                ActivateOutput<bool>( pActInfo, EOP_STARTED, StartView(GetPortString(pActInfo, EIP_MATNAME), GetPortString(pActInfo, EIP_OBJNAME) ));
            }
            break;
        }
    }
private:
    bool m_started;
    IPluginAwesomium * awesomium;
    CAwesomiumSystem * sys;
    CAwesomiumView * view;

    virtual bool StartView(int width, int height, string matName, string objName)
    {
      if(m_started) return true; 
      view = sys->CreateView(width, height, matName.c_str(), objName.c_str());
    }
}
}

REGISTER_FLOW_NODE_EX( "AwesomiumPlugin:View",  AwesomiumPlugin::CFlowAwesomiumView );
