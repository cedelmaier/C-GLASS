#ifndef _SIMCORE_SOFT_POTENTIAL_H_
#define _SIMCORE_SOFT_POTENTIAL_H_

#include "auxiliary.h"
#include "interaction.h"
#include "potential_base.h"

class SoftPotential : public PotentialBase {
  protected:
    double eps_;
  public:
    SoftPotential() {}
    void CalcPotential(Interaction *ix) {
      double rmag = sqrt(ix->dr_mag2);
      double r6 = pow(rmag,6);
      double r8 = r6*rmag*rmag;
      double R = 0.85*ix->buffer_mag;
      double R8inv = 1.0/pow(R,8);
      double exp1 = eps_ * exp(-r8*R8inv);
      double ffac = -8.0 * r6 * (exp1*R8inv);
      double *dr = ix->dr;
      // Cut off the force at fcut
      if (ABS(ffac) > fcut_) {
        ffac = SIGNOF(ffac) * fcut_;
      }
      double fmag = 0.0;
      for (int i = 0; i < n_dim_; ++i) {
        ix->force[i] = ffac*dr[i];
        fmag += ix->force[i] * ix->force[i];
      }
      printf("%2.2f %2.2f %2.2f\n", dr[0], dr[1], dr[2]);
      printf("%2.2f\n", sqrt(fmag));
      for (int i=0; i<n_dim_; ++i)
        for (int j=0; j<n_dim_; ++j)
          ix->stress[n_dim_*i+j] = -dr[i]*ix->force[j];
      ix->pote = exp1;
    }

    void Init(system_parameters *params) {
      // Initialize potential params
      n_dim_  = params->n_dim;
      eps_    = params->soft_potential_mag;
      fcut_   = params->f_cutoff;

      // For SoftPotential potentials, the rcutoff is
      // restricted to be at 2^(1/6)sigma

      rcut_ = 3; // goes quickly to zero after 1.2*rs_
      rcut2_ = rcut_*rcut_;
    }
};

#endif

