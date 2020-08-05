/*#ifndef _CGLASS_FILAMENT_ANISOTROPY_DENSITY_ANALYSIS_H_
#define _CGLASS_FILAMENT_ANISOTROPY_DENSITY_ANALYSIS_H_

#include "analysis.hpp"
#include "filament.hpp"
#include "minimum_distance.hpp"

class AnisotropyDensityAnalysis : public Analysis<Filament, species_id::filament> {
protected:
  void InitOutput() {
    SetAnalysisName("anisotropy_density");
    Analysis::InitOutput();
  }
  void InitAnalysis() {
    output_ << "anisotropy_density_analysis_file\n";
    output_ << "y F_y\n";
    RequireInteractionAnalysis();
  }
  void RunAnalysis() {
    double avg_pos_i[3] = {0};
    double avg_pos_j[3] = {0};
    int num_interactions = 0;
    for (auto it_i = members_->begin(); it_i != members_->end(); ++it_i) {
      std::vector<object_interaction> interactions;
      it_i->GetInteractions(interactions);
      num_interactions += interactions.size();
      for (auto it_j = interactions.begin(); it_j != interactions.end(); ++it_j) {
        it_j->first->obj1->GetAvgPosition(avg_pos_i);
        it_j->first->obj2->GetAvgPosition(avg_pos_j);
        output_ << avg_pos_i[1] - avg_pos_j[1] << " " << it_j->first->force[1] << "\n";
      }
    }
    num_interactions /= 2; //double-counted
    output_ << "num_interactions = " << num_interactions << "\n";
  }
  void EndAnalysis() {}
};
#endif */
