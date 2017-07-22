#pragma once

#include "Engine.h"
#include "DynamicalUtil.generated.h"



UCLASS( ClassGroup=(DynamicalSystems), meta=(BlueprintSpawnableComponent) )
class DYNAMICALSYSTEMS_API UDynamicalUtil : public UObject
{
	GENERATED_BODY()

public:	

	static int Test() { return 54; }

#ifdef EIGEN
	static void SVD(FMatrix& out, const FVector (&PointsA)[3], const FVector (&PointsB)[3], const FVector& CentroidA, const FVector& CentroidB);
#endif
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Float")
    static float MeanOfFloatArray(const TArray<float>& Samples);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math|Float")
    static float VarianceOfFloatArray(const TArray<float>& Samples);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Math|Float")
    static float MedianOfFloatArray(const TArray<float>& Samples);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="Math|Float")
    static float StandardDeviationOfFloatArray(const TArray<float>& Samples);
    
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(Exponent = "1.0"), Category = "Math|Float")
	static TArray<float> ExpandArray(const TArray<float>& Samples, const float Exponent);
};
