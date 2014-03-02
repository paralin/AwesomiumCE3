/* Awesomium_Plugin - for licensing and copyright see license.txt */

#include <IPluginBase.h>
#include "CAwesomiumSystem.h"
#pragma once

/**
* @brief Awesomium Plugin Namespace
*/
namespace AwesomiumPlugin
{
    /**
    * @brief plugin Awesomium concrete interface
    */
    struct IPluginAwesomium
    {
        /**
        * @brief Get Plugin base interface
        */
        virtual PluginManager::IPluginBase* GetBase() = 0;

        virtual void InitAwesomium() = 0;
        virtual void ShutdownAwesomium() = 0;
		virtual CAwesomiumSystem * GetSystem() = 0;
    };
};