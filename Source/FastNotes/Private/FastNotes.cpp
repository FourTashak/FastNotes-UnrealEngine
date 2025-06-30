// Copyright (c) 2025, Sefa Mankaloglu. All Rights Reserved.

#include "FastNotes.h"
#include "ContentBrowserModule.h"
#include "HAL/PlatformFilemanager.h"
#include "HAL/PlatformProcess.h"
#include "AssetRegistry/AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "FFastNotesModule"

void FFastNotesModule::StartupModule()
{
	//
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FContentBrowserMenuExtender_SelectedAssets>& Extenders = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();

	Extenders.Add(FContentBrowserMenuExtender_SelectedAssets::CreateRaw(this, &FFastNotesModule::OnExtendContentBrowserAssetSelectionMenu));

	//
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistryModule.Get().OnAssetRenamed().AddRaw(this, &FFastNotesModule::OnAssetRenamed);
}

void FFastNotesModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

TSharedRef<FExtender> FFastNotesModule::OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets)
{
	TSharedRef<FExtender> Extender = MakeShared<FExtender>();

	Extender->AddMenuExtension(
		"GetAssetActions",
		EExtensionHook::After,
		nullptr,
		FMenuExtensionDelegate::CreateRaw(this, &FFastNotesModule::AddNotesMenuEntry, SelectedAssets)
	);

	return Extender;
}

void FFastNotesModule::AddNotesMenuEntry(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets)
{
	if (SelectedAssets.Num() > 1) return;

	MenuBuilder.BeginSection("Note Options", LOCTEXT("MyCustomCategoryHeading", "Notes"));
	
	MenuBuilder.AddMenuEntry(
		LOCTEXT("OpenNotes", "Open Notes"),
		LOCTEXT("OpenNotesContextTooltip", "The Notes file will open in default text editor"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FFastNotesModule::OnOpenNoteAction, SelectedAssets))
	);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("DeleteNotes", "Delete Notes"),
		LOCTEXT("DeleteNotesContextTooltip", "The Notes file will be deleted"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FFastNotesModule::OnRemoveNoteAction, SelectedAssets))
	);

	MenuBuilder.EndSection();
}

void FFastNotesModule::OnOpenNoteAction(TArray<FAssetData> SelectedAssets)
{
	//No selected assets or more than one
	if (SelectedAssets.Num() != 1) return;

	FString FileName = SelectedAssets[0].AssetName.ToString() + FString("_Notes") + TEXT(".txt");
	FString Directory = FPaths::Combine(FPaths::ProjectDir(), TEXT("AssetNotes"));
	FString NewFilePath = FPaths::Combine(Directory, FileName);

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	FString AbsolutePathh = FPaths::ConvertRelativePathToFull(*NewFilePath);

	if (!PlatformFile.FileExists(*AbsolutePathh))
	{
		if (FFileHelper::SaveStringToFile(FString(), *AbsolutePathh, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), 8))
		{
			UE_LOG(LogTemp, Log, TEXT("File created at: %s"), *AbsolutePathh);

			FPlatformProcess::LaunchFileInDefaultExternalApplication(*AbsolutePathh);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Failed to create file at: %s"), *AbsolutePathh);
	}
	else
	{
		//Open file
		FPlatformProcess::LaunchFileInDefaultExternalApplication(*AbsolutePathh);
	}
}

void FFastNotesModule::OnRemoveNoteAction(TArray<FAssetData> SelectedAssets)
{
	//No selected assets or more than one
	if (SelectedAssets.Num() != 1) return;

	FString FileName = SelectedAssets[0].AssetName.ToString() + FString("_Notes") + TEXT(".txt");
	FString Directory = FPaths::Combine(FPaths::ProjectDir(), TEXT("AssetNotes"));
	FString NewFilePath = FPaths::Combine(Directory, FileName);

	FString AbsolutePathh = FPaths::ConvertRelativePathToFull(*NewFilePath);

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (PlatformFile.FileExists(*AbsolutePathh))
	{
		bool bSuccess = IFileManager::Get().Delete(*NewFilePath, true, true);

		if(bSuccess)
		{
			UE_LOG(LogTemp, Error, TEXT("Successfully deleted file at: %s"), *AbsolutePathh);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("Failed to delete file at: %s"), *AbsolutePathh);
	}
}

void FFastNotesModule::OnAssetRenamed(const FAssetData& AssetData, const FString& OldObjectPath)
{
	FString OldName = FPackageName::GetShortName(OldObjectPath); 
	FString OldFileName = FPaths::GetBaseFilename(OldName) + FString("_Notes") + TEXT(".txt");
	FString Directory = FPaths::Combine(FPaths::ProjectDir(), TEXT("AssetNotes"));
	FString OldFilePath = FPaths::Combine(Directory, OldFileName);
	
	FString NewName = FPackageName::GetShortName(AssetData.AssetName.ToString());
	FString NewFileName = FPaths::GetBaseFilename(NewName) + FString("_Notes") + TEXT(".txt");
	FString NewFilePath = FPaths::Combine(Directory,NewFileName);

	//Because OnAssetRenamed gets called when either the asset is renamed or moved we have to check
	//if its moved then the name is the same and we dont have to call the functions below.
	if (OldFileName == NewFileName) return;

	FString AbsolutePathh = FPaths::ConvertRelativePathToFull(*OldFilePath);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (PlatformFile.FileExists(*AbsolutePathh))
	{
		bool bSuccess = IFileManager::Get().Move(*NewFilePath, *OldFilePath, true, true);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFastNotesModule, FastNotes)