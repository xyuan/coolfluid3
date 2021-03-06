// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "common/Builder.hpp"
#include "common/OptionT.hpp"
#include "common/OptionURI.hpp"
#include "common/Signal.hpp"

#include "common/XML/SignalOptions.hpp"

#include "solver/CPlotXY.hpp"
#include "solver/LibSolver.hpp"

#include "solver/CPlotter.hpp"

////////////////////////////////////////////////////////////////////////////////

using namespace cf3::common;
using namespace cf3::common::XML;

namespace cf3 {
namespace solver {

////////////////////////////////////////////////////////////////////////////////

ComponentBuilder < CPlotter, Component, LibSolver > CPlotter_Builder;

////////////////////////////////////////////////////////////////////////////////

CPlotter::CPlotter(const std::string & name) :
   Component(name)
{
  // signals
  regist_signal("create_xyplot")
      .description("Creates an XY-Plot")
      .pretty_name("New XY-Plot")
      .connect( boost::bind(&CPlotter::signal_create_xyplot, this, _1) )
      .signature( boost::bind(&CPlotter::signature_create_xyplot, this, _1) );

  // hide some signals from the GUI
  signal("create_component")->hidden(true);
  signal("delete_component")->hidden(true);
  signal("move_component")->hidden(true);
  signal("rename_component")->hidden(true);

}

////////////////////////////////////////////////////////////////////////////////

void CPlotter::signal_create_xyplot(SignalArgs &args)
{
  SignalOptions options( args );

  std::string name = options.value<std::string>("Plot name");
  URI parent = options.value<URI>("Parent");

  // some checks
  if(name.empty())
    throw BadValue(FromHere(), "The plot name is empty.");

  if(parent.empty())
    throw BadValue(FromHere(), "The parent is empty.");

  if(parent.scheme() != URI::Scheme::CPATH)
    throw InvalidURI(FromHere(), "The parent scheme is not CPATH");

  // create and add the component
  Handle< Component > parent_comp = Core::instance().root().access_component(parent);
  boost::shared_ptr< CPlotXY > plot(common::allocate_component<CPlotXY>(name));
  parent_comp->add_component( plot );
  plot->mark_basic();
  plot->set_data(m_data);
}

////////////////////////////////////////////////////////////////////////////////

void CPlotter::signature_create_xyplot(SignalArgs &args)
{
  SignalOptions options( args );

  options.add_option("Plot name", std::string() )
      .description("Name for the new plot");

  options.add_option("Parent", Core::instance().root().uri() )
      .supported_protocol( URI::Scheme::CPATH )
      .description("Parent of the new component");
}

////////////////////////////////////////////////////////////////////////////////

void CPlotter::set_data_set(const URI &uri)
{
  cf3_assert ( !uri.empty() );
  cf3_assert ( uri.scheme() == URI::Scheme::CPATH );

  m_data = uri;
}

////////////////////////////////////////////////////////////////////////////////

} // common
} // cf3

////////////////////////////////////////////////////////////////////////////////

