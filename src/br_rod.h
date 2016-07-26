#ifndef _SIMCORE_BR_ROD_H_
#define _SIMCORE_BR_ROD_H_

#include "site.h"
#include "bond.h"
#include "species.h"
#include "auxiliary.h"
#include "wca.h"

typedef enum {
  SHRINK = 0,
  GROW = 1,
  PAUSE
} poly_state_t;

class BrRod : public Composite<Site,Bond> {

  private:
    int n_bonds_,
        dynamic_instability_flag_,
        force_induced_catastrophe_flag_;
    double max_length_,
           min_length_,
           max_child_length_,
           child_length_,
           gamma_par_,
           gamma_perp_,
           gamma_rot_,
           rand_sigma_par_,
           rand_sigma_perp_,
           rand_sigma_rot_,
           v_poly_,
           v_depoly_,
           p_s2g_,
           p_s2p_,
           p_p2s_,
           p_p2g_,
           p_g2s_,
           p_g2p_,
           tip_force_,
           body_frame_[6];
    poly_state_t poly_state_;
    //Bond * bond_ptr_;
    void UpdateSiteBondPositions();
    void SetDiffusion();
    void UpdateOrientation();
    void GetBodyFrame();
    void AddRandomDisplacement();
    void ApplyForcesTorques();
    void DynamicInstability();

  public:
    BrRod(system_parameters *params, space_struct * space, long seed, SID sid) 
      : Composite(params, space, seed, sid) {
        //bond_ptr_ = new Bond(params, space, gsl_rng_get(rng_.r), GetSID()) ;
        length_ = params->rod_length;
        diameter_ = params->rod_diameter;
        max_length_ = params->max_rod_length;
        min_length_ = params->min_rod_length;
        max_child_length_ = 0.5*params->cell_length;
        dynamic_instability_flag_ = params->dynamic_instability_flag;
        force_induced_catastrophe_flag_ = params->force_induced_catastrophe_flag;
        p_g2s_ = params->f_grow_to_shrink*delta_;
        p_g2p_ = params->f_grow_to_pause*delta_;
        p_s2p_ = params->f_shrink_to_pause*delta_;
        p_s2g_ = params->f_shrink_to_grow*delta_;
        p_p2s_ = params->f_pause_to_shrink*delta_;
        p_p2g_ = params->f_pause_to_grow*delta_;
        v_depoly_ = params->v_depoly;
        v_poly_ = params->v_poly;
        // Initialize end sites
        for (int i=0; i<2; ++i) {
          Site s(params, space, gsl_rng_get(rng_.r), GetSID());
          s.SetCID(GetCID());
          elements_.push_back(s);
        }
        // Initialize bonds
        n_bonds_ = (int) ceil(length_/max_child_length_);
        child_length_ = length_/n_bonds_;
        for (int i=0; i<n_bonds_; ++i) {
          Bond b(params, space, gsl_rng_get(rng_.r), GetSID());
          b.SetCID(GetCID());
          b.SetRID(GetRID());
          //b.InitOID();
          v_elements_.push_back(b);
        }
      }
    ~BrRod() {}
    BrRod(const BrRod& that) : Composite(that) {
      n_bonds_=that.n_bonds_;
      dynamic_instability_flag_ = that.dynamic_instability_flag_;
      max_length_ = that.max_length_;
      min_length_ = that.min_length_;
      max_child_length_ = that.max_child_length_;
      gamma_par_ = that.gamma_par_;
      gamma_perp_ = that.gamma_perp_;
      gamma_rot_ = that.gamma_rot_;
      rand_sigma_par_ = that.rand_sigma_par_;
      rand_sigma_perp_ = that.rand_sigma_perp_;
      rand_sigma_rot_ = that.rand_sigma_rot_;
      v_poly_ = that.v_poly_;
      v_depoly_ = that.v_depoly_;
      p_s2g_ = that.p_s2g_;
      p_s2p_ = that.p_s2p_;
      p_p2s_ = that.p_p2s_;
      p_p2g_ = that.p_p2g_;
      p_g2s_ = that.p_g2s_;
      p_g2p_ = that.p_g2p_;
      tip_force_ = that.tip_force_;
      std::copy(that.body_frame_, that.body_frame_+6, body_frame_);
      poly_state_ = that.poly_state_;
    }
    BrRod& operator=(BrRod const& that) {
      Composite::operator=(that); 
      n_bonds_=that.n_bonds_;
      dynamic_instability_flag_ = that.dynamic_instability_flag_;
      max_length_ = that.max_length_;
      min_length_ = that.min_length_;
      max_child_length_ = that.max_child_length_;
      gamma_par_ = that.gamma_par_;
      gamma_perp_ = that.gamma_perp_;
      gamma_rot_ = that.gamma_rot_;
      rand_sigma_par_ = that.rand_sigma_par_;
      rand_sigma_perp_ = that.rand_sigma_perp_;
      rand_sigma_rot_ = that.rand_sigma_rot_;
      v_poly_ = that.v_poly_;
      v_depoly_ = that.v_depoly_;
      p_s2g_ = that.p_s2g_;
      p_s2p_ = that.p_s2p_;
      p_p2s_ = that.p_p2s_;
      p_p2g_ = that.p_p2g_;
      p_g2s_ = that.p_g2s_;
      p_g2p_ = that.p_g2p_;
      tip_force_ = that.tip_force_;
      std::copy(that.body_frame_, that.body_frame_+6, body_frame_);
      poly_state_ = that.poly_state_;
      return *this;
    } 
    virtual void Init();
    virtual void Integrate();
    virtual double const * const GetDrTot();
    virtual void Draw(std::vector<graph_struct*> * graph_array);
    virtual void UpdatePositionMP();
    virtual void Dump();
};

class BrRodSpecies : public Species<BrRod> {
  protected:
    //void InitPotentials(system_parameters *params);
    double max_length_;
  public:
    BrRodSpecies(int n_members, system_parameters *params, 
        space_struct *space, long seed) 
      : Species(n_members, params, space, seed) {
      SetSID(SID::br_rod);
      //InitPotentials(params);
      max_length_ = params_->max_rod_length;
    }
    ~BrRodSpecies() {}
    BrRodSpecies(const BrRodSpecies& that) : Species(that) {}
    Species& operator=(Species const& that) {
      SpeciesBase::operator=(that);
      return *this;
    }
    void Init() {
      Species::Init();
    }
    double const GetMaxLength() {return max_length_;}

};

#endif // _SIMCORE_BR_ROD_H_
