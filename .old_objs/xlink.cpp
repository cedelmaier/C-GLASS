#include "xlink.h"

#include <iomanip>

void Xlink::Init() {
  Composite::InsertRandom(length_+diameter_);
  // Give each bead the location of the main position!
  int ihead = -1;
  for (auto i_bead = elements_.begin(); i_bead != elements_.end(); ++i_bead) {
    ihead++;
    i_bead->SetPosition(position_);
    i_bead->SetPrevPosition(position_);
    i_bead->SetDiameter(diameter_);
    i_bead->SetDiffusion();
    i_bead->SetSpace(space_);
    i_bead->UpdatePeriodic();
    i_bead->SetBound(false);
    i_bead->SetHeadID(ihead);
  }

  UpdateOrientation();
}

void Xlink::InitConfigurator(const double* const x, const double diameter) {
  SetPosition(x);
  diameter_ = diameter;
  int ihead = -1;
  for (auto i_bead = elements_.begin(); i_bead != elements_.end(); ++i_bead) {
    ihead++;
    i_bead->SetPosition(position_);
    i_bead->SetPrevPosition(position_);
    i_bead->SetDiameter(diameter_);
    i_bead->SetDiffusion();
    i_bead->SetSpace(space_);
    i_bead->UpdatePeriodic();
    i_bead->SetBound(false);
    i_bead->SetHeadID(ihead);
  }

  UpdateOrientation();
}

void Xlink::UpdateOrientation() {
  double const * const r1=elements_[0].GetPosition();
  double const * const r2=elements_[1].GetPosition();
  double const * const s1=elements_[0].GetScaledPosition();
  double const * const s2=elements_[1].GetScaledPosition();
  length_ = 0;
  double dr[3];
  separation_vector(n_dim_, space_->n_periodic, r1, s1, r2, s2, space_->unit_cell, dr);
  for (int i=0; i<n_dim_; ++i) {
    r_cross_[i] = dr[i];
    length_ += SQR(dr[i]);
  }
  if (length_ <= 0.0)
    length_ = 1.0;
  length_=sqrt(length_);
  for (int i=0; i<n_dim_; ++i) {
    orientation_[i] = (dr[i])/length_;
    position_[i] = r1[i] + 0.5*length_*orientation_[i];
  }
  UpdatePeriodic();
}

void Xlink::BindHeadSingle(int ihead, double crosspos, int rodoid) {
  if (bound_ != unbound) {
    std::cout << "For some reason this xlink isn't unbound...\n";
  }
  auto bindhead = elements_.begin() + ihead;
  if (bindhead->GetBound()) {
    std::cout << "Something funny happened with the binding of an xlink\n";
  }
  if (debug_trace) {
    std::cout << std::setprecision(16) << "[" << GetOID() << "]{" << bindhead->GetOID()
      << "} binding to {" << crosspos << "}[" << rodoid << "]\n";
  }
  bindhead->Attach(rodoid, crosspos);
  bindhead->SetBound(true);
  CheckBoundState();
}

void Xlink::BindHeadDouble(double crosspos0, int rodoid0, double crosspos1, int rodoid1) {
  if (bound_ != unbound) {
    std::cout << "Something has gone wrong in the xlink 2->0 stage bind\n";
  }
  auto head0 = elements_.begin();
  auto head1 = elements_.begin()+1;
  head0->Attach(rodoid0, crosspos0);
  head0->SetBound(true);
  head1->Attach(rodoid1, crosspos1);
  head1->SetBound(true);
  CheckBoundState();
}

void Xlink::CheckBoundState() {
  bound_ = unbound;
  auto head0 = elements_.begin();
  auto head1 = elements_.begin()+1;
  bool bound0 = head0->GetBound();
  bool bound1 = head1->GetBound();
  if (!bound0 && !bound1) {
    head0->SetOverbound(unbound);
    head1->SetOverbound(unbound);
  }
  if (bound0 && !bound1) {
    bound_ = singly;
    head0->SetOverbound(singly);
    head1->SetOverbound(singly);
  }
  if (!bound0 && bound1) {
    bound_ = singly;
    head0->SetOverbound(singly);
    head1->SetOverbound(singly);
  }
  if (bound0 && bound1) {
    bound_ = doubly;
    head0->SetOverbound(doubly);
    head1->SetOverbound(doubly);
  }
}

const double Xlink::GetInternalEnergy() {
  // Override this so that we can calculate it on the fly from the two heads.  We know
  // that the internal energy depends on the xlink heads, so we should be able to just
  // take their two potential energies and add them
  // Each already has 1/2 the total energy, so just sum them
  auto head0 = elements_.begin();
  auto head1 = elements_.begin()+1;
  p_energy_ = head0->GetPotentialEnergy();
  p_energy_ += head1->GetPotentialEnergy();
  return p_energy_;
}

std::pair<bool,bool> Xlink::GetBoundHeads(XlinkHead **freehead, XlinkHead **boundhead) {
  std::pair<bool,bool> nbound;
  auto head0 = elements_.begin();
  auto head1 = elements_.begin()+1;
  if (!head0->GetBound() && !head1->GetBound()) {
    // Neither is bound, just return
    // but do set the freehead, boundhead appropriately
    nbound.first = false;
    nbound.second = false;
    (*freehead) = &(*head0);
    (*boundhead) = &(*head1);
  } else if (head0->GetBound() && !head1->GetBound()) {
    // Head 0 is bound and not head1
    nbound.first = true;
    nbound.second = false;
    (*freehead) = &(*head1);
    (*boundhead) = &(*head0);
  } else if (!head0->GetBound() && head1->GetBound()) {
    nbound.first = false;
    nbound.second = true;
    (*freehead) = &(*head0);
    (*boundhead) = &(*head1);
  } else {
    // Both heads are bound
    // head 0 will be freehead (the first one)
    nbound.first = true;
    nbound.second = true;
    (*freehead) = &(*head0);
    (*boundhead) = &(*head1);
  }
  return nbound;
}

void Xlink::UpdatePositionMP() {
  CheckBoundState();
  Integrate();
}

void Xlink::ApplyInteractions() {
   //We have no internal constraints
  for (auto i_bead = elements_.begin(); i_bead != elements_.end(); ++i_bead) {
    i_bead->ApplyInteractions();
  }
}

void Xlink::Integrate() {
  // Different actions due to being free, singly, or doubly bound
  //switch(bound_) {
  //  case unbound:
  //    DiffuseXlink();
  //    break;
  //}
  /*double pos[3] = {0, 0, 0};
  // the beads know how to update position and periodicity
  for (auto i_bead = elements_.begin(); i_bead != elements_.end(); ++i_bead) {
    i_bead->UpdatePositionMP();
  }*/
  UpdateOrientation();
}

void Xlink::DiffuseXlink() {
  // Diffuse based on the first head
  auto head0 = elements_.begin();
  auto head1 = elements_.begin()+1;
  double oldpos[3];
  std::copy(head0->GetRigidPosition(), head0->GetRigidPosition()+n_dim_, oldpos);
  head0->UpdatePositionMP();
  head0->SetPrevPosition(oldpos);
  head1->SetPosition(head0->GetRigidPosition());
  head1->SetPrevPosition(oldpos);
  head1->AddDr();
  SetPosition(head0->GetRigidPosition());
  SetPrevPosition(oldpos);
  head0->UpdatePeriodic();
  head1->UpdatePeriodic();
  UpdatePeriodic();
}

void Xlink::Draw(std::vector<graph_struct*> * graph_array) {
  // draw half of rod coming from each bead (looks good for periodic boundaries)
  double const * const r1 = elements_[0].GetPosition();
  double const * const r2 = elements_[1].GetPosition();
  double r[3];
  for (int i=0; i<n_dim_; ++i)
    r[i] = r1[i] + 0.25*length_*orientation_[i];
  std::copy(r, r+3, g_.r);
  std::copy(orientation_, orientation_+3, g_.u);
  std::copy(color_, color_+4, g_.color);
  g_.draw_type = draw_type_;
  g_.length = 0.5 * length_;
  g_.diameter = 0.3*diameter_;
  graph_array->push_back(&g_);
  for (int i=0; i<n_dim_; ++i)
    r[i] = r2[i] - 0.25*length_*orientation_[i];
  std::copy(r, r+3, g2_.r);
  std::copy(orientation_, orientation_+3, g2_.u);
  std::copy(color_, color_+4, g2_.color);
  g2_.draw_type = draw_type_;
  g2_.length = 0.5 * length_;
  g2_.diameter = 0.3*diameter_;
  graph_array->push_back(&g2_);
  for (auto i_bead = elements_.begin(); i_bead != elements_.end(); ++i_bead) {
    i_bead->SetColor(color_, draw_type_);
    i_bead->Draw(graph_array);
  }
}

// Update staged position stuff
void Xlink::UpdateStagePosition(const double* const xr0, const double* const ur0, const double lr0, const int atidx0,
                                const double* const xr1, const double* const ur1, const double lr1, const int atidx1) {
  // Figure out what stage we're in, if unbound, don't do anything
  // Head 0 is the xr0 row, head 1 is the other
  // need to figure out before calling this
  switch(bound_) {
    case(unbound):
      // Do nothing
      break;
    case(singly):
      //std::cout << "Attempting UpdateStagePosition singly\n";
      // Figure out which head (and make sure it makes sense)
      // Also, everything is in the 0th row
      UpdateStage1Position(xr0, ur0, lr0, atidx0);
      break;
    case(doubly):
      //std::cout << "Attempting UpdateStagePosition doubly\n";
      UpdateStage2Position(xr0, ur0, lr0, atidx0,
                           xr1, ur1, lr1, atidx1);
      break;
  }
  UpdateOrientation();
}

void Xlink::UpdateStage0Position(const double* const xr) {
  if (xr == nullptr) {
    std::cout << "Passed in null pointers to Xlink::UpdateStage0Position, exiting\n";
    exit(1);
  }
  double rxold[3] = {0.0, 0.0, 0.0};
  double rxnew[3] = {0.0, 0.0, 0.0};
  double rsnew[3] = {0.0, 0.0, 0.0};
  XlinkHead *head0, *head1;
  auto isbound = GetBoundHeads(&head0, &head1);
  std::copy(head0->GetPosition(), head0->GetPosition()+space_->n_dim, rxold);
  std::copy(xr, xr+space_->n_dim, rxnew);
  // Bound rxnew inside the PBCs (if they exist)
  periodic_boundary_conditions(space_->n_periodic, space_->unit_cell, space_->unit_cell_inv, rxnew, rsnew);
  // Set head0
  head0->SetPrevPosition(rxold);
  head0->SetPosition(rxnew);
  head0->UpdatePeriodic();
  head0->AddDr();
  // Head1
  head1->SetPrevPosition(rxold);
  head1->SetPosition(rxnew);
  head1->UpdatePeriodic();
  head1->AddDr();
  // Main xlink
  SetPrevPosition(rxold);
  SetPosition(rxnew);
  UpdatePeriodic();
}

void Xlink::UpdateStage1Position(const double* const xr, const double* const ur, const double lr, const int atidx) {
  //std::cout << "UpdateStage1Position\n";
  if (xr == nullptr || ur == nullptr) {
    std::cout << "Passed in null pointers to Xlink::UpdateStage1Position, exiting\n";
    exit(1);
  }
  double rxold[3] = {0.0, 0.0, 0.0};
  double rxnew[3] = {0.0, 0.0, 0.0};
  double rsnew[3] = {0.0, 0.0, 0.0};
  XlinkHead *freehead, *boundhead;
  auto isbound = GetBoundHeads(&freehead, &boundhead);
  std::copy(boundhead->GetPosition(), boundhead->GetPosition()+space_->n_dim, rxold);
  //std::cout << std::setprecision(16) << "rxold: (" << rxold[0] << ", " << rxold[1] << ", " << rxold[2] << ")\n";
  auto crosspos = boundhead->GetAttach().second;
  // Check to make sure we have the same OID as the atidx
  if (atidx != boundhead->GetAttach().first) {
    std::cout << "Xlink attached wrong, given: " << atidx << ", but think attached to: " << boundhead->GetAttach().first << std::endl;
    exit(1);
  }
  //std::cout << std::setprecision(16) << "cross position: " << crosspos << std::endl;
  for (int i = 0; i < space_->n_dim; ++i) {
    rxnew[i] = xr[i] - 0.5 * ur[i] * lr + crosspos * ur[i];
  }
  // Bound rxnew inside the PBCs (if they exist)
  periodic_boundary_conditions(space_->n_periodic, space_->unit_cell, space_->unit_cell_inv, rxnew, rsnew);
  //std::cout << std::setprecision(16) << "rxnew: (" << rxnew[0] << ", " << rxnew[1] << ", " << rxnew[2] << ")\n";
  // Set the bound head
  boundhead->SetPrevPosition(rxold);
  boundhead->SetPosition(rxnew);
  boundhead->UpdatePeriodic();
  boundhead->AddDr();
  // Set the free head
  freehead->SetPrevPosition(rxold);
  freehead->SetPosition(rxnew);
  freehead->UpdatePeriodic();
  freehead->AddDr();
  // Set the xlink
  SetPrevPosition(rxold);
  SetPosition(rxnew);
  UpdatePeriodic();

  if (debug_trace) {
    std::cout << std::setprecision(16) << "[" << boundhead->GetOID() << "] single attached [" << boundhead->GetAttach().first << "], ("
      << boundhead->GetPrevPosition()[0] << ", " << boundhead->GetPrevPosition()[1] << ", " << boundhead->GetPrevPosition()[2] << ") -> setting {"
      << crosspos << "}("
      << boundhead->GetPosition()[0] << ", " << boundhead->GetPosition()[1] << ", " << boundhead->GetPosition()[2] << ")\n";
  }
}

void Xlink::UpdateStage2Position(const double* const xr0, const double* const ur0, const double lr0, const int atidx0,
                                 const double* const xr1, const double* const ur1, const double lr1, const int atidx1) {
  //std::cout << "UpdateStage2Position\n";

  XlinkHead *head0 = &(*(elements_.begin()));
  XlinkHead *head1 = &(*(elements_.begin()+1));

  double rxold[3] = {0.0, 0.0, 0.0};
  double rxnew[3] = {0.0, 0.0, 0.0};
  double rsnew[3] = {0.0, 0.0, 0.0};

  // Do head 0 first, do each explicitly
  std::copy(head0->GetPosition(), head0->GetPosition()+space_->n_dim, rxold);
  double crosspos = head0->GetAttach().second;
  if (head0->GetAttach().first != atidx0) {
    std::cout << "Xlink head0 attached wrong, given: " << atidx0 << ", but think attached to: " << head0->GetAttach().first << std::endl;
    exit(1);
  }
  for (int i = 0; i < space_->n_dim; ++i) {
    rxnew[i] = xr0[i] - 0.5 * ur0[i] * lr0 + crosspos * ur0[i];
  }
  // Bound rxnew inside the PBCs (if they exist)
  periodic_boundary_conditions(space_->n_periodic, space_->unit_cell, space_->unit_cell_inv, rxnew, rsnew);
  // Set head0
  head0->SetPrevPosition(rxold);
  head0->SetPosition(rxnew);
  head0->UpdatePeriodic();
  head0->AddDr();

  // Do head 1 last
  std::copy(head1->GetPosition(), head1->GetPosition()+space_->n_dim, rxold);
  crosspos = head1->GetAttach().second;
  if (head1->GetAttach().first != atidx1) {
    std::cout << "Xlink head0 attached wrong, given: " << atidx1 << ", but think attached to: " << head1->GetAttach().first << std::endl;
    exit(1);
  }
  for (int i = 0; i < space_->n_dim; ++i) {
    rxnew[i] = xr1[i] - 0.5 * ur1[i] * lr1 + crosspos * ur1[i];
  }
  // Bound rxnew inside the PBCs (if they exist)
  periodic_boundary_conditions(space_->n_periodic, space_->unit_cell, space_->unit_cell_inv, rxnew, rsnew);
  // Set head0
  head1->SetPrevPosition(rxold);
  head1->SetPosition(rxnew);
  head1->UpdatePeriodic();
  head1->AddDr();

  // Update the xlink position to lie between the two
  double xposold[3] = {0.0, 0.0, 0.0};
  std::copy(GetPosition(), GetPosition()+space_->n_dim, xposold);
  double xpos[3] = {0.0, 0.0, 0.0};
  for (int i = 0; i < space_->n_dim; ++i) {
    xpos[i] = 0.5 * (head0->GetPosition()[i] + head1->GetPosition()[i]);
  }
  SetPrevPosition(xposold);
  SetPosition(xpos);
  UpdatePeriodic();

  if (debug_trace) {
    std::cout << std::setprecision(16) << "[" << head0->GetOID() << ", " << head1->GetOID() << "] double attached [" << head0->GetAttach().first
      << ", " << head1->GetAttach().first << "] ("
      << head0->GetPrevPosition()[0] << ", " << head0->GetPrevPosition()[1] << ", " << head0->GetPrevPosition()[2] << "),("
      << head1->GetPrevPosition()[0] << ", " << head1->GetPrevPosition()[1] << ", " << head1->GetPrevPosition()[2] << ") -> setting {"
      << head0->GetAttach().second << ", " << head1->GetAttach().second << "}("
      << head0->GetPosition()[0] << ", " << head0->GetPosition()[1] << ", " << head0->GetPosition()[2] << "),("
      << head1->GetPosition()[0] << ", " << head1->GetPosition()[1] << ", " << head1->GetPosition()[2] << ")\n";
  }
}

// Species specifics
void XlinkSpecies::Configurator() {
  char *filename = params_->config_file;
  std::cout << "Xlink species\n";

  YAML::Node node = YAML::LoadFile(filename);

  std::cout << " Generic Properties:\n";
  std::string insertion_type;
  insertion_type = node["xlink"]["properties"]["insertion_type"].as<std::string>();
  std::cout << "   insertion type: " << insertion_type << std::endl;
  bool can_overlap = node["xlink"]["properties"]["overlap"].as<bool>();
  std::cout << "   can overlap:    " << (can_overlap ? "true" : "false") << std::endl;

  // Coloring
  double color[4] = {1.0, 0.0, 0.0, 1.0};
  int draw_type = 0; // default to orientation
  if (node["xlink"]["properties"]["color"]) {
    for (int i = 0; i < 4; ++i) {
      color[i] = node["xlink"]["properties"]["color"][i].as<double>();
    }
    std::cout << "   color: [" << color[0] << ", " << color[1] << ", " << color[2] << ", "
      << color[3] << "]\n";
  }
  if (node["xlink"]["properties"]["draw_type"]) {
    std::string draw_type_s = node["xlink"]["properties"]["draw_type"].as<std::string>();
    std::cout << "   draw_type: " << draw_type_s << std::endl;
    if (draw_type_s.compare("flat") == 0) {
      draw_type = 0;
    }
  }

  if (insertion_type.compare("xyz") == 0) {
    if (!can_overlap) {
      std::cout << "Warning, location insertion overrides overlap\n";
      can_overlap = true;
    }
    int nxlinks = (int)node["xlink"]["xit"].size();
    std::cout << "   nxlinks: " << nxlinks << std::endl;
    params_->n_xlink = nxlinks;
    for (int ix = 0; ix < nxlinks; ++ix) {
      double x[3] = {0.0, 0.0, 0.0};
      double diameter = 0.0;
      x[0] = node["xlink"]["xit"][ix]["x"][0].as<double>();
      x[1] = node["xlink"]["xit"][ix]["x"][1].as<double>();
      x[2] = node["xlink"]["xit"][ix]["x"][2].as<double>();
      std::cout << "   x(" << x[0] << ", " << x[1] << ", " << x[2] << ")\n";
      diameter = node["xlink"]["xit"][ix]["diameter"].as<double>();
      std::cout << "   diameter[" << diameter << "]\n";

      Xlink *member = new Xlink(params_, space_, gsl_rng_get(rng_.r), GetSID());
      member->InitConfigurator(x, diameter);
      member->SetColor(color, draw_type);
      member->Dump();
      members_.push_back(member);
    }
    n_members_ = nxlinks;
  } else if (insertion_type.compare("random") == 0) {
    int nxlinks     = node["xlink"]["xit"]["num"].as<int>();
    double diameter = node["xlink"]["xit"]["diameter"].as<double>();

    std::cout << std::setw(25) << std::left << "   n xlinks:" << std::setw(10)
      << std::left << nxlinks << std::endl;
    std::cout << std::setw(25) << std::left << "   diameter:" << std::setw(10)
      << std::left << diameter << std::endl;

    params_->n_xlink = nxlinks;
    params_->xlink_diameter = diameter;

    if (!can_overlap) {
      error_exit("ERROR, xlinks always allowed to overlap\n");
    }
    for (int i = 0; i < nxlinks; ++i) {
      Xlink* member = new Xlink(params_, space_, gsl_rng_get(rng_.r), GetSID());
      member->Init();
      member->SetColor(color, draw_type);
      members_.push_back(member);
    }
    n_members_ = nxlinks;
  } else {
    std::cout << "Insertion type " << insertion_type << " not implemented yet\n";
    exit(1);
  }
}

void XlinkSpecies::CreateTestXlink(Xlink **mxit,
                                   int ndim,
                                   std::vector<Simple*>* simples,
                                   std::unordered_map<int, int>* oid_position_map,
                                   const std::string &filename,
                                   const std::string &modulename,
                                   const std::string &unitname,
                                   const std::string &xname,
                                   int itest) {
  YAML::Node node = YAML::LoadFile(filename);
  Xlink *xit = *mxit;
  double xx[3] = {0.0, 0.0, 0.0};
  std::ostringstream posname;
  posname << "x_" << xname;
  std::cout << "posname: " << posname.str() << std::endl;
  for (int idim = 0; idim < ndim; ++idim) {
    xx[idim] = node[modulename][unitname]["test"][itest][posname.str()][idim].as<double>();
  }
  xit->InitConfigurator(xx, 1.0);

  XlinkHead *head0, *head1;
  auto isbound = xit->GetBoundHeads(&head0, &head1);

  // Read the RNG binary file in
  std::string rngfile = node[modulename][unitname]["test"][itest]["rngfile"].as<std::string>();
  xit->SetRNGState(rngfile);

  double nexp_0_1 = 0.0;
  if (node[modulename][unitname]["test"][itest]["nexp_0_1"]) {
    nexp_0_1 = node[modulename][unitname]["test"][itest]["nexp_0_1"].as<double>();
  }
  xit->SetNExp_0_1(nexp_0_1);
  head0->SetNExp_0_1(0.5*nexp_0_1);
  head1->SetNExp_0_1(0.5*nexp_0_1);

  xit->Dump();
  xit->DumpKMC();

  // simples and oid stuff
  std::vector<Simple*> sim_vec = xit->GetSimples();
  for (int i = 0; i < sim_vec.size(); ++i) {
    simples->push_back(sim_vec[i]);
    (*oid_position_map)[sim_vec[i]->GetOID()] = simples->size() -1;
  }
}

void XlinkSpecies::CreateTestXlink(Xlink **mxit,
                                   int ndim,
                                   std::vector<Simple*>* simples,
                                   std::unordered_map<int, int>* oid_position_map,
                                   const std::string &filename,
                                   const std::string &modulename,
                                   const std::string &unitname,
                                   const std::string &xname,
                                   int itest,
                                   int attachoid) {
  YAML::Node node = YAML::LoadFile(filename);
  Xlink *xit = *mxit;
  double xx[3] = {0.0, 0.0, 0.0};
  xit->InitConfigurator(xx, 1.0);
  //std::cout << "Xlink base name: " << xname << std::endl;
  std::ostringstream headname;
  headname << "head_" << xname;
  //std::cout << "head name: " << headname.str() << std::endl;
  int head = node[modulename][unitname.c_str()]["test"][itest][headname.str().c_str()].as<int>();
  std::ostringstream crossposname;
  crossposname << "crosspos_" << xname;
  //std::cout << "crosspos name: " << crossposname.str() << std::endl;
  double crosspos = node[modulename][unitname.c_str()]["test"][itest][crossposname.str().c_str()].as<double>();

  xit->BindHeadSingle(head, crosspos, attachoid);
  //xit->Dump();
  //xit->DumpKMC();

  // simples and oid stuff
  std::vector<Simple*> sim_vec = xit->GetSimples();
  for (int i = 0; i < sim_vec.size(); ++i) {
    simples->push_back(sim_vec[i]);
    (*oid_position_map)[sim_vec[i]->GetOID()] = simples->size() -1;
  }

  // Print to make sure working
  /*std::cout << "simples: \n";
  for (int i = 0; i < simples->size(); ++i) {
    std::cout << "[" << i << "] -> OID " << (*simples)[i]->GetOID();
    std::cout << " <--> " << (*oid_position_map)[(*simples)[i]->GetOID()] << std::endl;
  }*/
}

void XlinkSpecies::CreateTestXlink(Xlink **mxit,
                                   int ndim,
                                   std::vector<Simple*>* simples,
                                   std::unordered_map<int, int>* oid_position_map,
                                   const std::string &filename,
                                   const std::string &modulename,
                                   const std::string &unitname,
                                   const std::string &xname,
                                   int itest,
                                   int attachoid0,
                                   int attachoid1) {
  YAML::Node node = YAML::LoadFile(filename);
  Xlink *xit = *mxit;
  double xx[3] = {0.0, 0.0, 0.0};
  xit->InitConfigurator(xx, 1.0);
  std::ostringstream crossposname0;
  crossposname0 << "crosspos_" << xname << "0";
  std::ostringstream crossposname1;
  crossposname1 << "crosspos_" << xname << "1";
  double cpos0 = node[modulename][unitname.c_str()]["test"][itest][crossposname0.str().c_str()].as<double>();
  double cpos1 = node[modulename][unitname.c_str()]["test"][itest][crossposname1.str().c_str()].as<double>();

  xit->BindHeadDouble(cpos0, attachoid0, cpos1, attachoid1);
  //xit->Dump();
  //xit->DumpKMC();

  // simples and oid stuff
  std::vector<Simple*> sim_vec = xit->GetSimples();
  for (int i = 0; i < sim_vec.size(); ++i) {
    simples->push_back(sim_vec[i]);
    (*oid_position_map)[sim_vec[i]->GetOID()] = simples->size() -1;
  }
}