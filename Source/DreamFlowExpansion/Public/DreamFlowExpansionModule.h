#pragma once

#include "Modules/ModuleManager.h"

class DREAMFLOWEXPANSION_API FDreamFlowExpansionModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
