// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef cf3_mesh_neu_Writer_hpp
#define cf3_mesh_neu_Writer_hpp

////////////////////////////////////////////////////////////////////////////////

#include "mesh/MeshWriter.hpp"
#include "common/Table.hpp"

#include "mesh/neu/LibNeu.hpp"
#include "mesh/neu/Shared.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace cf3 {
namespace mesh {
  class Elements;

namespace neu {


//////////////////////////////////////////////////////////////////////////////

/// This class defines neu mesh format writer
/// @author Willem Deconinck
class neu_API Writer : public MeshWriter, public Shared
{

public: // typedefs

  typedef boost::shared_ptr<Writer> Ptr;
  typedef boost::shared_ptr<Writer const> ConstPtr;

private : // typedefs


public: // functions

  /// constructor
  Writer( const std::string& name );

  /// Gets the Class name
  static std::string type_name() { return "Writer"; }

  virtual void write_from_to(const Mesh& mesh, const common::URI& path);

  virtual std::string get_format() { return "neu"; }

  virtual std::vector<std::string> get_extensions();

private: // functions

  void write_headerData(std::fstream& file);

  void write_coordinates(std::fstream& file);

  void write_connectivity(std::fstream& file);

  void write_groups(std::fstream& file);

  void write_boundaries(std::fstream& file);

  void create_nodes_to_element_connectivity();

private: // data

  /// implementation detail, raw pointers are safe as keys
  std::map<Elements::ConstPtr,Uint> m_global_start_idx;

  std::string m_fileBasename;

}; // end Writer


////////////////////////////////////////////////////////////////////////////////

} // neu
} // mesh
} // cf3

////////////////////////////////////////////////////////////////////////////////

#endif // cf3_mesh_neu_Writer_hpp