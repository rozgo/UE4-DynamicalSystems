#include "DynamicalUtil.h"
#include "DynamicalSystemsPrivatePCH.h"

float UDynamicalUtil::MeanOfFloatArray(const TArray<float>& Samples)
{
    float T = 0.0f;
    for (auto I = 0; I < Samples.Num(); ++I) {
        T += Samples[I];
    }
    return T / Samples.Num();
}

float UDynamicalUtil::VarianceOfFloatArray(const TArray<float>& Samples)
{
    float M = MeanOfFloatArray(Samples);
    float R = 0.0f;
    for (auto I = 0; I < Samples.Num(); ++I) {
        R += FMath::Pow(Samples[I] - M, 2.0f);
    }
    return R / Samples.Num();
}

float UDynamicalUtil::MedianOfFloatArray(const TArray<float>& Samples)
{
    TArray<float> S;
    S.Append(Samples);
    S.Sort();
    
    int Length = S.Num();
    if (Length == 0) {
        return 0;
    }
    else if (Length == 1) {
        return S[0];
    }
    else if (Length % 2) {
        int Index = Length / 2;
        return S[Index];
    }
    else {
        int IndexA = Length / 2 - 1;
        int IndexB = Length / 2;
        return (S[IndexA] + S[IndexB]) / 2.0f;
    }
}

float UDynamicalUtil::StandardDeviationOfFloatArray(const TArray<float>& Samples)
{
    float V = VarianceOfFloatArray(Samples);
    return FMath::Sqrt(V);
}

FVector UDynamicalUtil::CubicBezier(float Time, FVector P0, FVector P1, FVector P2, FVector P3)
{
	// p = (1-t)^3*p0 + 3*t*(1-t)^2*p1 + 3*t^2*(1-t)*p2 + t^3*p3
	return
		P0 * FMath::Pow(1.f - Time, 3.f) +
		P1 * 3.f * Time * FMath::Pow(1.f - Time, 2.f) +
		P2 * 3.f * FMath::Pow(Time, 2.f) * (1.f - Time) +
		P3 * FMath::Pow(Time, 3.f);
}


