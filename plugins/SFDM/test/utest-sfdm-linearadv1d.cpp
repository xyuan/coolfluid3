// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Test module for cf3::SFDM"

#define run_solver 1

#include <boost/test/unit_test.hpp>
#include <boost/assign/list_of.hpp>
#include "common/Log.hpp"
#include "common/Core.hpp"
#include "common/Environment.hpp"
#include "common/OSystem.hpp"
#include "common/OSystemLayer.hpp"
#include "common/OptionList.hpp"
#include "common/List.hpp"
#include "common/Link.hpp"

#include "common/PE/Comm.hpp"

#include "math/Consts.hpp"
#include "math/VariablesDescriptor.hpp"

#include "solver/CModel.hpp"
#include "solver/Tags.hpp"

#include "physics/PhysModel.hpp"
#include "physics/Variables.hpp"

#include "mesh/Domain.hpp"
#include "mesh/SpaceFields.hpp"
#include "mesh/Field.hpp"
#include "mesh/FieldManager.hpp"
#include "mesh/SimpleMeshGenerator.hpp"
#include "mesh/MeshTransformer.hpp"
#include "mesh/Region.hpp"
#include "mesh/LinearInterpolator.hpp"

#include "SFDM/SFDSolver.hpp"
#include "SFDM/Term.hpp"
#include "SFDM/Tags.hpp"

#include "Tools/Gnuplot/Gnuplot.hpp"

//#include "mesh/Mesh.hpp"
//#include "mesh/CField.hpp"
//#include "mesh/Entities.hpp"
//#include "mesh/ElementType.hpp"
//#include "mesh/MeshWriter.hpp"
//#include "mesh/Domain.hpp"
//#include "mesh/actions/InitFieldFunction.hpp"
//#include "mesh/actions/CreateSpaceP0.hpp"
//#include "solver/CModelUnsteady.hpp"
//#include "solver/CSolver.hpp"
//#include "solver/CPhysicalModel.hpp"
//#include "mesh/actions/BuildFaces.hpp"
//#include "mesh/actions/BuildVolume.hpp"
//#include "mesh/actions/CreateSpaceP0.hpp"
//#include "SFDM/CreateSpace.hpp"

using namespace boost::assign;
using namespace cf3;
using namespace cf3::math;
using namespace cf3::common;
using namespace cf3::common::PE;
using namespace cf3::mesh;
using namespace cf3::physics;
using namespace cf3::solver;
using namespace cf3::SFDM;

std::map<Real,Real> xy(const Field& field)
{
  std::map<Real,Real> map;
  for (Uint i=0; i<field.size(); ++i)
    map[field.coordinates()[i][0]] = field[i][0];
  return map;
}

struct SFDM_MPITests_Fixture
{
  /// common setup for each test case
  SFDM_MPITests_Fixture()
  {
    m_argc = boost::unit_test::framework::master_test_suite().argc;
    m_argv = boost::unit_test::framework::master_test_suite().argv;
  }

  /// common tear-down for each test case
  ~SFDM_MPITests_Fixture()
  {
  }
  /// possibly common functions used on the tests below


  /// common values accessed by all tests goes here
  int    m_argc;
  char** m_argv;

};

////////////////////////////////////////////////////////////////////////////////

BOOST_FIXTURE_TEST_SUITE( SFDM_MPITests_TestSuite, SFDM_MPITests_Fixture )

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( init_mpi )
{
  PE::Comm::instance().init(m_argc,m_argv);
  Core::instance().environment().options().configure_option("log_level", (Uint)INFO);
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( solver1d_test )
{

  //////////////////////////////////////////////////////////////////////////////
  Uint dim=1;

  CModel& model   = *Core::instance().root().create_component<CModel>("model");
  model.setup("cf3.SFDM.SFDSolver","cf3.physics.Scalar.Scalar1D");
  PhysModel& physics = model.physics();
  SFDSolver& solver  = *model.solver().handle<SFDSolver>();
  Domain&   domain  = model.domain();


  Uint DOF = 10;
  Uint order = 3;

  Uint res = 20;//DOF/order;

  Uint sol_order = order;
  Uint time_order = 4;

  physics.options().configure_option("v",1.);

  //////////////////////////////////////////////////////////////////////////////
  // create and configure mesh

  // Create a 2D rectangular mesh
  Mesh& mesh = *domain.create_component<Mesh>("mesh");


  std::vector<Uint> nb_cells = list_of( res  );
  std::vector<Real> lengths  = list_of( 10.  );
  std::vector<Real> offsets  = list_of( 0.  );

  SimpleMeshGenerator& generate_mesh = *domain.create_component<SimpleMeshGenerator>("generate_mesh");
  generate_mesh.options().configure_option("mesh",mesh.uri());
  generate_mesh.options().configure_option("nb_cells",nb_cells);
  generate_mesh.options().configure_option("lengths",lengths);
  generate_mesh.options().configure_option("offsets",offsets);
  generate_mesh.options().configure_option("bdry",false);
  generate_mesh.execute();
  build_component_abstract_type<MeshTransformer>("cf3.mesh.actions.LoadBalance","load_balance")->transform(mesh);
  solver.options().configure_option(SFDM::Tags::mesh(),mesh.handle<Mesh>());

  //////////////////////////////////////////////////////////////////////////////
  // Prepare the mesh

  solver.options().configure_option(SFDM::Tags::solution_vars(),std::string("cf3.physics.Scalar.LinearAdv1D"));
  solver.options().configure_option(SFDM::Tags::solution_order(),sol_order);
  solver.iterative_solver().options().configure_option("rk_order",time_order);
  solver.prepare_mesh().execute();

  //////////////////////////////////////////////////////////////////////////////
  // Configure simulation


  // Initial condition
  solver::Action& init_gauss = solver.initial_conditions().create_initial_condition("gaussian");
  std::vector<std::string> functions;
  // Gaussian wave
  functions.push_back("sigma:="+to_str(lengths[XX]/20.)+";mu:="+to_str(lengths[XX]/2.)+";exp(-(x-mu)^2/(2*sigma^2))");
  init_gauss.options().configure_option("functions",functions);
  solver.initial_conditions().execute();

  Field& solution_field = *Handle<Field>( follow_link( solver.field_manager().get_child(SFDM::Tags::solution()) ) );
  solution_field.field_group().create_coordinates();

  // Discretization
  solver.domain_discretization().create_term("cf3.SFDM.Convection","convection",std::vector<URI>(1,mesh.topology().uri()));

//  // Boundary condition
//  std::vector<URI> bc_regions;
//  bc_regions.push_back(mesh.topology().uri()/"xneg");
//  bc_regions.push_back(mesh.topology().uri()/"xpos");
//  Term& dirichlet = solver.domain_discretization().create_term("cf3.SFDM.BCDirichlet","dirichlet",bc_regions);
//  std::vector<std::string> dirichlet_functions;
//  dirichlet_functions.push_back("0.1");
//  dirichlet.configure_option("functions",dirichlet_functions);


  std::vector<Real> cfl(5);
  cfl[1] = 1.;
  cfl[2] = 0.5;
  cfl[3] = 0.3;
  cfl[4] = 0.2254;

  Real cfl_matteo = 1./(2.2*(sol_order-1.)+1.);

  // Time stepping
  solver.time_stepping().time().options().configure_option("time_step",100.);
  solver.time_stepping().time().options().configure_option("end_time", 2.); // instead of 0.3
  solver.time_stepping().configure_option_recursively("cfl" , cfl_matteo );
  solver.time_stepping().configure_option_recursively("milestone_dt" , 100.);

  //////////////////////////////////////////////////////////////////////////////
  // Run simulation

  Field& residual_field = *follow_link(solver.field_manager().get_child(SFDM::Tags::residual()))->handle<Field>();
  Field& wave_speed_field = *follow_link(solver.field_manager().get_child(SFDM::Tags::wave_speed()))->handle<Field>();


#ifdef GNUPLOT_FOUND
  Gnuplot gp(std::string(GNUPLOT_COMMAND));
  gp << "set terminal png\n";
  gp << "set output 'linearadv1d_P"<<Comm::instance().rank()<<".png'\n";
  gp << "set yrange [-1.2:1.2]\n";
  gp << "set grid\n";
  gp << "set xlabel 'x'\n";
  gp << "set ylabel 'U'\n";
  gp << "set title 'Rank "<<PE::Comm::instance().rank()<<" , P"<<sol_order-1<<"  RK"<<time_order<<"  DOF="<<solution_field.size()<<"   CFL="<<1./(2.2*(sol_order-1.)+1.)<<"'\n";
  gp << "plot ";
  gp << "'-' with points title 'initial solution'"    << ", ";
  gp << "'-' with points title 'final solution'"      << ", ";
  gp << "'-' with points title 'residual'"          << ", ";
  gp << "'-' with points title 'wave_speed'"            << "\n";
  gp.send( solution_field.coordinates().array() , solution_field.array() );
#endif

  model.simulate();

#ifdef GNUPLOT_FOUND
  gp.send( solution_field.coordinates().array() , solution_field.array() );
  gp.send( residual_field.coordinates().array() , residual_field.array() );
  gp.send( wave_speed_field.coordinates().array() , wave_speed_field.array() );
#endif

  CFinfo << "memory: " << OSystem::instance().layer()->memory_usage_str() << CFendl;

  //////////////////////////////////////////////////////////////////////////////
  // Output

  std::vector<URI> fields;
  Field& rank = solution_field.field_group().create_field("rank");
  Field& rank_sync = solution_field.field_group().create_field("rank_sync");
  for (Uint r=0; r<rank.size(); ++r)
  {
    rank[r][0] = rank.rank()[r];
    rank_sync[r][0] = PE::Comm::instance().rank();
  }
  rank_sync.parallelize();
  rank_sync.synchronize();

  fields.push_back(solution_field.uri());
  fields.push_back(solution_field.field_group().field("residual").uri());
  fields.push_back(solution_field.field_group().field("solution_backup").uri());
  mesh.write_mesh("linearadv1d.plt",fields);

  RealVector max( solution_field.row_size() ); max.setZero();
  RealVector min( solution_field.row_size() ); min.setZero();
  for (Uint i=0; i<solution_field.size(); ++i)
  {
    for (Uint j=0; j<solution_field.row_size(); ++j)
    {
      max[j] = std::max(max[j],solution_field[i][j]);
      min[j] = std::min(min[j],solution_field[i][j]);
    }
  }
  std::cout << "solution_field.max = " << max.transpose() << std::endl;
  std::cout << "solution_field.min = " << min.transpose() << std::endl;
}

#if 1
BOOST_AUTO_TEST_CASE( solver2d_test )
{
  //////////////////////////////////////////////////////////////////////////////
  // create and configure SFD - Linear advection 2D model
  Uint dim=1;

  CModel& model   = *Core::instance().root().create_component<CModel>("model2d");
  model.setup("cf3.SFDM.SFDSolver","cf3.physics.Scalar.Scalar2D");
  PhysModel& physics = model.physics();
  SFDSolver& solver  = *model.solver().handle<SFDSolver>();
  Domain&   domain  = model.domain();

//  physics.configure_option("v",1.);

  //////////////////////////////////////////////////////////////////////////////
  // create and configure mesh

  // Create a 2D rectangular mesh
  Mesh& mesh = *domain.create_component<Mesh>("mesh");

  Uint DOF = 5;
  Uint order = 2;

  Uint res = 20;//DOF/order;

  Uint sol_order = order;
  Uint time_order = 2;

  std::vector<Uint> nb_cells = list_of( res )( res );
  std::vector<Real> lengths  = list_of( 10. )( 10. );
  std::vector<Real> offsets  = list_of( 0.  )( 0.  );

  SimpleMeshGenerator& generate_mesh = *domain.create_component<SimpleMeshGenerator>("generate_mesh");
  generate_mesh.options().configure_option("mesh",mesh.uri());
  generate_mesh.options().configure_option("nb_cells",nb_cells);
  generate_mesh.options().configure_option("lengths",lengths);
  generate_mesh.options().configure_option("offsets",offsets);
  generate_mesh.options().configure_option("bdry",false);
  generate_mesh.execute();
  build_component_abstract_type<MeshTransformer>("cf3.mesh.actions.LoadBalance","load_balance")->transform(mesh);
  solver.options().configure_option(SFDM::Tags::mesh(),mesh.handle<Mesh>());

  //////////////////////////////////////////////////////////////////////////////
  // Prepare the mesh

  solver.options().configure_option(SFDM::Tags::solution_vars(),std::string("cf3.physics.Scalar.LinearAdv2D"));
  solver.options().configure_option(SFDM::Tags::solution_order(),sol_order);
  solver.iterative_solver().options().configure_option("rk_order",time_order);
  solver.prepare_mesh().execute();

  //////////////////////////////////////////////////////////////////////////////
  // Configure simulation


  // Initial condition
  solver::Action& init_gauss = solver.initial_conditions().create_initial_condition("gaussian");
  std::vector<std::string> functions;
  // Gaussian wave
  functions.push_back("sigma:="+to_str(lengths[XX]/20.)+";mu:="+to_str(lengths[XX]/2.)+";exp(-((x-mu)^2+(y-mu)^2)/(2*sigma^2))");
  init_gauss.options().configure_option("functions",functions);
  solver.initial_conditions().execute();

  Field& solution_field = *follow_link(solver.field_manager().get_child(SFDM::Tags::solution()))->handle<Field>();
  solution_field.field_group().create_coordinates();

  // Discretization
  solver.domain_discretization().create_term("cf3.SFDM.Convection","convection",std::vector<URI>(1,mesh.topology().uri()));

//  // Boundary condition
//  std::vector<URI> bc_regions;
//  bc_regions.push_back(mesh.topology().uri()/"xneg");
//  bc_regions.push_back(mesh.topology().uri()/"xpos");
//  Term& dirichlet = solver.domain_discretization().create_term("cf3.SFDM.BCDirichlet","dirichlet",bc_regions);
//  std::vector<std::string> dirichlet_functions;
//  dirichlet_functions.push_back("0.1");
//  dirichlet.configure_option("functions",dirichlet_functions);


  std::vector<Real> cfl(5);
  cfl[1] = 1.;
  cfl[2] = 0.5;
  cfl[3] = 0.3;
  cfl[4] = 0.2254;

  Real cfl_1d = 1./(2.2*(sol_order-1.)+1.);
  Real cfl_matteo = 1./(std::pow(2.,1./(sol_order))) * cfl_1d;

  // Time stepping
  solver.time_stepping().time().options().configure_option("time_step",100.);
  solver.time_stepping().time().options().configure_option("end_time" , lengths[XX]/10.); // instead of 0.3
  solver.time_stepping().configure_option_recursively("cfl" , cfl_matteo );
  solver.time_stepping().configure_option_recursively("milestone_dt" , 100.);

  //////////////////////////////////////////////////////////////////////////////
  // Run simulation

  std::vector<URI> fields;
  fields.push_back(solution_field.uri());
  fields.push_back(solution_field.field_group().field("residual").uri());

  mesh.write_mesh("initial2d.msh",fields);


  Field& residual_field = *follow_link(solver.field_manager().get_child(SFDM::Tags::residual()))->handle<Field>();
  Field& wave_speed_field = *follow_link(solver.field_manager().get_child(SFDM::Tags::wave_speed()))->handle<Field>();


//#ifdef GNUPLOT_FOUND
//  Gnuplot gp(std::string(GNUPLOT_COMMAND));
//  gp << "set terminal png\n";
//  gp << "set output 'linearadv1d_P"<<Comm::instance().rank()<<".png'\n";
////  gp << "set yrange [-1.2:1.2]\n";
//  gp << "set grid\n";
//  gp << "set xlabel 'x'\n";
//  gp << "set ylabel 'U'\n";
//  gp << "set title 'Rank "<<PE::Comm::instance().rank()<<" , P"<<sol_order-1<<"  RK"<<time_order<<"  DOF="<<solution_field.size()<<"   CFL="<<1./(2.2*(sol_order-1.)+1.)<<"'\n";
//  gp << "plot ";
//  gp << "'-' with linespoints title 'initial solution'"    << ", ";
//  gp << "'-' with linespoints title 'final solution'"      << ", ";
//  gp << "'-' with linespoints title 'residual'"          << ", ";
//  gp << "'-' with linespoints title 'wave_speed'"            << "\n";
//  gp.send( solution_field.coordinates().array() , solution_field.array() );
//#endif

  CFinfo << "cfl = " << cfl_matteo << CFendl;

  model.simulate();

//#ifdef GNUPLOT_FOUND
//  gp.send( solution_field.coordinates().array() , solution_field.array() );
//  gp.send( residual_field.coordinates().array() , residual_field.array() );
//  gp.send( wave_speed_field.coordinates().array() , wave_speed_field.array() );
//#endif

  CFinfo << "memory: " << OSystem::instance().layer()->memory_usage_str() << CFendl;
  CFinfo << "cfl = " << cfl_matteo << CFendl;
  //////////////////////////////////////////////////////////////////////////////
  // Output

  Field& rank = solution_field.field_group().create_field("rank");
  Field& rank_sync = solution_field.field_group().create_field("rank_sync");
  for (Uint r=0; r<rank.size(); ++r)
  {
    rank[r][0] = rank.rank()[r];
    rank_sync[r][0] = PE::Comm::instance().rank();
  }
  rank_sync.parallelize();
  rank_sync.synchronize();

  fields.push_back(solution_field.field_group().field("wave_speed").uri());

  mesh.write_mesh("linearadv2d.msh",fields);

  RealVector max( solution_field.row_size() ); max.setZero();
  RealVector min( solution_field.row_size() ); min.setZero();
  for (Uint i=0; i<solution_field.size(); ++i)
  {
    for (Uint j=0; j<solution_field.row_size(); ++j)
    {
      max[j] = std::max(max[j],solution_field[i][j]);
      min[j] = std::min(min[j],solution_field[i][j]);
    }
  }
  std::cout << "solution_field.max = " << max.transpose() << std::endl;
  std::cout << "solution_field.min = " << min.transpose() << std::endl;
}

#endif
////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( finalize_mpi )
{
  PE::Comm::instance().finalize();
}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////////////////////////////////////
