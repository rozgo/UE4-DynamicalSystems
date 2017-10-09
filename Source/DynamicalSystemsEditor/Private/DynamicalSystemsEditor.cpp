// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "DynamicalSystemsEditor.h"
#include "Core.h"
#include "ModuleManager.h"
#include "IPluginManager.h"

#define LOCTEXT_NAMESPACE "FDynamicalSystemsModuleEditor"



void FDynamicalSystemsModuleEditor::StartupModule()
{

	
}

void FDynamicalSystemsModuleEditor::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDynamicalSystemsModuleEditor, DynamicalSystemsEditor)
