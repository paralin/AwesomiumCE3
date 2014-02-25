#include "StdAfx.h"
#include "CAwesomiumSystem.h"

#include "CPluginAwesomium.h"

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

WebCore* AwesomiumPlugin::CAwesomiumSystem::m_pWebCore = NULL;
namespace AwesomiumPlugin
{
    CAwesomiumSystem::CAwesomiumSystem( void )
    {
        IGameFramework* pGameFramework = gEnv->pGame->GetIGameFramework();
        pGameFramework->RegisterListener( this, "AwesomiumCE3", eFLPriority_Default );

        WebConfig config;
        //todo: check if we are in debug mode - if in Editor then show the inspector on localhost:3000
        config.remote_debugging_port = 3000;
        m_pWebCore = WebCore::Initialize( config );
    }


    CAwesomiumSystem::~CAwesomiumSystem( void )
    {
        // unregister listeners
        if ( gEnv )
        {
            if ( gEnv->pGame && gEnv->pGame->GetIGameFramework() )
            {
                gEnv->pGame->GetIGameFramework()->UnregisterListener( this );
            }
        }

        if ( m_pWebCore )
        {
            m_pWebCore->Shutdown();
            m_pWebCore = NULL;
        }

        if ( m_FullscreenDrawer )
        {
            delete m_FullscreenDrawer;
        }
    }

    void CAwesomiumSystem::OnPreRender()
    {
        m_pWebCore->Update();
    }

    void CAwesomiumSystem::OnSaveGame( ISaveGame* pSaveGame )
    {
    }

    void CAwesomiumSystem::OnLoadGame( ILoadGame* pLoadGame )
    {
    }

    void CAwesomiumSystem::OnLevelEnd( const char* nextLevel )
    {
    }

    void CAwesomiumSystem::OnActionEvent( const SActionEvent& event )
    {
    }

    void CAwesomiumSystem::OnPrePresent()
    {
        if ( !m_FullscreenDrawer )
        {
            m_FullscreenDrawer = new CFullscreenTriangleDrawer();
        }

        // Needs to be implemented
        if ( m_hudView )
        {
            void* pHUDTexture = m_hudView->GetTexture();

            if ( pHUDTexture )
            {
                m_FullscreenDrawer->Draw( pHUDTexture );
            }
        }
    }

    void CAwesomiumSystem::OnPreReset()
    {
        if ( m_hudView )
        {
            m_hudView->ReleaseTexture();
        }

        for ( std::vector<CAwesomiumView>::iterator iter = m_views.begin(); iter != m_views.end(); ++iter )
        {
            iter->SetTexture( NULL, 0 );
        }

        delete m_FullscreenDrawer;
        m_FullscreenDrawer = NULL;
    }

    void CAwesomiumSystem::OnPostReset()
    {
        m_FullscreenDrawer = new CFullscreenTriangleDrawer();

        if ( m_hudView )
        {
            m_hudView->Resize( gEnv->pRenderer->GetWidth(), gEnv->pRenderer->GetHeight() );
        }
    }

    void CAwesomiumSystem::OnPostBeginScene()
    {
        //ExecutePendingCreateSurfaceTasks();

        if ( m_hudView )
        {
            // Note that calling any ViewListener's OnDraw will be called in this stack.
            // Since we're in the render thread, it's safe to do drawing in the OnDraw method.
            m_hudView->DoDraw();
        }
    }

    void CAwesomiumSystem::OnPostUpdate( float fDeltaTime )
    {
        SetTexturesForListeners();

        m_pWebCore->Update();

        if ( m_hudView )
        {
            UpdateHUD();
        }
    }

    void CAwesomiumSystem::SetTexturesForListeners()
    {
        CAwesomiumView* pListener = NULL;

        // Create HUD texture
        pListener = m_hudView;

        if ( pListener && pListener->GetTexture() == NULL )
        {
            void* pD3DTextureDst = NULL;
            ITexture* pCryTex = gD3DSystem->CreateTexture(
                                    &pD3DTextureDst,
                                    gEnv->pRenderer->GetWidth(),
                                    gEnv->pRenderer->GetHeight(),
                                    1,
                                    eTF_A8R8G8B8,
                                    FT_USAGE_DYNAMIC
                                );

            pListener->SetTexture( pD3DTextureDst, pCryTex->GetTextureID() );
        }

        // Create textures for entities
        for ( std::vector<CAwesomiumView>::iterator iter = m_views.begin(); iter != m_views.end(); ++iter )
        {
            if ( pListener && pListener->GetTexture() == NULL )
            {
                ChangeEntityDiffuseTextureForMaterial( pListener, pListener->GetEngineObjectName(), pListener->GetOverriddenMaterialName() );
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

    void CAwesomiumSystem::UpdateHUD()
    {
        static Vec3 lastPosition = Vec3Constants<Vec3::value_type>::fVec3_Zero;
        static float lastRotation = 0;

        CAwesomiumView* pHUDView = m_hudView;

        if ( pHUDView )
        {
            CCamera& camera = gEnv->pSystem->GetViewCamera();
            Vec3 viewDir = camera.GetViewdir();
            float rotation = cry_atan2f( viewDir.y, viewDir.x ) * 180.0f / 3.14159f;
            // Adjust rotation so it is the same as in the game
            rotation = -rotation - 135.0f;
            Awesomium::WebView* pView = pHUDView->GetView();

            if ( pView )
            {
                if ( rotation != lastRotation )
                {
                    //Translate this to javascript events
                    //pView->TriggerEvent( "SetAbsoluteCompassRotation", rotation );
                    //pView->TriggerEvent( "SetPlayerRotationOnMap", rotation - 45.0f );

                    lastRotation = rotation;
                }

                Vec3 cameraPosition = camera.GetPosition();

                if ( ( cameraPosition - lastPosition ).GetLengthSquared() > VEC_EPSILON )
                {
                    //Translate to javascript event
                    //pView->TriggerEvent( "SetPlayerPositionOnMap", cameraPosition.x, cameraPosition.y );

                    lastPosition = cameraPosition;
                }
            }
        }
    }

}