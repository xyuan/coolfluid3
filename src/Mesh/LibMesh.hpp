// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_LibMesh_hpp
#define CF_LibMesh_hpp

////////////////////////////////////////////////////////////////////////////////

#include "Common/ExportAPI.hpp"
#include "Common/LibraryRegister.hpp"

////////////////////////////////////////////////////////////////////////////////

/// Define the macro Mesh_API
/// @note build system defines Mesh_EXPORTS when compiling MeshTools files
#ifdef Mesh_EXPORTS
#   define Mesh_API      CF_EXPORT_API
#   define Mesh_TEMPLATE
#else
#   define Mesh_API      CF_IMPORT_API
#   define Mesh_TEMPLATE CF_TEMPLATE_EXTERN
#endif

////////////////////////////////////////////////////////////////////////////////

namespace CF {

  /// Basic Classes for Mesh applications used by CF
  namespace Mesh {

////////////////////////////////////////////////////////////////////////////////

  /// Class defines the initialization and termination of the library Mesh
  /// @author Tiago Quintino
  class Mesh_API LibMesh :
      public Common::LibraryRegister<LibMesh>
  {
  public:

    /// Static function that returns the module name.
    /// Must be implemented for the LibraryRegister template
    /// @return name of the module
    static std::string library_name() { return "Mesh"; }

    /// Static function that returns the description of the module.
    /// Must be implemented for the LibraryRegister template
    /// @return descripton of the module
    static std::string library_description()
    {
      return "This library implements the mesh manipulation API.";
    }

    /// Gets the Class name
    static std::string type_name() { return "LibMesh"; }

    /// Start profiling
    virtual void initiate();

    /// Stop profiling
    virtual void terminate();

  }; // end LibMesh

////////////////////////////////////////////////////////////////////////////////

} // namespace Mesh
} // namespace CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_LibMesh_hpp
