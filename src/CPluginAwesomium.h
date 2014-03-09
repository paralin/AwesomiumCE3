/* Awesomium_Plugin - for licensing and copyright see license.txt */

#pragma once

#include <Game.h>

#include <IPluginManager.h>
#include <IPluginBase.h>
#include <CPluginBase.hpp>

#include <IPluginAwesomium.h>

#include <IPluginD3D.h>

#include "CAwesomiumSystem.h"

#define PLUGIN_NAME "Awesomium"
#define PLUGIN_CONSOLE_PREFIX "[" PLUGIN_NAME " " PLUGIN_TEXT "] " //!< Prefix for Logentries by this plugin

namespace AwesomiumPlugin
{
    /**
    * @brief Provides information and manages the resources of this plugin.
    */
    class CPluginAwesomium :
        public PluginManager::CPluginBase,
        public IPluginAwesomium
    {
        public:
            CPluginAwesomium();
            ~CPluginAwesomium();


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

            virtual void InitAwesomium() override;
            virtual void ShutdownAwesomium() override;
            virtual CAwesomiumSystem* GetSystem() override;

            bool CPluginAwesomium::InitDependencies();
            bool CPluginAwesomium::Release( bool bForce );
            bool CPluginAwesomium::CheckDependencies() const;

            bool m_bEnablePlugins;

            CAwesomiumSystem* g_system;
    };
    extern D3DPlugin::IPluginD3D* gD3DSystem;
    extern void* gD3DDevice;
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
