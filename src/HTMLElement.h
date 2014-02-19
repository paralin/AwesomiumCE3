#pragma once

#include <Awesomium\WebCore.h>
#include <Awesomium\WebView.h>
#include <Awesomium\BitmapSurface.h>
#include <Awesomium\STLHelpers.h>
#include <IPluginManager.h>

namespace AwesomiumPlugin
{

    class HTMLElement
        //: private D3DPlugin::ID3DEventListener
    {
        public:
            HTMLElement::HTMLElement( const char* pathToHtml );
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

            static Awesomium::JSObject s_javascriptObject;

    };

}
