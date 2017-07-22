#include "DynamicalSystems.h"
#include "DynamicalUtil.h"

#ifdef EIGEN

#include <Eigen/Core>
#include <Eigen/LU>
#include <Eigen/SVD>

void UDynamicalUtil::SVD(FMatrix& out, const FVector(&PointsA)[3], const FVector(&PointsB)[3], const FVector& CA, const FVector& CB)
{
	Eigen::Matrix<float, 3, 1> CentroidA(CA.X, CA.Y, CA.Z);
	Eigen::Matrix<float, 3, 1> CentroidB(CB.X, CB.Y, CB.Z);

	Eigen::Matrix<float, 3, 3> A;
	Eigen::Matrix<float, 3, 3> B;
	for (int I = 0; I < 3; ++I) {
		A.row(I) = Eigen::Matrix<float, 3, 1>(PointsA[I].X, PointsA[I].Y, PointsA[I].Z);
		B.row(I) = Eigen::Matrix<float, 3, 1>(PointsB[I].X, PointsB[I].Y, PointsB[I].Z);
	}

	Eigen::Matrix<float, 3, 3> H = A.transpose() * B;

	Eigen::JacobiSVD<Eigen::Matrix<float, 3, 3>> SVD(H, Eigen::ComputeThinU | Eigen::ComputeThinV);
	Eigen::Matrix<float, 3, 3> U = SVD.matrixU();
	Eigen::Matrix<float, 3, 3> V = SVD.matrixV();
	Eigen::Matrix<float, 3, 3> R = V * U.transpose();

	float D = R.determinant();
	if (D < 0) {
		V.col(2) = -V.col(2);
		R = V * U.transpose();
	}

	Eigen::Matrix<float, 3, 1> T = (-R * CentroidA).transpose() + CentroidB.transpose();

	out = FMatrix::Identity;

	out.M[0][0] = R(0, 0);
	out.M[1][0] = R(0, 1);
	out.M[2][0] = R(0, 2);

	out.M[0][1] = R(1, 0);
	out.M[1][1] = R(1, 1);
	out.M[2][1] = R(1, 2);

	out.M[0][2] = R(2, 0);
	out.M[1][2] = R(2, 1);
	out.M[2][2] = R(2, 2);

	out = out.ConcatTranslation(FVector(T(0, 0), T(1, 0), T(2, 0)));
}

#endif

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


