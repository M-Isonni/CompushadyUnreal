// Copyright 2023 - Roberto De Ioris.


#include "CompushadySRV.h"

bool UCompushadySRV::InitializeFromTexture(FTextureRHIRef InTextureRHIRef)
{
	if (!InTextureRHIRef)
	{
		return false;
	}

	TextureRHIRef = InTextureRHIRef;

	ENQUEUE_RENDER_COMMAND(DoCompushadyCreateShaderResourceView)(
		[this](FRHICommandListImmediate& RHICmdList)
		{

			SRVRHIRef = COMPUSHADY_CREATE_SRV(TextureRHIRef, 0);

		});

	FlushRenderingCommands();
	if (!SRVRHIRef)
	{
		return false;
	}

	InitFence(this);

	if (InTextureRHIRef->GetOwnerName() == NAME_None)
	{
		InTextureRHIRef->SetOwnerName(*GetPathName());
	}

	RHITransitionInfo = FRHITransitionInfo(TextureRHIRef, ERHIAccess::Unknown, ERHIAccess::SRVMask);

	return true;
}

bool UCompushadySRV::InitializeFromSceneTexture(const ECompushadySceneTexture InSceneTexture)
{
	if (InSceneTexture == ECompushadySceneTexture::None)
	{
		return false;
	}

	SceneTexture = InSceneTexture;

	return true;
}

bool UCompushadySRV::InitializeFromBuffer(FBufferRHIRef InBufferRHIRef, const EPixelFormat PixelFormat)
{
	if (!InBufferRHIRef)
	{
		return false;
	}

	BufferRHIRef = InBufferRHIRef;

	ENQUEUE_RENDER_COMMAND(DoCompushadyCreateShaderResourceView)(
		[this, PixelFormat](FRHICommandListImmediate& RHICmdList)
		{

			SRVRHIRef = COMPUSHADY_CREATE_SRV(BufferRHIRef, GPixelFormats[PixelFormat].BlockBytes, PixelFormat);

		});

	FlushRenderingCommands();

	if (!SRVRHIRef)
	{
		return false;
	}

	InitFence(this);

	if (InBufferRHIRef->GetOwnerName() == NAME_None)
	{
		InBufferRHIRef->SetOwnerName(*GetPathName());
	}

	RHITransitionInfo = FRHITransitionInfo(BufferRHIRef, ERHIAccess::Unknown, ERHIAccess::SRVMask);

	return true;
}

bool UCompushadySRV::InitializeFromStructuredBuffer(FBufferRHIRef InBufferRHIRef)
{
	if (!InBufferRHIRef)
	{
		return false;
	}

	if (InBufferRHIRef->GetStride() == 0)
	{
		return false;
	}

	BufferRHIRef = InBufferRHIRef;

	ENQUEUE_RENDER_COMMAND(DoCompushadyCreateShaderResourceView)(
		[this](FRHICommandListImmediate& RHICmdList)
		{

			SRVRHIRef = COMPUSHADY_CREATE_SRV(BufferRHIRef);

		});

	FlushRenderingCommands();

	if (!SRVRHIRef)
	{
		return false;
	}

	InitFence(this);

	if (InBufferRHIRef->GetOwnerName() == NAME_None)
	{
		InBufferRHIRef->SetOwnerName(*GetPathName());
	}

	RHITransitionInfo = FRHITransitionInfo(BufferRHIRef, ERHIAccess::Unknown, ERHIAccess::SRVMask);

	return true;
}

FTextureRHIRef UCompushadySRV::GetRHI(const FPostProcessMaterialInputs& PPInputs) const
{

	switch (SceneTexture)
	{
	case(ECompushadySceneTexture::SceneColorInput):
		return PPInputs.GetInput(EPostProcessMaterialInput::SceneColor).Texture->GetRHI();
		break;
	case(ECompushadySceneTexture::GBufferA):
		return PPInputs.SceneTextures.SceneTextures->GetContents()->GBufferATexture->GetRHI();
		break;
	case(ECompushadySceneTexture::GBufferB):
		return PPInputs.SceneTextures.SceneTextures->GetContents()->GBufferBTexture->GetRHI();
		break;
	case(ECompushadySceneTexture::GBufferC):
		return PPInputs.SceneTextures.SceneTextures->GetContents()->GBufferCTexture->GetRHI();
		break;
	case(ECompushadySceneTexture::GBufferD):
		return PPInputs.SceneTextures.SceneTextures->GetContents()->GBufferDTexture->GetRHI();
		break;
	case(ECompushadySceneTexture::GBufferE):
		return PPInputs.SceneTextures.SceneTextures->GetContents()->GBufferETexture->GetRHI();
		break;
	case(ECompushadySceneTexture::GBufferF):
		return PPInputs.SceneTextures.SceneTextures->GetContents()->GBufferFTexture->GetRHI();
		break;
	case(ECompushadySceneTexture::SceneColor):
		return PPInputs.SceneTextures.SceneTextures->GetContents()->SceneColorTexture->GetRHI();
		break;
	case(ECompushadySceneTexture::Depth):
		return PPInputs.SceneTextures.SceneTextures->GetContents()->SceneDepthTexture->GetRHI();
		break;
	case(ECompushadySceneTexture::CustomDepth):
		return PPInputs.SceneTextures.SceneTextures->GetContents()->CustomDepthTexture->GetRHI();
		break;
	default:
		return nullptr;
	}
}

FShaderResourceViewRHIRef UCompushadySRV::GetRHI() const
{
	return SRVRHIRef;
}

bool UCompushadySRV::IsSceneTexture() const
{
	return SceneTexture != ECompushadySceneTexture::None;
}