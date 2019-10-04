#ifndef _SIMCORE_BEAD_SPRING_H_
#define _SIMCORE_BEAD_SPRING_H_

#include "mesh.hpp"
class BeadSpring : public Mesh {
 private:
  int stoch_flag_, eq_steps_, eq_steps_count_ = 0;
  double max_bond_length_, bond_rest_length_, bond_spring_, persistence_length_,
      rand_sigma_, driving_factor_;
  std::vector<double> cos_thetas_;
  void SetDiffusion();
  void GenerateProbableOrientation();
  void CalculateAngles();
  void CalculateTangents();
  void CalcRandomForces();
  void AddRandomForces();
  void AddBendingForces();
  void AddSpringForces();
  void UpdateSitePositions(bool midstep);
  void UpdateSiteOrientations();
  void ApplyForcesTorques();
  void ApplyInteractionForces();
  void SetParameters();
  void InitElements();
  void InsertBeadSpring();
  void InsertFirstBond();
  void UpdateAvgPosition();
  void ReportAll();

 public:
  BeadSpring();
  virtual void Init();
  virtual void InsertAt(double *pos, double *u);
  // void DiffusionValidationInit();
  virtual void Integrate(bool midstep);
  // virtual void Draw(std::vector<graph_struct*> * graph_array);
  virtual void UpdatePosition() {}
  virtual void UpdatePosition(bool midstep);
  double const GetLength() { return length_; }
  double const GetDriving() { return driving_factor_; }
  double const GetPersistenceLength() { return persistence_length_; }
  double const GetBondLength() { return bond_length_; }
  int const GetNBonds() { return n_bonds_; }
  void GetAvgOrientation(double *au);
  void GetAvgPosition(double *ap);
  std::vector<double> const *const GetThetas() { return &cos_thetas_; }
  double GetTipZ() { return sites_[n_sites_ - 1].GetOrientation()[n_dim_ - 1]; }
  double const *const GetHeadPosition() {
    return sites_[n_sites_ - 1].GetPosition();
  }
  double const *const GetTailPosition() { return sites_[0].GetPosition(); }
  double const *const GetTailOrientation() {
    return sites_[0].GetOrientation();
  }
  void AddTorqueTail(double *t) { bonds_[0].AddTorque(t); }
  void AddForceTail(double *f) { sites_[0].AddForce(f); }
  double GetContourLength() {
    double clen = 0;
    for (int i = 0; i < n_bonds_; ++i) {
      clen += bonds_[i].GetLength();
    }
    return clen;
  }
  void WritePosit(std::fstream &oposit);
  void ReadPosit(std::fstream &iposit);
  void WriteSpec(std::fstream &ospec);
  void ReadSpec(std::fstream &ispec);
  void WriteCheckpoint(std::fstream &ocheck);
  void ReadCheckpoint(std::fstream &icheck);
  void ScalePosition();
  double const GetVolume();
};

#endif  // _SIMCORE_BEAD_SPRING_H_
