#ifndef LINEARREGRESSION_HPP
#define LINEARREGRESSION_HPP

#include <ControlHelper.hpp>
#include <iostream>
namespace HapticGlove
{
class LinearRegression;
}

class HapticGlove::LinearRegression
{

public:
    LinearRegression();
    ~LinearRegression();

    bool Initialize();

    bool LearnOneShot(const CtrlHelper::Eigen_Mat& inputData,
                      const CtrlHelper::Eigen_Mat& outputData,
                      CtrlHelper::Eigen_Mat& tetha);

    bool LearnOneShotMatrix(const CtrlHelper::Eigen_Mat& inputData,
                            const CtrlHelper::Eigen_Mat& ouputData,
                            CtrlHelper::Eigen_Mat& theta); // A size: <n,m>

    bool LearnIncrementally();
};
template <typename DynamicEigenMatrix, typename DynamicEigenVector>
bool push_back_row(DynamicEigenMatrix& m, const DynamicEigenVector& values)
{
    if (m.size() != 0)
    {
        if (m.cols() != values.cols())
        {

            std::cerr << "[push_back_row]: the number of columns in matrix and vactor are not "
                         "equal; m.cols(): "
                      << m.cols() << ", values.cols():" << values.cols() << std::endl;
            return false;
        }
    }
    Eigen::Index row = m.rows();
    m.conservativeResize(row + 1, values.cols());
    for (int i = 0; i < values.cols(); i++)
        m(row, i) = values(0, i);
    return true;
}

#endif // LINEARREGRESSION_HPP
