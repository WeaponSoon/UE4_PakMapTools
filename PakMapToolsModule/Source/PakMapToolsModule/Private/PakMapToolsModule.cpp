// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PakMapToolsModule.h"
#include "PakMapToolsModuleEdMode.h"

#define LOCTEXT_NAMESPACE "FPakMapToolsModuleModule"

void FPakMapToolsModuleModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FPakMapToolsModuleEdMode>(FPakMapToolsModuleEdMode::EM_PakMapToolsModuleEdModeId, LOCTEXT("PakMapToolsModuleEdModeName", "PakMapToolsModuleEdMode"), FSlateIcon(), true);
}

void FPakMapToolsModuleModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FPakMapToolsModuleEdMode::EM_PakMapToolsModuleEdModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPakMapToolsModuleModule, PakMapToolsModule)