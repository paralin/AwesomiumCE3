#include "StdAfx.h"
#include "HTMLElement.h"

#include <IGame.h>
#include <IGameFramework.h>
#include <IActorSystem.h>

#include <IPluginManager.h>
#include <CPluginAwesomium.h>
#include "ComPtr.h"

using namespace Awesomium;

namespace AwesomiumPlugin
{

    JSObject HTMLElement::s_javascriptObject = JSObject();
    int __count = 0;

    HTMLElement::HTMLElement( const char* pathToHtml )
    {
        m_width = 1024;
        m_height = 1024;

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

        m_pWebView->LoadURL( WebURL( WSLit( "http://www.youtube.com/watch?v=tjLSCVm3CEY" ) ) );
        //m_pWebView->LoadURL( WebURL( WSLit( "http://www.google.com" ) ) );
        //m_pWebView->LoadURL( WebURL( WSLit( path ) ) );
        Error teste = m_pWebView->last_error();
        m_pWebView->SetTransparent( false );

        __count++;
    }

    HTMLElement::~HTMLElement()
    {
    }

    struct VERTEX
    {
        float X, Y, Z;    // vertex position
    };

    void HTMLElement::OnUpdate()
    {
    }

    void HTMLElement::SetObjectProperty( const char* propertyName, const char* propertyValue )
    {
        s_javascriptObject.SetProperty( WSLit( propertyName ), JSValue( WSLit( propertyValue ) ) );
    }


    void HTMLElement::SetVisible( bool visible )
    {
        m_bVisible = visible;
    }

    bool HTMLElement::IsVisible() const
    {
        return m_bVisible;
    }

}