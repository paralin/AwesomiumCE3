#pragma once

#include <Awesomium\WebCore.h>
#include <Awesomium\WebView.h>
#include <Awesomium\BitmapSurface.h>
#include <Awesomium\STLHelpers.h>

class CUIElement
{
public:
	CUIElement(const char* pathToHtml);
	virtual ~CUIElement();

	virtual void OnUpdate();
	virtual void SetVisible(bool visible);
	virtual bool IsVisible() const;
	virtual void SetObjectProperty(const char* propertyName, const char* propertyValue);
	virtual int GetTextureId() const;

protected:
	virtual int CreateTexture();
	virtual void UpdateTexture();

	static const int InvalidTexture = -1;
	const char* m_pathToHtml;
	bool m_bVisible;
	int m_textureId;
	int m_width;
	int m_height;
	Awesomium::WebView* m_pWebView;
	const unsigned char* m_pRenderBuffer;

	static Awesomium::JSObject s_javascriptObject;

};

