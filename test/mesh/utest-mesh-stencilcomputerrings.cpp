// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Tests mesh octtree"

#include <boost/test/unit_test.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>

#include "common/Core.hpp"
#include "common/Foreach.hpp"
#include "common/Log.hpp"
#include "common/OptionList.hpp"

#include "common/FindComponents.hpp"
#include "common/Link.hpp"

#include "mesh/Mesh.hpp"
#include "mesh/Region.hpp"
#include "mesh/Elements.hpp"
#include "common/Table.hpp"
#include "mesh/SpaceFields.hpp"
#include "mesh/MeshGenerator.hpp"
#include "mesh/StencilComputerRings.hpp"

using namespace boost;
using namespace boost::assign;
using namespace cf3;
using namespace cf3::mesh;
using namespace cf3::common;

////////////////////////////////////////////////////////////////////////////////

struct StencilComputerRings_Fixture
{
  /// common setup for each test case
  StencilComputerRings_Fixture()
  {
     // uncomment if you want to use arguments to the test executable
     //int*    argc = &boost::unit_test::framework::master_test_suite().argc;
     //char*** argv = &boost::unit_test::framework::master_test_suite().argv;
  }

  /// common tear-down for each test case
  ~StencilComputerRings_Fixture()
  {
  }

  /// possibly common functions used on the tests below


  /// common values accessed by all tests goes here

};

////////////////////////////////////////////////////////////////////////////////

BOOST_FIXTURE_TEST_SUITE( StencilComputerRings_TestSuite, StencilComputerRings_Fixture )

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( StencilComputerRings_creation )
{

  // create meshreader
  boost::shared_ptr< MeshGenerator > mesh_generator = build_component_abstract_type<MeshGenerator>("cf3.mesh.SimpleMeshGenerator","mesh_generator");
  Core::instance().root().add_component(mesh_generator);
  mesh_generator->options().configure_option("mesh",Core::instance().root().uri()/"mesh");
  mesh_generator->options().configure_option("lengths",std::vector<Real>(2,10.));
  mesh_generator->options().configure_option("nb_cells",std::vector<Uint>(2,5));
  Mesh& mesh = mesh_generator->generate();

  Handle<StencilComputerRings> stencil_computer = Core::instance().root().create_component<StencilComputerRings>("stencilcomputer");
  stencil_computer->options().configure_option("mesh", mesh.handle<Mesh>() );

  std::vector<Uint> stencil;
//  stencil_computer->options().configure_option("stencil_size", 10u );
  stencil_computer->options().configure_option("nb_rings", 1u );
  stencil_computer->compute_stencil(7, stencil);
  BOOST_CHECK_EQUAL(stencil.size(), 9u);

  stencil_computer->options().configure_option("nb_rings", 2u );
  stencil_computer->compute_stencil(7, stencil);
  BOOST_CHECK_EQUAL(stencil.size(), 20u);

  stencil_computer->options().configure_option("nb_rings", 3u );
  stencil_computer->compute_stencil(7, stencil);
  BOOST_CHECK_EQUAL(stencil.size(), 25u);

}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////////////////////////////////////

