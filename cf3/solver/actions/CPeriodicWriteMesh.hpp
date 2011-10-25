// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef cf3_solver_actions_CPeriodicWriteMesh_hpp
#define cf3_solver_actions_CPeriodicWriteMesh_hpp

#include "solver/actions/LibActions.hpp"
#include "solver/Action.hpp"

/////////////////////////////////////////////////////////////////////////////////////

namespace cf3 {
namespace mesh   { class Field; class Mesh; class WriteMesh; }
namespace solver {
namespace actions {

class solver_actions_API CPeriodicWriteMesh : public solver::Action {

public: // typedefs

  /// pointers
  typedef boost::shared_ptr<CPeriodicWriteMesh> Ptr;
  typedef boost::shared_ptr<CPeriodicWriteMesh const> ConstPtr;

public: // functions
  /// Contructor
  /// @param name of the component
  CPeriodicWriteMesh ( const std::string& name );

  /// Virtual destructor
  virtual ~CPeriodicWriteMesh() {}

  /// Get the class name
  static std::string type_name () { return "CPeriodicWriteMesh"; }

  /// execute the action
  virtual void execute ();

private: // data

  boost::weak_ptr<Component> m_iterator;  ///< component that holds the iteration

  mesh::WriteMesh& m_writer; ///< mesh writer

};

////////////////////////////////////////////////////////////////////////////////

} // actions
} // solver
} // cf3

#endif // cf3_solver_actions_CPeriodicWriteMesh_hpp