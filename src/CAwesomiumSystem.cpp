#include "StdAfx.h"
#include "CAwesomiumSystem.h"

#include "CPluginAwesomium.h"

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

bool AwesomiumPlugin::CAwesomiumSystem::g_WebCoreInit = false;
namespace AwesomiumPlugin
{
    CAwesomiumSystem::CAwesomiumSystem( void )
        : m_FullscreenDrawer( NULL ),
          m_hudView( NULL )
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
            gEnv->pLog->Log( PLUGIN_CONSOLE_PREFIX " Registered CAwesomiumSystem in D3D..." );
        }

        else
        {
            gEnv->pLog->LogError( PLUGIN_CONSOLE_PREFIX " D3D not initialized when initing CAwesomiumSystem.." );
        }
    }


    CAwesomiumSystem::~CAwesomiumSystem( void )
    {
        if ( gD3DSystem )
        {
            gD3DSystem->UnregisterListener( this );
        }

        if ( g_WebCoreInit )
        {
            //Could cause issues if all of the subviews are not de-inited first
            WebCore::Shutdown();
            g_WebCoreInit = false;
        }

        if ( m_FullscreenDrawer )
        {
            CryLogAlways( PLUGIN_CONSOLE_PREFIX " Deleting fullscreen drawer" );
            delete m_FullscreenDrawer;
        }

        gEnv->pLog->Log( PLUGIN_CONSOLE_PREFIX " Awesomium System unloaded..." );
    }

    //This code needs to go somewhere: (OnPreUpdate)
    /*
        SetTexturesForListeners();
        m_pWebCore->Update();

        if ( m_hudView )
        {
            UpdateHUD();
        }
    */

    void CAwesomiumSystem::OnPrePresent()
    {
        if ( !m_FullscreenDrawer )
        {
            CryLogAlways( PLUGIN_CONSOLE_PREFIX " Creating a new TriangleDrawer in OnPrePresent" );
            m_FullscreenDrawer = new CFullscreenTriangleDrawer();
            CryLogAlways( PLUGIN_CONSOLE_PREFIX " Fullscreen drawer created. " );
        }

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

        //Free the fullscreen drawer in reset
        CryLogAlways( PLUGIN_CONSOLE_PREFIX "Deleting fullscreen drawer" );
        delete m_FullscreenDrawer;
        m_FullscreenDrawer = NULL;
    }

    void CAwesomiumSystem::OnPostReset()
    {
        if ( !m_FullscreenDrawer )
        {
            CryLogAlways( PLUGIN_CONSOLE_PREFIX " Creating a new TriangleDrawer in OnPostReset" );
            m_FullscreenDrawer = new CFullscreenTriangleDrawer();
            CryLogAlways( PLUGIN_CONSOLE_PREFIX " Fullscreen drawer created. " );
        }

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
            m_hudView->DoDraw();
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
