#pragma once

#include <Awesomium\WebCore.h>
#include <Awesomium\WebView.h>
#include <Awesomium\BitmapSurface.h>
#include <Awesomium\STLHelpers.h>
#include <IPluginManager.h>
#include <IPluginD3D.h>

#include <d3d11.h>

namespace AwesomiumPlugin
{

    class HTMLElement
        //: private D3DPlugin::ID3DEventListener
    {
        public:
            HTMLElement::HTMLElement( const char* pathToHtml, D3DPlugin::IPluginD3D* d3dPlugin );
            ~HTMLElement();

            void OnUpdate();
            void SetVisible( bool visible );
            bool IsVisible() const;
            void SetObjectProperty( const char* propertyName, const char* propertyValue );

        protected:

            const char* m_pathToHtml;
            bool m_bVisible;

            int m_width;
            int m_height;

            Awesomium::WebView* m_pWebView;

            ID3D11Device* pDevice;
            D3DPlugin::IPluginD3D* d3d;

            ID3D11Texture2D* renderTargetTextureMap;
            ID3D11RenderTargetView* renderTargetViewMap;
            ID3D11ShaderResourceView* shaderResourceViewMap;

            static Awesomium::JSObject s_javascriptObject;

    };

}
