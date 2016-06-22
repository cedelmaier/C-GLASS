#include "md_kmc_bead.h"

void MDKMCBead::Init() {
  Simple::Init();
  for (int i=0; i<n_dim_; ++i) {
    orientation_[i] = 1.0/sqrt(n_dim_);
    velocity_[i] = 4*(gsl_rng_uniform_pos(rng_.r)-0.5);
    prev_position_[i] = position_[i] - delta_ * velocity_[i];
  }
  orientation_[0] = 0; // Set default color to red
}
void MDKMCBead::UpdatePosition() {
  ZeroForce();
  ApplyInteractions();
  Integrate();
  UpdatePeriodic();
  ClearInteractions();
}

// Update the position based on MP
void MDKMCBead::UpdatePositionMP() {
    Integrate();
    UpdatePeriodic();
}

// Basic verlet integrator, very stable
void MDKMCBead::Integrate() {
  double delta2 = SQR(delta_);
  double temp_pos[3];
  for (int i=0; i<n_dim_; ++i) {
    temp_pos[i] = position_[i];
    position_[i] = 2.0*position_[i] - prev_position_[i] 
      + ((force_[i]/mass_))*delta2;
    velocity_[i] =  (position_[i] - prev_position_[i])/(2.0*delta_);
    prev_position_[i] = temp_pos[i];
    dr_tot_[i] += position_[i] - prev_position_[i];
  }
}

void MDKMCBead::UpdateKineticEnergy() {
  double vel_mag_sqr = 0.0;
  for (int i=0; i<n_dim_; ++i)
    vel_mag_sqr += SQR(velocity_[i]);
  k_energy_ = 0.5 * mass_ * vel_mag_sqr;
}

double const MDKMCBead::GetKineticEnergy() {
  UpdateKineticEnergy();
  return k_energy_;
}

void MDKMCBeadSpecies::InitPotentials (system_parameters *params) {
  AddPotential(SID::md_bead, SID::md_bead, 
      // Set md_bead-md_bead interaction
      new LJ126(params->lj_epsilon,params->md_bead_diameter,
        space_, 2.5*params->md_bead_diameter));
    //new WCA(params->lj_epsilon,params->md_bead_diameter,
     //   space_, pow(2.0,1.0/6.0) *params->md_bead_diameter));
}