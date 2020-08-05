#ifndef _CGLASS_SPHEROCYLINDER_SPECIES_H_
#define _CGLASS_SPHEROCYLINDER_SPECIES_H_

#include "species.hpp"
#include "spherocylinder.hpp"

class SpherocylinderSpecies
    : public Species<Spherocylinder, species_id::spherocylinder> {
protected:
  bool midstep_;
  double **pos0_, **u0_, *msd_, *msd_err_, *vcf_, *vcf_err_, *pressure_;
  int time_, time_avg_interval_, n_samples_;
  std::fstream diff_file_;
  void InitDiffusionAnalysis();
  void DiffusionAnalysis();
  void CalculateMSD();
  void CalculateVCF();
  void CalculatePressure();
  void UpdateInitPositions();
  void FinalizeDiffusionAnalysis();

public:
  SpherocylinderSpecies(unsigned long seed);
  void Init(std::string spec_name, ParamsParser &parser);
  void UpdatePositions() {
    for (auto it = members_.begin(); it != members_.end(); ++it) {
      it->UpdatePosition();
    }
  }
  virtual void InitAnalysis();
  virtual void RunAnalysis();
  virtual void FinalizeAnalysis();
};

class DiffusionAnalysis : public Analysis<SpherocylinderSpecies, species_id::spherocylinder> {
protected:
  void InitOutput() {
    SetAnalysisName("diffusion");
    Analysis::InitOutput();
  }
  void InitAnalysis() {
    output_ << "diffusion_analysis_file\n";
    RequireInteractionAnalysis();
    members_->at(0).InitAnalysis();
  }
  void RunAnalysis() {
    members_->at(0).RunAnalysis();
  }
  void EndAnalysis() {
    members_->at(0).FinalizeAnalysis();
  }
};
#endif
