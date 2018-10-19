// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PakMapToolsModuleEdModeToolkit.h"
#include "PakMapToolsModuleEdMode.h"
#include "Engine/Selection.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorModeManager.h"


#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "Editor/ContentBrowser/Public/IContentBrowserSingleton.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"

#include "pakTools.h"

#define LOCTEXT_NAMESPACE "FPakMapToolsModuleEdModeToolkit"

FPakMapToolsModuleEdModeToolkit::FPakMapToolsModuleEdModeToolkit()
{
}

void FPakMapToolsModuleEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{


	SAssignNew(ToolkitWidget, SBorder)
		.HAlign(HAlign_Center)
		.Padding(25)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().HAlign(HAlign_Center).AutoHeight()
			[
				SNew(SEditableTextBox)
				.HintText(FText::FromString(useProjectName))
				.OnTextChanged_Lambda([&](const FText & InFilterText) {
					useProjectName = InFilterText.ToString();
				})
			]
			+ SVerticalBox::Slot().HAlign(HAlign_Center).AutoHeight()
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("对选择的关卡进行打包")))
				.OnClicked_Lambda([&]() {
					auto assets = GetAllSelectedSource();
					if (assets.Num() == 0)return FReply::Handled();

					TArray<UWorld*> temArray;
					for (auto asset : assets) {
						if (auto world = Cast<UWorld>(asset.GetAsset()))
							temArray.Add(world);
					}
					PakWorld(temArray);
				return FReply::Handled();
				})
			]
		];
		
	FModeToolkit::Init(InitToolkitHost);
}

FName FPakMapToolsModuleEdModeToolkit::GetToolkitFName() const
{
	return FName("PakMapToolsModuleEdMode");
}

FText FPakMapToolsModuleEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("PakMapToolsModuleEdModeToolkit", "DisplayName", "PakMapToolsModuleEdMode Tool");
}

class FEdMode* FPakMapToolsModuleEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FPakMapToolsModuleEdMode::EM_PakMapToolsModuleEdModeId);
}

FString FPakMapToolsModuleEdModeToolkit::OpenSaveDirDialog(FString title, FString defaultPath)
{
	auto DesktopPlatform = FDesktopPlatformModule::Get();
	auto ArchiveDirectory = FPaths::GetProjectFilePath();
	if (DesktopPlatform)
	{
		if (!DesktopPlatform->OpenDirectoryDialog(nullptr, title, defaultPath, ArchiveDirectory))
			ArchiveDirectory = FString();
	}
	return ArchiveDirectory;
}

TArray<FAssetData> FPakMapToolsModuleEdModeToolkit::GetAllSelectedSource()
{
	TArray<FAssetData> SelectedAssets;
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
	return SelectedAssets;
}

void FPakMapToolsModuleEdModeToolkit::GetDependeciesByPackageName(const FName & PackageName, TSet<FName>& AllDependencies)
{
	static FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	TArray<FName> Dependencies;
	AssetRegistryModule.Get().GetDependencies(PackageName, Dependencies);
	for (auto DependsIt = Dependencies.CreateConstIterator(); DependsIt; ++DependsIt)
	{
		auto packageName = *DependsIt;

		if (!AllDependencies.Contains(packageName))
		{
			AllDependencies.Add(packageName);
			GetDependeciesByPackageName(packageName, AllDependencies);
		}
	}
}

void FPakMapToolsModuleEdModeToolkit::PakWorld(TArray<UWorld*> worlds)
{
	if (worlds.Num() == 0)return;
	auto path = OpenSaveDirDialog(TEXT("保存目录"));
	if (path.IsEmpty())return;
	TMap<FString, FString> listArray;
	/*Cook*/
	MyTools::Editor::CookProject(true);
	/*找出所有要打包的资源*/
	for (auto tem : worlds) {
		TSet<FName> temAssetList;
		GetDependeciesByPackageName(*tem->GetOutermost()->GetName(), temAssetList);
		temAssetList.Add(*tem->GetOutermost()->GetName());
		auto temStr=MyTools::Editor::CreateAssetList(temAssetList, true, useProjectName);
		listArray.Add(FPaths::GetBaseFilename(tem->GetOutermost()->GetName()), temStr);
	}
	/*开始打包*/
	for (auto tem : listArray) {
		auto out = tem.Value;
		FFileHelper::SaveStringToFile(out, *MyTools::Editor::GetPakListTemPath(), FFileHelper::EEncodingOptions::ForceUTF8);
		MyTools::Editor::PakAssetData(path + TEXT("/") + tem.Key + TEXT(".pak"), MyTools::Editor::GetPakListTemPath());
	}
}

#undef LOCTEXT_NAMESPACE
