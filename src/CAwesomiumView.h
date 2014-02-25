#pragma once

#include <Awesomium\WebView.h>

struct ID3D11ShaderResourceView;

namespace AwesomiumPlugin
{

    class CAwesomiumView
    {
        public:
            CAwesomiumView( int width, int height );
            ~CAwesomiumView( void );

            void SetTexture( void* pTexture, int cryTexID );
            void* GetTexture() const;
            void ReleaseTexture();

            void Resize( int width, int height );

            Awesomium::WebView* GetView();

            void DoDraw();

            void DrawFrameDX11SharedMemory( int width, int height );

            const char* GetEngineObjectName() const;
            const char* GetOverriddenMaterialName() const;

            void SetEngineObjectAndMaterialNames( const char* engineObjectName, const char* materialToOverrideName );

        private:
            void* m_pTexture; // IDirect3D9Texture* or ID3D11Texture2D*
            Awesomium::WebView* m_pView;
            int m_CryTextureID;

            int width;
            int height;

            string m_EngineObjectName;
            string m_MaterialToOverrideName;

            ID3D11ShaderResourceView* m_pTextureSRV;

            void CreateShaderResourceViewForDX11Texture();
    };

}