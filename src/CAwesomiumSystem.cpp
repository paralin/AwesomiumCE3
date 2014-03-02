#include "StdAfx.h"
#include "CAwesomiumSystem.h"

#include "CPluginAwesomium.h"

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

//#undef TEST_VIEW
bool AwesomiumPlugin::CAwesomiumSystem::g_WebCoreInit = false;
namespace AwesomiumPlugin
{
    CAwesomiumSystem::CAwesomiumSystem( void )
    {
        WebConfig config;
        //todo: check if we are in debug mode - if in Editor then show the inspector on localhost:3000

        if ( !g_WebCoreInit )
        {
            config.remote_debugging_port = 3000;
            g_WebCoreInit = true;
            WebCore::Initialize( config );
        }

        if ( gD3DSystem )
        {
            gD3DSystem->RegisterListener( this );
        }

        else
        {
            gEnv->pLog->LogError( PLUGIN_CONSOLE_PREFIX " D3D not initialized when initing CAwesomiumSystem.." );
        }

        IGameFramework* pGameFramework = gEnv->pGame->GetIGameFramework();
        pGameFramework->RegisterListener( this, "CAwesomiumSystem", eFLPriority_Default );

#ifdef TEST_VIEW
        CAwesomiumView* view = new CAwesomiumView( 1000, 1000, "TestScreen", "Materials/ReplacedSquareUI" );
        m_views.push_back( view );
#endif
    }


    CAwesomiumSystem::~CAwesomiumSystem( void )
    {
        if ( gD3DSystem )
        {
            gD3DSystem->UnregisterListener( this );
        }

        for ( std::vector<CAwesomiumView*>::iterator iter = m_views.begin(); iter != m_views.end(); ++iter )
        {
            CAwesomiumView* view = ( *iter );
            delete view;
        }

        m_views.clear();

        if ( g_WebCoreInit )
        {
            //Could cause issues if all of the subviews are not de-inited first
            WebCore::Shutdown();
            g_WebCoreInit = false;
        }

        gEnv->pLog->Log( PLUGIN_CONSOLE_PREFIX " Awesomium System unloaded..." );
    }

    void CAwesomiumSystem::OnPrePresent()
    {

        if ( g_WebCoreInit )
        {
            WebCore::instance()->Update();
        }
    }

    void CAwesomiumSystem::OnPreReset()
    {

        for ( std::vector<CAwesomiumView*>::iterator iter = m_views.begin(); iter != m_views.end(); ++iter )
        {
            ( *iter )->SetTexture( NULL, 0 );
        }
    }

    void CAwesomiumSystem::OnPostReset()
    {
    }

    void CAwesomiumSystem::OnPostBeginScene()
    {
        //ExecutePendingCreateSurfaceTasks();

        for ( std::vector<CAwesomiumView*>::iterator iter = m_views.begin(); iter != m_views.end(); ++iter )
        {
            ( *iter )->DoDraw();
        }
    }

    void CAwesomiumSystem::SetTexturesForListeners()
    {
        // Create textures for entities
        for ( std::vector<CAwesomiumView*>::iterator iter = m_views.begin(); iter != m_views.end(); ++iter )
        {
            CAwesomiumView* view = ( *iter );

            if ( view && view->GetTexture() == NULL )
            {
                ChangeEntityDiffuseTextureForMaterial( view, view->GetEngineObjectName(), view->GetOverriddenMaterialName() );
            }
        }
    }

    void CAwesomiumSystem::OnPostPresent()
    {
    }

    void CAwesomiumSystem::ChangeEntityDiffuseTextureForMaterial( CAwesomiumView* pViewListener, const char* entityName, const char* materialName )
    {
        IMaterial* pMaterial = gEnv->p3DEngine->GetMaterialManager()->FindMaterial( materialName );
        IEntity* pEntity = gEnv->pEntitySystem->FindEntityByName( entityName );

        if ( pEntity && !pMaterial )
        {
            pMaterial = pEntity->GetMaterial();
        }

        if ( pMaterial )
        {
            STexSamplerRT& sampler = pMaterial->GetShaderItem().m_pShaderResources->GetTexture( EFTT_DIFFUSE )->m_Sampler;

            // Create a new texture and scrap the old one
            void* pD3DTextureDst = NULL;
            ITexture* pCryTex = gD3DSystem->CreateTexture(
                                    &pD3DTextureDst,
                                    sampler.m_pITex->GetWidth(),
                                    sampler.m_pITex->GetHeight(),
                                    1,
                                    eTF_A8R8G8B8,
                                    FT_USAGE_DYNAMIC
                                );

            int oldTextureID = sampler.m_pITex->GetTextureID();
            gEnv->pRenderer->RemoveTexture( oldTextureID );
            sampler.m_pITex = pCryTex;
            pCryTex->AddRef();

            pViewListener->SetTexture( pD3DTextureDst, pCryTex->GetTextureID() );
        }
    }

    //IGameFramework
    void CAwesomiumSystem::OnPostUpdate( float fDeltaTime )
    {
        SetTexturesForListeners();
    }

    void CAwesomiumSystem::OnSaveGame( ISaveGame* pSaveGame )
    {
    }

    void CAwesomiumSystem::OnLoadGame( ILoadGame* pLoadGame )
    {
    }

    void CAwesomiumSystem::OnLevelEnd( const char* nextLevel )
    {
        for ( std::vector<CAwesomiumView*>::iterator iter = m_views.begin(); iter != m_views.end(); ++iter )
        {
            ( *iter )->SetTexture( NULL, 0 );
        }
    }

    void CAwesomiumSystem::OnActionEvent( const SActionEvent& event )
    {
    }

    void CAwesomiumSystem::OnPreRender()
    {
    }
}
