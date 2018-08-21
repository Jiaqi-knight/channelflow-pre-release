/**
 * This file is a part of channelflow version 2.0.
 * License is GNU GPL version 2 or later: https://channelflow.org/license
 */
#ifndef NSOLVER_ARNOLDI_H
#define NSOLVER_ARNOLDI_H
#include "cfbasics/cfbasics.h"

using namespace Eigen;
using namespace cfbasics;
namespace nsolver {

/*==================================================================================*/
/*            Class Arnoldi                                                         */
/*==================================================================================*/

// Arnoldi iteration to estimate eigenvalues of matrix A. Usage:
// Arnoldi arnoldi(N, b);
// for (int n=0; n<N; ++n) {
//   VectorXd q  = arnoldi.testVector();
//   VectorXd Aq = A*q; // or however else you calculate A*q
//   arnoldi.iterate(Aq);
//   VectorXcd ew = arnoldi.ew(); // current estimate of eigenvalues
// }

class Arnoldi {
   public:
    Arnoldi();
    Arnoldi(const VectorXd& b, int Niterations, Real minCondition = 1e-13);

    const VectorXd& testVector() const;        // get test vector q
    virtual void iterate(const VectorXd& Aq);  // tell Arnoldi the value of Aq

    void orthocheck();  // save Q' Q into file QtQ.asc, should be I.

    int n() const;      // current iteration number
    int Niter() const;  // total number iterations

    const VectorXcd& ew();  // current estimate of eigenvals
    const MatrixXcd& ev();  // current estimate of eigenvecs
    const VectorXd& rd();   // current estimate of eigenvecs residual

    virtual ~Arnoldi(){};

   protected:
    int M_;      // dimension of linear operator (A is M x M)
    int Niter_;  // number of Arnoldi iterations
    int n_;      // current iteration number
    Real condition_;

    MatrixXd H_;
    MatrixXd Q_;
    MatrixXd Vn_;
    VectorXd qn_;
    VectorXd v_;

    VectorXcd ew_;
    MatrixXcd ev_;
    VectorXd rd_;

    virtual void eigencalc();
};

}  // namespace nsolver
#endif
