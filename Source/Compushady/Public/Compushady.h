// Copyright 2023 - Roberto De Ioris.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Runtime/Launch/Resources/Version.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCompushady, Log, All);

#if ENGINE_MAJOR_VERSION == 5
#if ENGINE_MINOR_VERSION == 2
#define COMPUSHADY_UE_VERSION 52
#define COMPUSHADY_CREATE_BUFFER RHICreateBuffer
#define COMPUSHADY_CREATE_SRV RHICreateShaderResourceView
#define COMPUSHADY_CREATE_UAV RHICreateUnorderedAccessView
#elif ENGINE_MINOR_VERSION == 3
#define COMPUSHADY_UE_VERSION 53
#define COMPUSHADY_CREATE_BUFFER RHICmdList.CreateBuffer
#define COMPUSHADY_CREATE_SRV RHICmdList.CreateShaderResourceView
#define COMPUSHADY_CREATE_UAV RHICmdList.CreateUnorderedAccessView
#endif
#endif

namespace Compushady
{
	enum class ECompushadySharedResourceType : uint8
	{
		UniformBuffer,
		Buffer,
		StructuredBuffer,
		ByteAddressBuffer,
		Texture
	};

	struct FCompushadyShaderResourceBinding
	{
		uint32 BindingIndex;
		uint32 SlotIndex;
		FString Name;
		ECompushadySharedResourceType Type;
	};

	struct FCompushadyShaderSemantic
	{
		FString Name;
		uint32 Index;
		uint32 Register;
		uint32 Mask;

		bool operator==(const FCompushadyShaderSemantic& Other) const
		{
			return Name == Other.Name && Index == Other.Index && Register == Other.Register && Mask == Other.Mask;
		}

		FCompushadyShaderSemantic(const FString& InName, const uint32 InIndex, const uint32 InRegister, const uint32 InMask)
		{
			Name = InName;
			Index = InIndex;
			Register = InRegister;
			Mask = InMask;
		}
	};

	struct FCompushadyShaderResourceBindings
	{
		TArray<FCompushadyShaderResourceBinding> CBVs;
		TArray<FCompushadyShaderResourceBinding> SRVs;
		TArray<FCompushadyShaderResourceBinding> UAVs;

		TArray<FCompushadyShaderSemantic> InputSemantics;
		TArray<FCompushadyShaderSemantic> OutputSemantics;
	};

	bool CompileHLSL(const TArray<uint8>& ShaderCode, const FString& EntryPoint, const FString& TargetProfile, TArray<uint8>& ByteCode, FCompushadyShaderResourceBindings& ShaderResourceBindings, FIntVector& ThreadGroupSize, FString& ErrorMessages);
	bool CompileGLSL(const TArray<uint8>& ShaderCode, const FString& EntryPoint, const FString& TargetProfile, TArray<uint8>& ByteCode, FString& ErrorMessages);
	bool FixupSPIRV(TArray<uint8>& ByteCode, FCompushadyShaderResourceBindings& ShaderResourceBindings, FIntVector& ThreadGroupSize, FString& ErrorMessages);
	bool FixupDXIL(TArray<uint8>& ByteCode, FCompushadyShaderResourceBindings& ShaderResourceBindings, FIntVector& ThreadGroupSize, FString& ErrorMessages);
	bool DisassembleSPIRV(const TArray<uint8>& ByteCode, FString& Disassembled, FString& ErrorMessages);
	bool DisassembleDXIL(const TArray<uint8>& ByteCode, FString& Disassembled, FString& ErrorMessages);
	bool SPIRVToHLSL(const TArray<uint8>& ByteCode, FString& HLSL, FString& ErrorMessages);
	bool SPIRVToGLSL(const TArray<uint8>& ByteCode, FString& GLSL, FString& ErrorMessages);
	bool SPIRVToMSL(const TArray<uint8>& ByteCode, FString& MSL, FString& ErrorMessages);

	void StringToShaderCode(const FString& Code, TArray<uint8>& ShaderCode);
	bool ToUnrealShader(const TArray<uint8>& ByteCode, TArray<uint8>& Blob, const uint32 NumCBVs, const uint32 NumSRVs, const uint32 NumUAVs);

	void DXCTeardown();
}

class FCompushadyModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
