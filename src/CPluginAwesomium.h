/* Awesomium_Plugin - for licensing and copyright see license.txt */

#pragma once

#include <Game.h>

#include <IPluginManager.h>
#include <IPluginBase.h>
#include <CPluginBase.hpp>

#include <IPluginAwesomium.h>
#include <UIElement.h>

#include <IGameFramework.h>
#include <Awesomium\WebCore.h>
#include <Awesomium\DataPak.h>
#include <Awesomium\STLHelpers.h>

#define PLUGIN_NAME "Awesomium"
#define PLUGIN_CONSOLE_PREFIX "[" PLUGIN_NAME " " PLUGIN_TEXT "] " //!< Prefix for Logentries by this plugin

using namespace Awesomium;

namespace AwesomiumPlugin
{
    /**
    * @brief Provides information and manages the resources of this plugin.
    */
    class CPluginAwesomium :
        public PluginManager::CPluginBase,
        public IPluginAwesomium,
        public IGameFrameworkListener
    {
        public:
            CPluginAwesomium();
            ~CPluginAwesomium();

            // IPluginBase
            bool Release( bool bForce = false );

            int GetInitializationMode() const
            {
                return int( PluginManager::IM_Default );
            };

            bool Init( SSystemGlobalEnvironment& env, SSystemInitParams& startupParams, IPluginBase* pPluginManager, const char* sPluginDirectory );

            const char* GetVersion() const
            {
                return "1.0.0.0";
            };

            const char* GetName() const
            {
                return PLUGIN_NAME;
            };

            const char* GetCategory() const
            {
                return "Visual";
            };

            const char* ListAuthors() const
            {
                return "Christian Stewart";
            };

            const char* ListCVars() const;

            const char* GetStatus() const;

            const char* GetCurrentConcreteInterfaceVersion() const
            {
                return "1.0";
            };

            void* GetConcreteInterface( const char* sInterfaceVersion )
            {
                return static_cast < IPluginAwesomium* > ( this );
            };

            // IPluginAwesomium
            IPluginBase* GetBase()
            {
                return static_cast<IPluginBase*>( this );
            };

            virtual bool InitAwesomium() override;
            virtual void Shutdown() override;
            virtual void SetVisible( bool visible ) override;
            virtual bool IsVisible() const override;

            // IGameFramework
            virtual void OnPostUpdate( float fDeltaTime ) override;
            virtual void OnSaveGame( ISaveGame* pSaveGame ) override;
            virtual void OnLoadGame( ILoadGame* pLoadGame ) override;
            virtual void OnLevelEnd( const char* nextLevel ) override;
            virtual void OnActionEvent( const SActionEvent& event ) override;
            virtual void OnPreRender() override;

            bool m_bEnablePlugins;
            bool m_bVisible;
            std::vector<std::shared_ptr<CUIElement>> m_uiElements;
            WebCore* m_pWebCore;
            DataSource* m_DataSource;

            virtual int LoadElement( const char* pathToHtml );

    };

    extern CPluginAwesomium* gPlugin;
}

/**
* @brief This function is required to use the Autoregister Flownode without modification.
* Include the file "CPluginAwesomium.h" in front of flownode.
*/
inline void GameWarning( const char* sFormat, ... ) PRINTF_PARAMS( 1, 2 );
inline void GameWarning( const char* sFormat, ... )
{
    va_list ArgList;
    va_start( ArgList, sFormat );
    AwesomiumPlugin::gPlugin->LogV( ILog::eWarningAlways, sFormat, ArgList );
    va_end( ArgList );
};
