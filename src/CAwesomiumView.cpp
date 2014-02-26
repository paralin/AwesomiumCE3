#include "StdAfx.h"
#include "CPluginAwesomium.h"
#include "CAwesomiumView.h"
#include "CAwesomiumSystem.h"
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <Awesomium\BitmapSurface.h>

namespace AwesomiumPlugin
{

    CAwesomiumView::CAwesomiumView( int width, int height )
        : m_pTextureSRV( NULL ),
          m_CryTextureID( 0 )
    {
        this->width = width;
        this->height = height;
        m_pView = WebCore::instance()->CreateWebView( width, height );
    }

    CAwesomiumView::~CAwesomiumView( void )
    {
        ReleaseTexture();

        if ( m_pView )
        {
            m_pView->Destroy();
            m_pView = NULL;
        }
    }

    void CAwesomiumView::ReleaseTexture()
    {
        SAFE_RELEASE( m_pTextureSRV );
    }

    void* CAwesomiumView::GetTexture() const
    {
        return m_pTextureSRV;
    }

    void CAwesomiumView::SetTexture( void* pTexture, int cryTexID )
    {
        ReleaseTexture();

        m_CryTextureID = cryTexID;
        m_pTexture = pTexture;

        CreateShaderResourceViewForDX11Texture();
    }

    void CAwesomiumView::CreateShaderResourceViewForDX11Texture()
    {
        ID3D11Texture2D* pTexture = static_cast<ID3D11Texture2D*>( m_pTexture );

        ID3D11Device* pDevice = static_cast<ID3D11Device*>( gD3DDevice );
        ID3D11DeviceContext* pContext = NULL;
        pDevice->GetImmediateContext( &pContext );

        D3D11_TEXTURE2D_DESC texDesc;
        pTexture->GetDesc( &texDesc );

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        pDevice->CreateShaderResourceView( pTexture, &srvDesc, &m_pTextureSRV );
    }

    Awesomium::WebView* CAwesomiumView::GetView()
    {
        return m_pView;
    }

    void CAwesomiumView::DoDraw()
    {
        if ( m_pTexture == NULL )
        {
            return;
        }

        DrawFrameDX11SharedMemory( width, height );
    }

    void CAwesomiumView::DrawFrameDX11SharedMemory( int width, int height )
    {
        ID3D11Texture2D* pTexture = static_cast<ID3D11Texture2D*>( m_pTexture );
        ID3D11Device* pDevice = static_cast<ID3D11Device*>( gD3DDevice );
        ID3D11DeviceContext* pContext = NULL;
        pDevice->GetImmediateContext( &pContext );

        D3D11_MAPPED_SUBRESOURCE mapped;
        HRESULT hr = pContext->Map( pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped );

        if ( FAILED( hr ) )
        {
            return;
        }

        const size_t size = width * height * 4;
        char* pSrc = reinterpret_cast<char*>( const_cast<unsigned char*>( ( ( Awesomium::BitmapSurface* )( m_pView->surface() ) )->buffer() ) );

        if ( pSrc == NULL )
        {
            pContext->Unmap( pTexture, 0 );
            return;
        }

        char* pDest = static_cast<char*>( mapped.pData );

        // copy and convert from BGRA to RGBA
        int offsetSrc = 0;
        int offsetDst = 0;
        int rowOffset = mapped.RowPitch % width;

        for ( int row = 0; row < height; ++row )
        {
            for ( int col = 0; col < width; ++col )
            {
                pDest[offsetDst] = pSrc[offsetSrc + 2];
                pDest[offsetDst + 1] = pSrc[offsetSrc + 1];
                pDest[offsetDst + 2] = pSrc[offsetSrc];
                pDest[offsetDst + 3] = pSrc[offsetSrc + 3];
                offsetSrc += 4;
                offsetDst += 4;
            }

            offsetDst += rowOffset;
        }

        pContext->Unmap( pTexture, 0 );
    }

    void CAwesomiumView::Resize( int width, int height )
    {
        this->width = width;
        this->height = height;
        m_pView->Resize( width, height );
    }

    const char* CAwesomiumView::GetEngineObjectName() const
    {
        return m_EngineObjectName.c_str();
    }

    const char* CAwesomiumView::GetOverriddenMaterialName() const
    {
        return m_MaterialToOverrideName.c_str();
    }

    void CAwesomiumView::SetEngineObjectAndMaterialNames( const char* engineObjectName, const char* materialToOverrideName )
    {
        m_EngineObjectName = engineObjectName;
        m_MaterialToOverrideName = materialToOverrideName;
    }
}
