#pragma once
#ifndef PAK_TOOLS_H
#define PAK_TOOLS_H
#include "Engine.h"
#include "Runtime/PakFile/Public/IPlatformFilePak.h"
#include "Runtime/Engine/Public/ImageUtils.h"
#ifndef MYPRINTLOG
#define MYPRINTLOG
namespace MyTools {
	static void printLog(FString name) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *name);
	}
}
#endif
namespace MyTools {
	static FString GetLevelThumbnailAssetExtension() {
		return  TEXT("_Thumbnail");
	}
	static FString GetEnginePakKeyWorld() {
		return  TEXT("_Engine");
	}
	static FString GetProjectFloderName() {
		return FApp::GetProjectName();
	}
	static FString GetProjectDir() {
		auto tem=FPaths::ProjectDir();
		auto result=FPaths::ConvertRelativePathToFull(tem);
		return result;
	}
	static FString GetEngineDir() {
		auto tem = FPaths::EngineDir();
		auto result = FPaths::ConvertRelativePathToFull(tem);
		return result;
	}
	static FString GetProjectSavedDir() {
		auto tem=FPaths::ProjectSavedDir();
		auto result = FPaths::ConvertRelativePathToFull(tem);
		return result;
	}
	static FString GetProjectContentDir() {
		auto tem = FPaths::ProjectContentDir();
		auto result = FPaths::ConvertRelativePathToFull(tem);
		return result;
	}
	static FString GetProjectPluginsDir() {
		auto tem = FPaths::ProjectPluginsDir();
		auto result = FPaths::ConvertRelativePathToFull(tem);
		return result;
	}

	static FString GetRootDir() {
		auto tem = FPaths::RootDir();
		auto result = FPaths::ConvertRelativePathToFull(tem);
		return result;
	}
}
#ifdef MY_EDITOR_API
	#if WITH_EDITORONLY_DATA
#include "Editor/UnrealEd/Public/SourceCodeNavigation.h"
	namespace MyTools {
		namespace Editor {
			/*临时保存的列表文件路径*/
			static FString GetPakListTemPath() {
				return GetProjectDir() + TEXT("MyPakList.txt");
			}
			/*将PackagePath转换为绝对路径*/
			FString GetAssetFullPath(FString PackageName) {
				FString out;
				static const TCHAR* ScriptString = TEXT("/Script/");
				auto basePath = FPaths::ProjectSavedDir();
				if (PackageName.StartsWith(ScriptString))
				{
					// Handle C++ classes specially, as FPackageName::LongPackageNameToFilename won't return the correct path in this case
					const FString ModuleName = PackageName.RightChop(FCString::Strlen(ScriptString));
					FString ModulePath;
					if (FSourceCodeNavigation::FindModulePath(ModuleName, ModulePath))
					{
						//MaterialFunction'/Engine/Functions/Engine_MaterialFunctions03/Procedurals/NormalFromHeightmap.NormalFromHeightmap'
						FString RelativePath;
						FString baseName = FPaths::GetBaseFilename(PackageName);
						auto tem = StaticLoadObject(UObject::StaticClass(), NULL, *FString::Printf(TEXT("%s.%s"), *PackageName, *baseName));
						FAssetData AssetData(tem);
						if (AssetData.GetTagValue("ModuleRelativePath", RelativePath))
						{
							out = FPaths::ConvertRelativePathToFull(ModulePath / (*RelativePath));
						}
					}
				}
				else {
					auto tem = FPaths::ConvertRelativePathToFull(
						FPackageName::LongPackageNameToFilename(PackageName,
							FPackageName::GetAssetPackageExtension()));
					if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*tem)) {
						tem = FPaths::ConvertRelativePathToFull(
							FPackageName::LongPackageNameToFilename(PackageName,
								FPackageName::GetMapPackageExtension()));
					}
					out = tem;
				}
				return out;
			}
			/*将项目资源路径转换为pak加载路径*/
			FString ConvertContentPath(FString path, FString targetProjectName = TEXT("")) {
				static FString projetName = GetProjectFloderName();
				FString repleaceName = TEXT("../../../") + (targetProjectName.IsEmpty() ? projetName : targetProjectName) + TEXT("/");
				FString tem;
				if (path.Find(TEXT("/Plugins/"))>=0) {
					/*插件内容路径转换*/
					FString left, right;
					path.Split(TEXT("/Plugins/"), &left, &right);
					tem = repleaceName + TEXT("Plugins/") + right;
				}
				else {
					/*普通项目路径转换*/
					FString left, right;
					path.Split(TEXT("/Content/"), &left, &right);
					tem = repleaceName + TEXT("Content/") + right;
				}
				printLog(path + TEXT(" to ") + tem);
				return tem;
			}
			/*将引擎资源路径转换为pak加载路径*/
			FString ConvertEngineContentPath(FString path) {
				FString out, left, right;
				FString engineDir = GetEngineDir();
				path.Split(engineDir, &left, &right);
				out = TEXT("../../../Engine/") + right;
				printLog(path + TEXT(" to ") + out);
				return out;
			}
			/*将资源的路径转换为pak加载路径*/
			FString CreateAssetListByFullPaths(TArray<FString> paths, bool isCook,FString targetProjectName=TEXT("")) {
				FString outString = "";
				const auto ch = TEXT("\"");
				const auto end = TEXT(" \r\n");
				static FString projetName = GetProjectFloderName();
				FString projectDir = GetProjectDir();
				FString engineDir = GetEngineDir();
				printLog(TEXT("项目路径：")+ projectDir);
				printLog(TEXT("引擎路径：") + engineDir);
				if (isCook)
				{
					auto repleaceGameCookContent = projectDir + TEXT("Saved/Cooked/WindowsNoEditor/") + projetName + TEXT("/");
					auto repleaceEngineCookContent = projectDir + TEXT("Saved/Cooked/WindowsNoEditor/");

					for (auto tem : paths) {
						FString extension = FPaths::GetExtension(tem, true);
						if (tem.Find(engineDir) >= 0) {
							/*引擎资源*/
							auto temPath = tem.Replace(*GetRootDir(), *repleaceEngineCookContent);
							outString += ch + temPath.Replace(TEXT("/"), TEXT("\\")) + ch + TEXT(" ");
							outString += ch + ConvertEngineContentPath(tem) + ch + end;

							auto addFilePath = temPath.Replace(*extension, TEXT(".uexp"));
							auto addTemPath = tem.Replace(*extension, TEXT(".uexp"));
							if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*addFilePath)) {
								outString += ch + addFilePath.Replace(TEXT("/"), TEXT("\\")) + ch + TEXT(" ");
								outString += ch + ConvertEngineContentPath(addTemPath) + ch + end;
							}
							addFilePath = temPath.Replace(*extension, TEXT(".ubulk"));
							addTemPath = tem.Replace(*extension, TEXT(".ubulk"));
							if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*addFilePath)) {
								outString += ch + addFilePath.Replace(TEXT("/"), TEXT("\\")) + ch + TEXT(" ");
								outString += ch + ConvertEngineContentPath(addTemPath) + ch + end;
							}
						}
						else {
							/*项目资源*/
							auto temPath = tem.Replace(*projectDir, *repleaceGameCookContent);
							outString += ch + temPath.Replace(TEXT("/"), TEXT("\\")) + ch + TEXT(" ");
							outString += ch + ConvertContentPath(tem, targetProjectName) + ch + end;

							auto addFilePath = temPath.Replace(*extension, TEXT(".uexp"));
							auto addTemPath = tem.Replace(*extension, TEXT(".uexp"));
							if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*addFilePath)) {
								outString += ch + addFilePath.Replace(TEXT("/"), TEXT("\\")) + ch + TEXT(" ");
								outString += ch + ConvertContentPath(addTemPath, targetProjectName) + ch + end;
							}
							addFilePath = temPath.Replace(*extension, TEXT(".ubulk"));
							addTemPath = tem.Replace(*extension, TEXT(".ubulk"));
							if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*addFilePath)) {
								outString += ch + addFilePath.Replace(TEXT("/"), TEXT("\\")) + ch + TEXT(" ");
								outString += ch + ConvertContentPath(addTemPath, targetProjectName) + ch + end;
							}
						}

					}
				}
				else
					for (auto tem : paths) {
						if (tem.Find(engineDir) >= 0) {
							/*引擎资源*/
							//outString += ch + tem + ch + TEXT(" ");
							//outString += ch + ConvertEngineContentPath(tem) + ch + end;
						}
						else {
							/*项目资源*/
							outString += ch + tem + ch + TEXT(" ");
							outString += ch + ConvertContentPath(tem, targetProjectName) + ch + end;
						}
					}
				return outString;
			}
			/*将content目录中的资源对象生成pak打包路径*/
			static FString CreateAssetList(TSet<FName> assetList, bool isCook, FString targetProjectName = TEXT("")) {
				auto AssetList = assetList.Array();
				TArray<FString> pakLists;
				for (auto tem : AssetList) {
					auto name = tem.ToString();
					pakLists.Add(GetAssetFullPath(name));
				}
				return CreateAssetListByFullPaths(pakLists, isCook, targetProjectName);
			}

			/*打包资源*/
			static void PakAssetData(FString pakPath,FString pakListPath) {
				//auto pakListPath = CreatePakAssetListFile(pakLists, isCook);
				auto exePath = GetRootDir() + TEXT("Engine/Binaries/Win64/UnrealPak.exe");
				auto parm = pakPath + TEXT(" -create=") + pakListPath+TEXT(" -UTF8Output -encryptionini -enginedir=")+ GetEngineDir();
				parm += TEXT(" -platform=Windows");
				auto cookListPath = FString::Printf(TEXT("%sBuild/WindowsNoEditor/FileOpenOrder/CookerOpenOrder.log"), *GetProjectDir());
				parm += TEXT(" -installed -order=") + cookListPath + TEXT(" -multiprocess -patchpaddingalign=2048");
				printLog(TEXT("执行打包指令：") + exePath+TEXT(" ")+ parm);
				auto handle = FPlatformProcess::CreateProc(*exePath, *parm, false, false, false, nullptr, 0, nullptr, nullptr);
				FPlatformProcess::WaitForProc(handle);
			}
			/*Cook*/
			static void CookProject(bool isWait,UWorld* world=NULL) {
				auto exePath = GetRootDir() + TEXT("Engine/Binaries/Win64/UE4Editor-Cmd.exe");
				FString ProjectName = FApp::GetProjectName();
				auto parm = GetProjectDir() + ProjectName + TEXT(".uproject -run=Cook -NoLogTimes -TargetPlatform=WindowsNoEditor -fileopenlog -unversioned -stdout -CrashForUAT -unattended -UTF8OuTput -iterate");
				if (world) {
					auto Levelname = FPaths::GetCleanFilename(world->GetOutermost()->GetName());
					parm += TEXT(" -Map=") + Levelname;
				}
				printLog(TEXT("执行cook指令：") + exePath + TEXT(" ") + parm);
				auto handle=FPlatformProcess::CreateProc(*exePath, *parm,false, false, false, nullptr, 0, nullptr, nullptr);
				if (isWait)
					FPlatformProcess::WaitForProc(handle);
			}
		}
	}
	#endif
#endif

namespace MyTools {
		/*获取IPlatformFile*/
		/*获取FPakPlatformFile*/
		static FPakPlatformFile* GetPakPlatformFileInstance() {
			static bool isInstance = false;
			static FPakPlatformFile* PakPlatformFile = new FPakPlatformFile();
			if (!isInstance) {
				IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
				PakPlatformFile->Initialize(&PlatformFile, TEXT(""));
				FPlatformFileManager::Get().SetPlatformFile(*PakPlatformFile);
				isInstance = true;
			}
			return PakPlatformFile;
		}
		/*获取Pak包内所有文件信息*/
		static TArray<FString> GetAllFilesByPak(FPakFile* pakFile) {
			TArray<FString> FileList;
			if (pakFile) {
				auto inPath = pakFile->GetMountPoint();
				printLog(TEXT("inPath:") + inPath);
				pakFile->FindFilesAtPath(FileList, *inPath, true, false, true);
			}

			return FileList;
		}
		/*获取Pak包内所有文件信息*/
		static TArray<FString> GetAllFilesByPak(FString pakPath) {
			FPakPlatformFile* PakPlatformFile = GetPakPlatformFileInstance();
			FPakFile PakFile(&*PakPlatformFile, *pakPath, false);
			if (!PakFile.IsValid())return TArray<FString>();
			return GetAllFilesByPak(&PakFile);
		}
		/*根据路径加载Pak文件并挂载到项目路径
		*/
		static FPakFile* LoadPakToMount(FString pakPath, FPakPlatformFile* PakPlatformFile, FString MountPath = TEXT("")) {
			if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*pakPath))return NULL;
			MyTools::printLog(TEXT("LoadPakToMount:") + pakPath);
			MyTools::printLog(TEXT("MountPath:") + MountPath);
			auto PlatformFile = PakPlatformFile->GetLowerLevel();
			auto PakFile = new FPakFile(PlatformFile, *pakPath, false);
			auto index = PakFile->GetIndex();
			auto lastMounPoint = PakFile->GetMountPoint();
			MyTools::printLog(TEXT("lastMounPoint:") + lastMounPoint);
#if WITH_EDITORONLY_DATA
			FString left, right;
			lastMounPoint.Split(TEXT("../../../"), &left, &right);
			right.Split(TEXT("/"), &left, &right);
			auto SetMountPath = MountPath + right;
			MyTools::printLog(TEXT("SetMountPath:") + SetMountPath);
			PakPlatformFile->Mount(*pakPath, 0, *SetMountPath);
#else
			PakPlatformFile->Mount(*pakPath, 0);
#endif
			return PakFile;
		}
		static FPakFile* LoadPakToMount(FString pakPath, FString MountPath = TEXT("")) {
			auto PakPlatformFile = GetPakPlatformFileInstance();
			return LoadPakToMount(pakPath, PakPlatformFile, MountPath);
		}
		/*取消挂载到路径*/
		static void UnMountPak(FPakPlatformFile* PakPlatformFile, FString pakPath) {
			auto result = PakPlatformFile->Unmount(*pakPath);
			printLog(TEXT("Unmount:") + pakPath + (result ? TEXT(" success") : TEXT(" fail")));
		}
		static void UnMountPak(FString pakPath) {
			auto PakPlatformFile = GetPakPlatformFileInstance();
			UnMountPak(PakPlatformFile, pakPath);
		}
		/*PakFiles路径转到加载Object路径*/
		static FString PakFilePathToLoadPath(FString pakFilePath) {
			auto baseName = FPaths::GetBaseFilename(pakFilePath);
			FString left, right;
			pakFilePath.Split(TEXT("/Content/"), &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			right.Split(TEXT("."), &left, &right);
			if (!left.IsEmpty())
				return TEXT("/Game/") + left + TEXT(".") + baseName;
			else return TEXT("/Game/") + baseName + TEXT(".") + baseName;
		}
		/*PakFiles路径转到加载Package路径*/
		static FString PakFilePathToLoadPackagePath(FString pakFilePath) {
			auto baseName = FPaths::GetBaseFilename(pakFilePath);
			FString left, right;
			pakFilePath.Split(TEXT("/Content/"), &left, &right, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			right.Split(TEXT("."), &left, &right);
			return TEXT("/Game/") + left;
		}
		/*根据加载路径加载Pak中的资源*/
		//template<T>
		static UObject* LoadObjectFromPakLoadPath(FString loadPath) {
			FStreamableManager StreamableManager;
			return StreamableManager.SynchronousLoad(loadPath);
		}
	}
#endif // !PAK_TOOLS_H
