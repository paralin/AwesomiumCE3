/* Awesomium_Plugin - for licensing and copyright see license.txt */

#include <StdAfx.h>
#include <CPluginAwesomium.h>
#include <IUIDraw.h>
#include <IEntitySystem.h>

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

        InitAwesomium();

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

    bool CPluginAwesomium::InitAwesomium()
    {
        if ( !gEnv || !gEnv->pGame->GetIGameFramework() )
        {
            gEnv->pLog->LogError( "Failed to initialize Awesomium, no gameframework found. This is probably caused by calling the Init method too soon" );
            return false;
        }

        IGameFramework* pGameFramework = gEnv->pGame->GetIGameFramework();
        pGameFramework->RegisterListener( this, "AwesomiumCE3", eFLPriority_Default );

        WebConfig config;
        //todo: check if we are in debug mode - if in Editor then show the inspector on localhost:3000
        config.remote_debugging_port = 3000;
        m_pWebCore = WebCore::Initialize( config );

        return true;

    }

    void CPluginAwesomium::Shutdown()
    {
    }

    void CPluginAwesomium::OnPostUpdate( float fDeltaTime )
    {
        WebCore::instance()->Update();

        //todo: foreach element, render
    }

    void CPluginAwesomium::OnSaveGame( ISaveGame* pSaveGame )
    {
    }

    void CPluginAwesomium::OnLoadGame( ILoadGame* pLoadGame )
    {
    }

    void CPluginAwesomium::OnLevelEnd( const char* nextLevel )
    {
    }

    void CPluginAwesomium::OnActionEvent( const SActionEvent& event )
    {
    }

    void CPluginAwesomium::OnPreRender()
    {
    }


    bool CPluginAwesomium::CheckDependencies() const
    {
        bool bRet = CPluginBase::CheckDependencies();

        return bRet;
    }

    bool CPluginAwesomium::Release( bool bForce )
    {
        bool bRet = true;

        if ( !m_bCanUnload )
        {
            // Should be called while Game is still active otherwise there might be leaks/problems
            bRet = CPluginBase::Release( bForce );

            if ( bRet )
            {
                // TODO: Do your own cleanups here
                // Cleanup like this always (since the class is static its cleaned up when the dll is unloaded)
                gPluginManager->UnloadPlugin( GetName() );

                // Allow Plugin Manager garbage collector to unload this plugin
                AllowDllUnload();
            }
        }

        return bRet;
    };

    bool CPluginAwesomium::InitDependencies()
    {
        return CPluginBase::InitDependencies();
    }
}