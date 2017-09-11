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

TArray<float> UDynamicalUtil::ExpandArray(const TArray<float>& Samples, const float Exponent)
{
	TArray<float> tempArray;					// Create tempArray.
	tempArray.Init(0.0f, Samples.Num());		// Populate it with as many zeroes as the incoming float array.
	float originalSum = 0.0f;					// Create originalSum.
	float processedSum = 0.0f;					// Create processedSum.
	float difference = 0.0f;					// Create difference.
	float energyDifferenceRatio = 0.0f;			// Create energyDifferenceRatio.

	for (auto I = 0; I < Samples.Num(); ++I) {	// Calculate the sum of all of the original samples in the input array.
		originalSum += Samples[I];
	}

	for (auto I = 0; I < Samples.Num(); ++I) {	// Set the indices of tempArray to ((InputArray[i] + 1) - (Mean(InputArray)) ^ Exponent).
		tempArray[I] = pow(((Samples[I] + 1.0f) - MeanOfFloatArray(Samples)), Exponent);
	}

	for (auto I = 0; I < Samples.Num(); ++I) {	// Calculate the sum of all samples processed above from the original vector.
		processedSum += tempArray[I];
	}

	difference = processedSum - originalSum;	// Calculate the difference between the processed sum and the original sum and set it to the difference variable.

	for (auto I = 0; I < Samples.Num(); ++I) {	// Recalculate the samples in tempArray as tempArray[i] - (difference / tempArray.Length).
		tempArray[I] = tempArray[I] - (difference / tempArray.Num());
		// Unipolarize the data set - normalize negative values to 0.
		if (tempArray[I] < 0.0f) { tempArray[I] = 0.0f; }
	}

	processedSum = 0.0f;
	for (auto I = 0; I < Samples.Num(); ++I) {	// Reset the value of processedSum to the new sum of the unipolarized array.
		processedSum += tempArray[I];
	}
	// Divide-by-zero suppression.
	if (FMath::IsNearlyZero(originalSum)) { originalSum = FLT_EPSILON; }

	energyDifferenceRatio = processedSum / originalSum;
	if (FMath::IsNearlyZero(energyDifferenceRatio)) { energyDifferenceRatio = 1.0f; }

	for (auto I = 0; I < Samples.Num(); ++I) {
		tempArray[I] = tempArray[I] / energyDifferenceRatio;
	}

	return tempArray;
}


