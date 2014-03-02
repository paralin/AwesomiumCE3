#pragma once
#include <IPluginD3D.h>
#include <IGameFramework.h>
#include <ILevelSystem.h>

#include <Awesomium\WebCore.h>
#include <Awesomium\DataPak.h>
#include <Awesomium\STLHelpers.h>

#include "CFullscreenTriangleDrawer.h"

#include "CAwesomiumView.h"

//Comment if you don't want a default view to be created at start
#define TEST_VIEW

using namespace Awesomium;
namespace AwesomiumPlugin
{
    class CAwesomiumSystem :
        public D3DPlugin::ID3DEventListener,
        public IGameFrameworkListener
    {
        public:
            CAwesomiumSystem( void );
            ~CAwesomiumSystem( void );

            // ID3DListener methods
            virtual void OnPrePresent() override;
            virtual void OnPostPresent()  override;
            virtual void OnPreReset() override;
            virtual void OnPostReset() override;
            virtual void OnPostBeginScene() override;

            // IGameFramework
            virtual void OnPostUpdate( float fDeltaTime ) override;
            virtual void OnSaveGame( ISaveGame* pSaveGame ) override;
            virtual void OnLoadGame( ILoadGame* pLoadGame ) override;
            virtual void OnLevelEnd( const char* nextLevel ) override;
            virtual void OnActionEvent( const SActionEvent& event ) override;
            virtual void OnPreRender() override;

			CAwesomiumView * CreateView(int widht, int height, const char* objName, const char* matName);

            void SetTexturesForListeners();
            void ChangeEntityDiffuseTextureForMaterial( CAwesomiumView* pViewListener, const char* entityName, const char* materialName );
            static bool g_WebCoreInit;
        private:
            CFullscreenTriangleDrawer* m_FullscreenDrawer;
            CAwesomiumView* m_hudView;
            std::vector<CAwesomiumView*> m_views;


            void UpdateHUD();
    };
}
