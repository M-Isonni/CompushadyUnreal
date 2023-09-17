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

	RHITransitionInfo = FRHITransitionInfo(TextureRHIRef, ERHIAccess::Unknown, ERHIAccess::SRVCompute);

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

	RHITransitionInfo = FRHITransitionInfo(BufferRHIRef, ERHIAccess::Unknown, ERHIAccess::SRVCompute);

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

	RHITransitionInfo = FRHITransitionInfo(BufferRHIRef, ERHIAccess::Unknown, ERHIAccess::SRVCompute);

	return true;
}

FShaderResourceViewRHIRef UCompushadySRV::GetRHI() const
{
	return SRVRHIRef;
}