#include "StdAfx.h"
#include "UIElement.h"
#include <string>

#include <IGame.h>
#include <IGameFramework.h>
#include <IActorSystem.h>

using namespace std;
using namespace Awesomium;

JSObject CUIElement::s_javascriptObject = JSObject();
int __count = 0;

CUIElement::CUIElement(const char* pathToHtml)
{
	char* path = "asset://";
	strcat(path, pathToHtml);

	m_pWebView = WebCore::instance()->CreateWebView(m_width, m_height);
	
	s_javascriptObject = m_pWebView->CreateGlobalJavascriptObject(WSLit("awesomiumce3")).ToObject();

	SetObjectProperty("version", "1.0.0.0");
	SetObjectProperty("author", "erik");

	m_pWebView->LoadURL(WebURL(WSLit(path)));
	m_pWebView->SetTransparent(true);

	__count++;
}

CUIElement::~CUIElement()
{
}

void CUIElement::OnUpdate()
{
	BitmapSurface* surface = static_cast<BitmapSurface*>(m_pWebView->surface());
	if(surface->is_dirty())
	{
		m_pRenderBuffer = surface->buffer();

		if(m_textureId == InvalidTexture)
		{
			m_textureId = CreateTexture();
			auto entity = gEnv->pEntitySystem->FindEntityByName("cabin_table1");

			auto material = entity->GetMaterial();
		} else
		{
			UpdateTexture();
		}
	}
}

void CUIElement::SetObjectProperty(const char* propertyName, const char* propertyValue)
{
	s_javascriptObject.SetProperty(WSLit(propertyName), JSValue(WSLit(propertyValue)));
}

void CUIElement::UpdateTexture()
{
	gEnv->pRenderer->UpdateTextureInVideoMemory(m_textureId, const_cast<unsigned char*>(m_pRenderBuffer), 0, 0, m_width, m_height, eTF_A8R8G8B8);
}

int CUIElement::CreateTexture()
{
	auto material = gEnv->p3DEngine->GetMaterialManager()->FindMaterial("cemono/Game/Levels/Forest/TestMaterial");
	if(material)
	{
		gEnv->pEntitySystem->FindEntityByName("screen1")->SetMaterial(material);
		return material->GetShaderItem().m_pShaderResources->GetTexture(EFTT_DIFFUSE)->m_Sampler.m_pITex->GetTextureID();
	}
	
	return -1;
}

void CUIElement::SetVisible(bool visible)
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
