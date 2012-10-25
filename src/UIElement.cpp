#include "StdAfx.h"
#include "UIElement.h"

#include <IGame.h>
#include <IGameFramework.h>
#include <IActorSystem.h>

#include <IPluginManager.h>
#include <CPluginAwesomium.h>

using namespace Awesomium;

namespace AwesomiumPlugin
{

    JSObject CUIElement::s_javascriptObject = JSObject();
    int __count = 0;

    CUIElement::CUIElement( const char* pathToHtml )
    {
        m_width = 1024;
        m_height = 1024;
        m_textureId = InvalidTexture;

        string path = "asset://";
        path += pathToHtml;

        string pathu = gPluginManager->GetDirectoryUser();
        pathu += "\\Awesomium";

        WebPreferences* testw = new WebPreferences();
        WebSession* test = gPlugin->m_pWebCore->CreateWebSession( WSLit( pathu.c_str() ), *testw );
        test->AddDataSource( WSLit( "UI" ), gPlugin->m_DataSource );

        m_pWebView = gPlugin->m_pWebCore->CreateWebView( m_width, m_height, test );

        s_javascriptObject = m_pWebView->CreateGlobalJavascriptObject( WSLit( "awesomiumce3" ) ).ToObject();

        SetObjectProperty( "version", gPlugin->GetVersion() );
        SetObjectProperty( "author", gPlugin->ListAuthors() );

        m_pWebView->LoadURL( WebURL( WSLit( "http://www.google.de" ) ) );
        //m_pWebView->LoadURL( WebURL( WSLit( path ) ) );
        Error teste = m_pWebView->last_error();
        m_pWebView->SetTransparent( false );

        __count++;
    }

    CUIElement::~CUIElement()
    {
    }

    void CUIElement::OnUpdate()
    {
        BitmapSurface* surface = static_cast<BitmapSurface*>( m_pWebView->surface() );

        if ( surface && surface->is_dirty() )
        {
            m_pRenderBuffer = surface->buffer();

            if ( m_textureId == InvalidTexture )
            {
                m_textureId = CreateTexture();
                //auto entity = gEnv->pEntitySystem->FindEntityByName("cabin_table1");
                //auto material = entity->GetMaterial();
            }

            else
            {
                UpdateTexture();
            }
        }
    }

    void CUIElement::SetObjectProperty( const char* propertyName, const char* propertyValue )
    {
        s_javascriptObject.SetProperty( WSLit( propertyName ), JSValue( WSLit( propertyValue ) ) );
    }

    void CUIElement::UpdateTexture()
    {
        gEnv->pRenderer->UpdateTextureInVideoMemory( m_textureId, const_cast<unsigned char*>( m_pRenderBuffer ), 0, 0, m_width, m_height, eTF_A8R8G8B8 );
        gEnv->pRenderer->Push2dImage( 0, 0, 500, 500, m_textureId );
    }

    int CUIElement::CreateTexture()
    {
        return gEnv->pRenderer->SF_CreateTexture( m_width, m_height, 1, NULL, eTF_X8R8G8B8, FT_USAGE_DYNAMIC );
        /*
        auto material = gEnv->p3DEngine->GetMaterialManager()->FindMaterial("cemono/Game/Levels/Forest/TestMaterial");
        if(material)
        {
            gEnv->pEntitySystem->FindEntityByName("screen1")->SetMaterial(material);
            return material->GetShaderItem().m_pShaderResources->GetTexture(EFTT_DIFFUSE)->m_Sampler.m_pITex->GetTextureID();
        }
        */
        return -1;
    }

    void CUIElement::SetVisible( bool visible )
    {
        m_bVisible = visible;
    }

    bool CUIElement::IsVisible() const
    {
        return m_bVisible;
    }

    int CUIElement::GetTextureId() const
    {
        return m_textureId;
    }

}