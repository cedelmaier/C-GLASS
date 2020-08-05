#ifndef _CGLASS_RIGID_FILAMENT_SPECIES_H_
#define _CGLASS_RIGID_FILAMENT_SPECIES_H_

#include "rigid_filament.hpp"
#include "species.hpp"
#include "rigid_filament_anisotropy_density_analysis.hpp"

typedef Analysis<RigidFilament, species_id::rigid_filament> RigidFilamentAnalysis;

class RigidFilamentSpecies
    : public Species<RigidFilament, species_id::rigid_filament> {
 protected:
  double fill_volume_;
  double packing_fraction_;
 public:
  RigidFilamentSpecies(unsigned long seed);
  void Init(std::string spec_name, ParamsParser &parser);
  void PopMember();

  void AddMember();

  void Reserve();
  void UpdatePositions();
  void LoadAnalysis();
  // Redundant for filaments.
  virtual void CenteredOrientedArrangement() {}
};

#endif
