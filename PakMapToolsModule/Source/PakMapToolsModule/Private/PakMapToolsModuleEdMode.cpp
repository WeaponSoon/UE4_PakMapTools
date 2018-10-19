// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PakMapToolsModuleEdMode.h"
#include "PakMapToolsModuleEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"

const FEditorModeID FPakMapToolsModuleEdMode::EM_PakMapToolsModuleEdModeId = TEXT("EM_PakMapToolsModuleEdMode");

FPakMapToolsModuleEdMode::FPakMapToolsModuleEdMode()
{

}

FPakMapToolsModuleEdMode::~FPakMapToolsModuleEdMode()
{

}

void FPakMapToolsModuleEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FPakMapToolsModuleEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FPakMapToolsModuleEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

bool FPakMapToolsModuleEdMode::UsesToolkits() const
{
	return true;
}




