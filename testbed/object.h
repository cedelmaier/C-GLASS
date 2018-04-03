#include "params.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <vector>

class Site {
  public:
    gsl_rng * rng;
    double pos[NDIM];
    double spos[NDIM];
    double pos0[NDIM];
    double dr;
    void InitRandomPosition() {
      for (int i=0; i<NDIM; ++i) {
        pos[i] = BOX_SIZE*(gsl_rng_uniform_pos(rng)-0.5);
        pos0[i] = pos[i];
        dr=0;
      }
      PBC();
    }
    double GetDr() {
      double dx = pos[0]-pos0[0];
      double dy = pos[1]-pos0[1];
      dx = (dx < 0 ? -dx : dx);
      dy = (dy < 0 ? -dy : dy);
      dr = (dx > dy ? dx : dy);
      return dr;
    }
    void UpdatePos0 () {
      for (int i=0;i<NDIM; ++i) {
        pos0[i] = pos[i];
      }
    }
    void UpdatePosition() {
      for (int i=0; i<NDIM; ++i) {
        pos[i] += (gsl_rng_uniform_pos(rng)-0.5)*0.01;
      }
      PBC();
    }
    void PBC() {
      for (int i = 0; i < NPER; ++i) {
        spos[i] = 0.0;
        for (int j = 0; j < NPER; ++j) {
          spos[i] += (i==j ? 1.0 : 0) * pos[j];
          //spos[i] += (i==j ? 1.0/BOX_SIZE : 0) * pos[j];
        }
        spos[i] -= NINT(spos[i]);
      }
    }
};

typedef std::vector<Site>::iterator site_it;
