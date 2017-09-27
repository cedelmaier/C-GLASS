#ifndef _SIMCORE_MINIMUM_DISTANCE_H_
#define _SIMCORE_MINIMUM_DISTANCE_H_

#include "object.h"
#include "interaction.h"
typedef std::pair< Object*, Interaction > boundary_interaction;

class MinimumDistance {
  private:
    int n_dim_,
        n_periodic_;
    double * unit_cell_,
           boundary_cut2_;
    space_struct * space_;
    void PointPoint(double const * const r1, double const * const s1, 
                    double const * const r2, double const * const s2, 
                    double *dr, double *dr_mag2);
    void PointCarrierLineInf(double *r_point, double *s_point,
                             double *r_line, double *s_line, double *u_line,
                             double length, double *dr, double *mu);
    void PointCarrierLine(double *r_point, double *s_point,
                          double *r_line, double *s_line, double *u_line,
                          double length, double *dr, double *r_contact,
                          double *mu_ret);
    void Sphero(double const * const r_1, double const * const s_1,
                double const * const u_1, double const length_1,
                double const * const r_2, double const * const s_2, 
                double const * const u_2, double const length_2,
                double *r_min, double *r_min_mag2, 
                double *contact1, double *contact2);
    void SpheroDr(double *r_1, double *s_1, double *u_1, double length_1,
                  double *r_2, double *s_2, double *u_2, double length_2,
                  double *dr, double *r_min, double *r_min_mag2, 
                  double *lambda, double *mu);
    void SphereSphero(double const * const r_1, double const * const s_1,
                      double const * const r_2, double const * const s_2, 
                      double const * const u_2, double const length_2,
                      double *r_min, double *r_min_mag2,
                      double *contact2);
    void SpheroPlane(double *r_bond, double *u_bond, double length,
                     double *r_plane, double *n_plane, double *lambda,
                     double *r_min_mag2, double *r_min);
    void CarrierLines(double *r_1, double *s_1, double *u_1, 
                      double *r_2, double *s_2, double *u_2, 
                      double *r_min, double *r_min_mag2, 
                      double *lambda, double *mu);
    void PointSphereBC(double const * const r, double *dr, double *dr_mag2);
    void SpheroSphereBC(double const * const r, double const * const u, double const length, 
                        double * r_contact, double *dr, double *dr_mag2);
    void PointBuddingBC(double const * const r, double *dr, double *dr_mag2);
    void SpheroBuddingBC(double const * const r, double const * const u, double const length,
                         double *dr, double *dr_mag2, double *r_contact);

  public:
    MinimumDistance() {}
    void Init(space_struct * space, double boundary_cutoff_sq);
    void ObjectObject(Object* o1, Object* o2, Interaction *ix);
    bool CheckBoundary(Object *o1, Interaction *ix);
    void BoundaryCondition(std::vector<boundary_interaction>::iterator bix);
};

#endif
