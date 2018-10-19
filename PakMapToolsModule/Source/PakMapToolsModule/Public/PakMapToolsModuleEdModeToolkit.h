// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

#define MY_EDITOR_API
class FPakMapToolsModuleEdModeToolkit : public FModeToolkit
{
public:

	FPakMapToolsModuleEdModeToolkit();
	
	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return ToolkitWidget; }

	/*打开保存目录窗口*/
	FString OpenSaveDirDialog(FString title, FString defaultPath = FString());
	/*获取所有选择的资源对象*/
	TArray<FAssetData> GetAllSelectedSource();
	/*获取资源的所有引用*/
	void GetDependeciesByPackageName(const FName& PackageName, TSet<FName>& AllDependencies);
	/*打包Map*/
	void PakWorld(TArray<UWorld*> worlds);
private:
	FString useProjectName = TEXT("YOUPROJECTNAME");
	TSharedPtr<SWidget> ToolkitWidget;
};
