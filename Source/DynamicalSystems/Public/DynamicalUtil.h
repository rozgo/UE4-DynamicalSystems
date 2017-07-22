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
};
