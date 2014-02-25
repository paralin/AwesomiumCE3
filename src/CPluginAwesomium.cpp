/* Awesomium_Plugin - for licensing and copyright see license.txt */

#include <StdAfx.h>
#include <CPluginAwesomium.h>
#include <IUIDraw.h>
#include <IEntitySystem.h>

D3DPlugin::IPluginD3D* AwesomiumPlugin::gD3DSystem = NULL;
void* AwesomiumPlugin::gD3DDevice = NULL;
namespace AwesomiumPlugin
{
    CPluginAwesomium* gPlugin = NULL;


    CPluginAwesomium::CPluginAwesomium() : m_bEnablePlugins( false )
    {
        gPlugin = this;
    }

    CPluginAwesomium::~CPluginAwesomium()
    {
        gPlugin = NULL;
    }


    bool CPluginAwesomium::Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager, const char* sPluginDirectory )
    {
        gPluginManager = ( PluginManager::IPluginManager* )pPluginManager->GetConcreteInterface( NULL );
        CPluginBase::Init( env, startupParams, pPluginManager, sPluginDirectory );

        if ( gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting() )
        {
            InitAwesomium();
        }

        return true;
    }

    const char* CPluginAwesomium::ListCVars() const
    {
        return ""; // TODO: Enter CVARs/Commands here if you have some
    }

    const char* CPluginAwesomium::GetStatus() const
    {
        return "OK";
    }

    void CPluginAwesomium::InitAwesomium()
    {
        if ( !gEnv || !gEnv->pGame->GetIGameFramework() )
        {
            gEnv->pLog->LogError( "Failed to initialize Awesomium, no gameframework found. This is probably caused by calling the Init method too soon" );
            return;
        }

        if ( !g_system )
        {
            g_system = new CAwesomiumSystem();

            if ( gD3DSystem )
            {
                gD3DSystem->RegisterListener( g_system );
            }

            else
            {
                gEnv->pLog->LogError( "Failed to initialize Awesomium, no D3D device found. Probably InitAwesomium() was called before gD3D device was initialized." );
            }
        }
    }

    bool CPluginAwesomium::CheckDependencies() const
    {
        bool bRet = CPluginBase::CheckDependencies();

        if ( bRet )
        {
            bRet = PluginManager::safeGetPluginConcreteInterface<D3DPlugin::IPluginD3D*>( "D3D" );
        }

        return bRet;
    }

    void CPluginAwesomium::ShutdownAwesomium()
    {
        //todo: shutdown existing UI elements
        if ( g_system )
        {
            delete g_system;
            g_system = NULL;
        }
    }

    bool CPluginAwesomium::Release( bool bForce )
    {
        bool bRet = true;

        if ( !m_bCanUnload )
        {
            bRet = CPluginBase::Release( bForce );

            if ( bRet )
            {
                ShutdownAwesomium();

                if ( gD3DSystem )
                {
                    PluginManager::safeReleasePlugin( "D3D", gD3DSystem );
                }

                gPluginManager->UnloadPlugin( GetName() );
                AllowDllUnload();
            }
        }

        return bRet;
    };

    bool CPluginAwesomium::InitDependencies()
    {
        if ( gEnv && gEnv->pSystem && !gEnv->pSystem->IsQuitting() )
        {
            gD3DSystem = PluginManager::safeUsePluginConcreteInterface<D3DPlugin::IPluginD3D*>( "D3D" );

            if ( gD3DSystem )
            {
                gD3DDevice = gD3DSystem->GetDevice();
            }
        }

        return CPluginBase::InitDependencies();
    }
}