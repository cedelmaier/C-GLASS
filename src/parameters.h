#ifndef _SIMCORE_PARAMETERS_H_
#define _SIMCORE_PARAMETERS_H_

class species_parameters;

class species_parameters {
  public:
    int checkpoint_flag = 0;
    int num = 0;
    std::string insertion_type = "random";
    int overlap = 0;
    std::string draw_type = "orientation";
    double color = 0;
    int posit_flag = 0;
    int spec_flag = 0;
    int n_posit = 100;
    int n_spec = 100;
    int n_checkpoint = 10000;
};

class filament_parameters : public species_parameters {
  public:
    double f_shrink_to_pause = 0.0;
    double diameter = 1;
    double length = 40;
    double persistence_length = 400;
    double max_length = 500;
    double min_length = 1;
    double max_bond_length = 3;
    double min_bond_length = 1.5;
    int spiral_flag = 0;
    double driving_factor = 0;
    double friction_ratio = 2;
    int dynamic_instability_flag = 0;
    int force_induced_catastrophe_flag = 0;
    double f_shrink_to_grow = 0.017;
    double f_pause_to_grow = 0.0;
    double f_pause_to_shrink = 0.0;
    double f_grow_to_pause = 0.0;
    double f_grow_to_shrink = 0.00554;
    int metric_forces = 1;
    double v_poly = 0.44;
    double v_depoly = 0.793;
    int theta_analysis = 0;
    int lp_analysis = 0;
};

class br_bead_parameters : public species_parameters {
  public:
    double diameter = 1;
    double driving_factor = 0;
    double packing_fraction = -1;
};

class hard_rod_parameters : public species_parameters {
  public:
    double diameter = 1;
    double length = 40;
    double min_length = 3;
    double max_length = 300;
    double max_bond_length = 5;
    double driving_factor = 0;
};

class md_bead_parameters : public species_parameters {
  public:
    double diameter = 1;
    double mass = 1;
    double driving_factor = 0;
};

class centrosome_parameters : public species_parameters {
  public:
    double diameter = 10;
    int num_filaments_min = 0;
    int num_filaments_max = 10;
    int fixed_spacing = 0;
    int alignment_potential = 0;
    double k_spring = 0;
    double k_align = 0;
    double spring_length = 0;
};

class system_parameters {
  public:
    long seed = 7859459105545;
    int n_runs = 1;
    int n_random = 1;
    std::string run_name = "sc";
    int n_dim = 3;
    int load_checkpoint = 0;
    int n_periodic = 0;
    int boundary = 0;
    double system_radius = 100;
    long n_steps = 1000000;
    double delta = 0.001;
    double cell_length = 10;
    int n_update_cells = 10;
    int graph_flag = 0;
    int n_graph = 1000;
    double graph_diameter = 0;
    int graph_background = 1;
    int draw_boundary = 1;
    std::string insertion_type = "species";
    int movie_flag = 0;
    std::string movie_directory = "frames";
    int time_flag = 0;
    int species_insertion_failure_threshold = 10000;
    int uniform_crystal = 0;
    double bud_height = 680;
    double bud_radius = 300;
    double lj_epsilon = 1;
    double wca_eps = 1;
    double wca_sig = 1;
    double ss_a = 1;
    double ss_rs = 1.5;
    double ss_eps = 1;
    double f_cutoff = 100;
    int max_overlap = 100000;
    int constant_pressure = 0;
    int constant_volume = 0;
    double target_pressure = 0;
    double target_radius = 100;
    int pressure_time = 100;
    double compressibility = 1;
    int stoch_flag = 1;
    int thermo_flag = 0;
    int n_thermo = 1000;
    int interaction_flag = 1;
    int n_steps_equil = 0;
    species_parameters species;
    filament_parameters filament;
    br_bead_parameters br_bead;
    hard_rod_parameters hard_rod;
    md_bead_parameters md_bead;
    centrosome_parameters centrosome;
};

#endif // _SIMCORE_PARAMETERS_H_