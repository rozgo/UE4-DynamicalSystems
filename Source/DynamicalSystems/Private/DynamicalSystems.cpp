// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "DynamicalSystems.h"
#include "Core.h"
#include "ModuleManager.h"
#include "IPluginManager.h"
#include "RustyDynamics.h"

#include <string>

#define LOCTEXT_NAMESPACE "FDynamicalSystemsModule"

extern "C" void ffi_log(const char* log)
{
	UE_LOG(LogTemp, Warning, TEXT("[Rust] %s"), UTF8_TO_TCHAR(log));
}

void TestFFI()
{
	RigidBodyPack bodies[] = {
		{10, 1.0, 1.1, 1.2, 1.3, 2.0, 2.1, 2.2, 2.3},
		{11, 1.0, 1.1, 1.2, 1.3, 2.0, 2.1, 2.2, 2.3},
		{12, 1.0, 1.1, 1.2, 1.3, 2.0, 2.1, 2.2, 2.3},
	};
	AvatarPack parts[] = {
		{20, 1.0, 1.1, 1.2, 1.3, 2.0, 2.1, 2.2, 2.3},
		{21, 1.0, 1.1, 1.2, 1.3, 2.0, 2.1, 2.2, 2.3},
		{22, 1.0, 1.1, 1.2, 1.3, 2.0, 2.1, 2.2, 2.3},
		{23, 1.0, 1.1, 1.2, 1.3, 2.0, 2.1, 2.2, 2.3},
		{24, 1.0, 1.1, 1.2, 1.3, 2.0, 2.1, 2.2, 2.3},
	};

	UE_LOG(LogTemp, Warning, TEXT("sizeof<AvatarPack> %lu\n"), sizeof(AvatarPack));
	UE_LOG(LogTemp, Warning, TEXT("sizeof<RigidBodyPack> %lu\n"), sizeof(RigidBodyPack));
	UE_LOG(LogTemp, Warning, TEXT("sizeof<RustVec> %lu\n"), sizeof(RustVec));
	UE_LOG(LogTemp, Warning, TEXT("sizeof<WorldPack> %lu\n"), sizeof(WorldPack));

	RustVec bodies_vec;
	bodies_vec.vec_ptr = (size_t)&bodies[0];
	bodies_vec.vec_len = 3;
	bodies_vec.vec_cap = 3;

	RustVec parts_vec;
	parts_vec.vec_ptr = (size_t)&parts[0];
	parts_vec.vec_len = 5;
	parts_vec.vec_cap = 5;

	WorldPack real_world;
	real_world.avatarparts = parts_vec;
	real_world.rigidbodies = bodies_vec;

	rd_netclient_real_world(&real_world);
}

void FDynamicalSystemsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("DynamicalSystems")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/RustyDynamics/target/Debug/RustyDynamics.dll"));
#elif PLATFORM_MAC
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/RustyDynamics/target/Debug/libRustyDynamics.dylib"));
#endif // PLATFORM_WINDOWS

	RustyDynamicsHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (RustyDynamicsHandle)
	{
		rb_log_fn(ffi_log);
		TestFFI();
		// Call the test function in the third party library that opens a message box
        
		//ExampleLibraryFunction();
//        FString CoordinateString = FString::Printf(TEXT("rd_get_pow_2_of_int32 %i"), rd_get_pow_2_of_int32(12));
//        FMessageDialog::Open(EAppMsgType::Ok, FText::AsCultureInvariant(CoordinateString));
        
//        const char* addr = std::string("127.shit.0.0:8080").c_str();
//        rd_netclient_new(addr);
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
	}
}

void FDynamicalSystemsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(RustyDynamicsHandle);
	RustyDynamicsHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDynamicalSystemsModule, DynamicalSystems)
