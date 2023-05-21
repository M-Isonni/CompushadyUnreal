// Copyright 2023 - Roberto De Ioris.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCompushady, Log, All);

namespace Compushady
{
	enum class ECompushadySharedResourceType : uint8
	{
		Buffer,
		Texture
	};
	struct FCompushadyShaderResourceBinding
	{
		uint32 BindingIndex;
		uint32 SlotIndex;
		FString Name;
		ECompushadySharedResourceType Type;
	};

	struct FCompushadyShaderResourceBindings
	{
		TArray<FCompushadyShaderResourceBinding> CBVs;
		TArray<FCompushadyShaderResourceBinding> SRVs;
		TArray<FCompushadyShaderResourceBinding> UAVs;
	};

	bool CompileHLSL(const TArray<uint8>& ShaderCode, const FString& EntryPoint, const FString& TargetProfile, TArray<uint8>& ByteCode, FCompushadyShaderResourceBindings& ShaderResourceBindings, FString& ErrorMessages);
}

class FCompushadyModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
