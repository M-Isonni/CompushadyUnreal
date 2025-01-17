// Copyright 2023 - Roberto De Ioris.

#include "CompushadyCBV.h"

bool UCompushadyCBV::Initialize(const FString& Name, const uint8* Data, const int64 Size)
{
	if (Size <= 0)
	{
		return false;
	}

	// align to 16 bytes to be d3d12 compliant
	const int64 AlignedSize = Align(Size, 16);

	BufferData.AddZeroed(AlignedSize);

	if (Data)
	{
		FMemory::Memcpy(BufferData.GetData(), Data, Size);
	}

	bBufferDataDirty = true;

	FRHIUniformBufferLayoutInitializer LayoutInitializer(*Name, AlignedSize);

	UniformBufferLayoutRHIRef = RHICreateUniformBufferLayout(LayoutInitializer);
	if (!UniformBufferLayoutRHIRef.IsValid())
	{
		return false;
	}

	UniformBufferRHIRef = RHICreateUniformBuffer(nullptr, UniformBufferLayoutRHIRef, EUniformBufferUsage::UniformBuffer_MultiFrame, EUniformBufferValidation::None);
	if (!UniformBufferRHIRef.IsValid() || !UniformBufferRHIRef->IsValid())
	{
		return false;
	}

	return true;
}

FUniformBufferRHIRef UCompushadyCBV::GetRHI()
{
	return UniformBufferRHIRef;
}

bool UCompushadyCBV::BufferDataIsDirty() const
{
	return bBufferDataDirty;
}

void UCompushadyCBV::SyncBufferData(FRHICommandList& RHICmdList)
{
	RHICmdList.UpdateUniformBuffer(UniformBufferRHIRef, BufferData.GetData());
	bBufferDataDirty = false;
}

bool UCompushadyCBV::SetFloat(const int64 Offset, const float Value)
{
	return SetValue(Offset, Value);
}

bool UCompushadyCBV::SetFloatArray(const int64 Offset, const TArray<float>& Values)
{
	return SetArrayValue(Offset, Values);
}

bool UCompushadyCBV::SetInt(const int64 Offset, const int32 Value)
{
	return SetValue(Offset, Value);
}

bool UCompushadyCBV::SetUInt(const int64 Offset, const int64 Value)
{
	if (Value < 0)
	{
		return false;
	}
	return SetValue(Offset, static_cast<uint32>(Value));
}

bool UCompushadyCBV::SetUInt(const int64 Offset, const uint32 Value)
{
	return SetValue(Offset, Value);
}

bool UCompushadyCBV::SetDouble(const int64 Offset, const double Value)
{
	return SetValue(Offset, Value);
}

bool UCompushadyCBV::SetDoubleArray(const int64 Offset, const TArray<double>& Values)
{
	return SetArrayValue(Offset, Values);
}

bool UCompushadyCBV::SetTransformFloat(const int64 Offset, const FTransform& Transform, const bool bTranspose)
{
	if (IsValidOffset(Offset, 16 * sizeof(float)))
	{
		FMatrix44f Matrix(Transform.ToMatrixWithScale());
		FMemory::Memcpy(BufferData.GetData() + Offset, bTranspose ? Matrix.GetTransposed().M : Matrix.M, 16 * sizeof(float));
		bBufferDataDirty = true;
		return true;
	}
	return false;
}

bool UCompushadyCBV::SetTransformDouble(const int64 Offset, const FTransform& Transform, const bool bTranspose)
{
	if (IsValidOffset(Offset, 16 * sizeof(double)))
	{
		FMatrix44d Matrix(Transform.ToMatrixWithScale());
		FMemory::Memcpy(BufferData.GetData() + Offset, bTranspose ? Matrix.GetTransposed().M : Matrix.M, 16 * sizeof(double));
		bBufferDataDirty = true;
		return true;
	}
	return false;
}

bool UCompushadyCBV::SetPerspectiveFloat(const int64 Offset, const float HalfFOV, const int32 Width, const int32 Height, const float ZNear, const float ZFar, const bool bRightHanded, const bool bTranspose)
{
	if (IsValidOffset(Offset, 16 * sizeof(float)))
	{
		FPerspectiveMatrix44f Matrix(FMath::DegreesToRadians(HalfFOV), Width, Height, ZNear, ZFar);
		if (bRightHanded)
		{
			Matrix.M[2][2] = ((ZNear == ZFar) ? 0.0f : ZFar / (ZNear - ZFar));
			Matrix.M[2][3] *= -1;
			Matrix.M[3][2] = ((ZNear == ZFar) ? 0.0f : ZNear * ZFar / (ZNear - ZFar));
		}
		FMemory::Memcpy(BufferData.GetData() + Offset, bTranspose ? Matrix.GetTransposed().M : Matrix.M, 16 * sizeof(float));
		bBufferDataDirty = true;
		return true;
	}
	return false;
}

bool UCompushadyCBV::SetRotationFloat2(const int64 Offset, const float Radians)
{
	if (IsValidOffset(Offset, 4 * sizeof(float)))
	{
		float Matrix[4] = { FMath::Cos(Radians), -FMath::Sin(Radians), FMath::Sin(Radians), FMath::Cos(Radians) };
		FMemory::Memcpy(BufferData.GetData() + Offset, Matrix, 4 * sizeof(float));
		bBufferDataDirty = true;
		return true;
	}
	return false;
}

void UCompushadyCBV::BufferDataClean()
{
	bBufferDataDirty = false;
}

int64 UCompushadyCBV::GetBufferSize() const
{
	return BufferData.Num();
}

bool UCompushadyCBV::GetFloat(const int64 Offset, float& Value)
{
	return GetValue(Offset, Value);
}

bool UCompushadyCBV::GetDouble(const int64 Offset, double& Value)
{
	return GetValue(Offset, Value);
}

bool UCompushadyCBV::GetInt(const int64 Offset, int32& Value)
{
	return GetValue(Offset, Value);
}

bool UCompushadyCBV::GetUInt(const int64 Offset, int64& Value)
{
	uint32 OutValue;
	if (GetValue(Offset, OutValue))
	{
		Value = static_cast<uint32>(OutValue);
		return true;
	}
	return false;
}

bool UCompushadyCBV::GetUInt(const int64 Offset, uint32& Value)
{
	return GetValue(Offset, Value);
}

bool UCompushadyCBV::IsValidOffset(const int64 Offset, const int64 Size) const
{
	return Offset >= 0 && (Offset + Size <= BufferData.Num());
}

bool UCompushadyCBV::SetPerspectiveFromMinimalViewInfo(const int64 Offset, const FMinimalViewInfo& MinimalViewInfo, const bool bTranspose)
{
	if (IsValidOffset(Offset, 16 * sizeof(float)))
	{
		FMatrix Matrix = MinimalViewInfo.CalculateProjectionMatrix();
		FMemory::Memcpy(BufferData.GetData() + Offset, bTranspose ? Matrix.GetTransposed().M : Matrix.M, 16 * sizeof(float));
		bBufferDataDirty = true;
		return true;
	}
	return false;
}

bool UCompushadyCBV::SetPerspectiveFromCameraComponent(const int64 Offset, UCameraComponent* CameraComponent, const bool bTranspose)
{
	FMinimalViewInfo MinimalViewInfo;
	CameraComponent->GetCameraView(0, MinimalViewInfo);
	return SetPerspectiveFromMinimalViewInfo(Offset, MinimalViewInfo, bTranspose);
}

bool UCompushadyCBV::SetPerspectiveFromSceneCaptureComponent2D(const int64 Offset, USceneCaptureComponent2D* SceneCaptureComponent, const bool bTranspose)
{
	FMinimalViewInfo MinimalViewInfo;
	SceneCaptureComponent->GetCameraView(0, MinimalViewInfo);
	return SetPerspectiveFromMinimalViewInfo(Offset, MinimalViewInfo, bTranspose);
}