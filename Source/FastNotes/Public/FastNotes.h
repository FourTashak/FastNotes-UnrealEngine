// Copyright (c) 2025, Sefa Mankaloglu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FFastNotesModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets);
	void AddNotesMenuEntry(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets);
	void OnOpenNoteAction(TArray<FAssetData> SelectedAssets);
	void OnRemoveNoteAction(TArray<FAssetData> SelectedAssets);
	void OnAssetRenamed(const FAssetData& AssetData, const FString& OldObjectPath);
};
