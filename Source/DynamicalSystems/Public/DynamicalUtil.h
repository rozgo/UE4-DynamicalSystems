#pragma once

#include "Engine.h"
#include "DynamicalUtil.generated.h"

UCLASS( ClassGroup=(DynamicalSystems), meta=(BlueprintSpawnableComponent) )
class DYNAMICALSYSTEMS_API UDynamicalUtil : public UObject
{
	GENERATED_BODY()

public:	
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Float")
    static float MeanOfFloatArray(const TArray<float>& Samples);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Float")
    static float VarianceOfFloatArray(const TArray<float>& Samples);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Math|Float")
    static float MedianOfFloatArray(const TArray<float>& Samples);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Math|Float")
    static float StandardDeviationOfFloatArray(const TArray<float>& Samples);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Math|Vector")
    static FVector CubicBezier(float Time, FVector P0, FVector P1, FVector P2, FVector P3);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (Exponent = "1.0"), Category = "Math|Float")
	static TArray<float> ExpandArray(const TArray<float>& Samples, const float Exponent);
};
