#ifndef _CGLASS_RIGID_FILAMENT_ANISOTROPY_DENSITY_ANALYSIS_H_
#define _CGLASS_RIGID_FILAMENT_ANISOTROPY_DENSITY_ANALYSIS_H_

#include "analysis.hpp"
#include "rigid_filament.hpp"
#include "minimum_distance.hpp"

class AnisotropyDensityAnalysis : public Analysis<RigidFilament, species_id::rigid_filament> {
protected:
  void InitOutput() {
    SetAnalysisName("anisotropy_density");
    Analysis::InitOutput();
  }
  void InitAnalysis() {
    output_ << "anisotropy_density_analysis_file\n";
    output_ << "y w\n";
    RequireInteractionAnalysis();
  }
  void RunAnalysis() {
    double avg_pos_i[3] = {0};
    double avg_pos_j[3] = {0};
    int num_interactions = 0;
    double interaction_force[3] = {0};
    double interaction_dr[3] = {0};
    double w = 0;
    double dx = 0;
    double dy = 0;
    for (auto it_i = members_->begin(); it_i != members_->end(); ++it_i) {
      std::vector<object_interaction> interactions;
      it_i->GetInteractions(interactions);
      for (auto it_j = interactions.begin(); it_j != interactions.end(); ++it_j) {
        /*it_j->first->obj1->GetAvgPosition(avg_pos_i);
        it_j->first->obj2->GetAvgPosition(avg_pos_j);*/
        if (it_j->first->obj1->GetMeshID() == it_j->first->obj2->GetMeshID()) {
          continue;
        } // Don't include intrafilament bonds
        std::copy(it_j->first->force, it_j->first->force + 3, interaction_force);
        std::copy(it_j->first->dr, it_j->first->dr + 3, interaction_dr);
        if ((interaction_force[0] > 0 || interaction_force[1] > 0) && it_j->first->force[0] < 1e200) {
          /*dx = avg_pos_i[0] - avg_pos_j[0];
          dy = avg_pos_i[1] - avg_pos_j[1];*/
          w = interaction_force[1]*interaction_dr[1] - interaction_force[0]*interaction_dr[0];
          output_ << interaction_dr[0] << " " << w << "\n";
          num_interactions++;
        }
      }
    }
    num_interactions /= 2; //double-counted
    output_ << num_interactions << "\n";
  }
  void EndAnalysis() {}
};
#endif 
