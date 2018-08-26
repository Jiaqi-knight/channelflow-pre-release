/**
 * This file is a part of channelflow version 2.0.
 * License is GNU GPL version 2 or later: https://channelflow.org/license
 */

#ifndef NSOLVER_NEWTONALGORITHM_H
#define NSOLVER_NEWTONALGORITHM_H

#include <memory>
#include "cfbasics/arglist.h"
#include "cfbasics/cfbasics.h"

#include "cfbasics/brent.h"
#include "nsolver/bicgstabl.h"
#include "nsolver/fgmres.h"
#include "nsolver/gmres.h"
#include "nsolver/newton.h"

namespace nsolver {
//   /*==================================================================================*/
//   /*            function  GMRESHookstep and friends                                   */
//   /*==================================================================================*/
//
//   // Find solution of equation sigma f^T(u) - u == 0. Inputs are initial
//   // guess and return as solution values. Real return value is residual
//   // L2Norm(sigma f^T(u) - u)
//   Real GMRESHookstep(FlowField& u, Real& T, FieldSymmetry& sigma,
//                PoincareCondition* h, const GMRESHookstepFlags& searchflags,
//                const DNSFlags& dnsflags, const TimeStep& dt, Real& CFL);
//
//

//   // Versions of f, G, DG that handle Poincare section calculations, additionally.
//   void fp(const FlowField& u, Real& T, PoincareCondition* h,
//        FlowField& fu, const DNSFlags& flags, const TimeStep& dt, int& fcount, Real& CFL,
//        std::ostream& os=std::cout);
//
//   // sf(u,sigma,T) = sigma f^T(u)
//   void sfp(const FlowField& u, Real T, PoincareCondition* h,
//         const FieldSymmetry& sigma, FlowField& sfu, const DNSFlags& flags,
//         const TimeStep& dt, int& fcount, Real& CFL, std::ostream& os=std::cout);
//
//   // Dsf(u,sigma,T,du) = sigma f^T(u+du) - sigma f^T(u)
//   void Dsfp(const FlowField& u, Real T, PoincareCondition* h,
//          const FieldSymmetry& sigma, const FlowField& sfu, const FlowField& du,
//          FlowField& Dsf_du, const DNSFlags& flags, const TimeStep& dt, Real eps,
//          bool centerdiff, int& fcount, Real& CFL, std::ostream& os=std::cout);
//
//   void Gp(const FlowField& u, Real& T, PoincareCondition* h,
//        const FieldSymmetry& sigma, FlowField& Gu, const DNSFlags& flags, const TimeStep& dt,
//        bool Tnormalize, bool Unormalize, int& fcount, Real& CFL, std::ostream& os=std::cout) ;
//
//   void DGp(const FlowField& u, const FlowField& du, Real& T, Real dT,
//         PoincareCondition* h, const FieldSymmetry& sigma, const FieldSymmetry& dsigma,
//         const FlowField& Gu, FlowField& DG_dx, const DNSFlags& flags, const TimeStep& dt,
//         bool Tnormalize, bool Unormalize, Real epsDu, bool centdiff, int& fcount, Real& CFL, std::ostream&
//         os=std::cout);

class NewtonSearchFlags {
   public:
    cfbasics::SolverMethod solver = cfbasics::SolverGMRES;
    cfbasics::OptimizationMethod optimization = cfbasics::Hookstep;
    cfbasics::SolutionType solntype;
    bool xrelative;  // new
    bool zrelative;  // new
    cfbasics::Real epsSearch;
    cfbasics::Real epsKrylov;
    cfbasics::Real epsDx;
    cfbasics::Real epsDt;
    cfbasics::Real epsSolver;
    cfbasics::Real epsSolverF;
    bool centdiff;
    int Nnewton;
    int Nsolver;
    int Nhook;
    cfbasics::Real delta;
    cfbasics::Real deltaMin;
    cfbasics::Real deltaMax;
    cfbasics::Real deltaFuzz;
    cfbasics::Real lambdaMin;
    cfbasics::Real lambdaMax;
    cfbasics::Real lambdaRequiredReduction;
    cfbasics::Real improvReq;
    cfbasics::Real improvOk;
    cfbasics::Real improvGood;
    cfbasics::Real improvAcc;
    int lBiCGStab;
    int nShot;
    bool fixtphase;
    cfbasics::Real TRef;
    cfbasics::Real axRef;
    cfbasics::Real azRef;
    cfbasics::Real gRatio;
    std::string outdir;
    std::ostream* logstream;
    bool verbose;
    bool orbit;
    bool laurette;

    NewtonSearchFlags(cfbasics::SolutionType solntype = cfbasics::Equilibrium, bool xrelative = false, bool zrelative = false,
                      cfbasics::Real epsSearch = 1e-13, cfbasics::Real epsKrylov = 1e-14, cfbasics::Real epsDx = 1e-7,
                      cfbasics::Real epsDt = 1e-5, cfbasics::Real epsSolver = 1e-3, cfbasics::Real epsSolverF = 0.05,
                      bool centdiff = false, int Nnewton = 20, int Ngmres = 500, int Nhook = 20,
                      cfbasics::Real delta = 1e-2, cfbasics::Real deltaMin = 1e-12, cfbasics::Real deltaMax = 1e-1,
                      cfbasics::Real deltaFuzz = 1e-6, cfbasics::Real lambdaMin = 0.2, cfbasics::Real lambdaMax = 1.5,
                      cfbasics::Real lambdaRequiredReduction = 0.5, cfbasics::Real improvReq = 1e-3,
                      cfbasics::Real improvOk = 1e-1, cfbasics::Real improvGood = 0.75, cfbasics::Real improvAcc = 1e-1,
                      int lBiCGStab = 2, int nShot = 1, bool fixtphase = false, cfbasics::Real TRef = 1.0,
                      cfbasics::Real axRef = 1.0, cfbasics::Real azRef = 1.0, cfbasics::Real gRatio = 10.0,
                      std::string outdir = "./", std::ostream* logstream = &std::cout, bool laurette = false);

    NewtonSearchFlags(cfbasics::ArgList& args);
    cfbasics::SolverMethod string2solver(std::string);
    std::string solver2string() const;
    cfbasics::OptimizationMethod string2optimization(std::string);
    std::string optimization2string() const;
    cfbasics::SolutionType string2solntype(std::string s) const;
    std::string solntype2string() const;
    void save(const std::string& outdir = "") const;
    void load(int taskid, const std::string indir);
    const std::vector<std::string> getFlagList();
};

/** Combined Newton Hookstep algorithm
 *
 * \param[in] dsiG DynamicalSystemsInterface specifying the equation to integrate
 * \param[in] x0 initial guess
 * \param[in] searchflags parameters for the search algorithms
 *
 * \return fixed point/one point on periodic orbit
 */
Eigen::VectorXd hookstepSearch(DSI& dsiG, const Eigen::VectorXd& x0, const NewtonSearchFlags& searchflags,
                               cfbasics::Real& gx);

class NewtonAlgorithm : public Newton {
   public:
    NewtonAlgorithm(NewtonSearchFlags searchflags);
    virtual Eigen::VectorXd solve(DSI& dsi, const Eigen::VectorXd& x, cfbasics::Real& residual);
    NewtonSearchFlags searchflags;

    virtual void setLogstream(std::ostream* os);
    virtual void setOutdir(std::string od);

    Eigen::MatrixXd jacobi(const Eigen::VectorXd& x, const cfbasics::Real epsilon, const bool centerdiff, int& fcount);

   private:
    int linear(Eigen::VectorXd& dxOpt, Eigen::VectorXd& GxOpt, const Eigen::VectorXd& x);
    int hookstep(Eigen::VectorXd& dxH, Eigen::VectorXd& GxH, const Eigen::VectorXd& x, const Eigen::VectorXd& b);
    int convergenceCheckAC(const Eigen::VectorXd& dx, Eigen::VectorXd& Gx, const Eigen::VectorXd& x);

    std::ostream* os;
    int fcount_newton_;
    int fcount_opt_;

    // required for Hookstep algorithm
    std::unique_ptr<GMRES> gmres_;
    std::unique_ptr<FGMRES> fgmres_;
    cfbasics::Real delta_;
    cfbasics::Real rx_;  // Dennis & Schnabel residual r(x) = 1/2 ||f(x)||^2
};

}  // namespace nsolver

#endif
