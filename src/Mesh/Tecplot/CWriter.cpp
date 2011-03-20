// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include <iostream>

#include "Common/Foreach.hpp"
// #include "Common/Log.hpp"
#include "Common/MPI/PE.hpp"
#include "Common/CBuilder.hpp"
#include "Common/FindComponents.hpp"
#include "Common/StringConversion.hpp"

#include "Mesh/Tecplot/CWriter.hpp"
#include "Mesh/GeoShape.hpp"
#include "Mesh/CMesh.hpp"
#include "Mesh/CTable.hpp"
#include "Mesh/CRegion.hpp"
#include "Mesh/CNodes.hpp"
#include "Mesh/CField2.hpp"
#include "Mesh/CFieldView.hpp"

//////////////////////////////////////////////////////////////////////////////

using namespace CF::Common;

namespace CF {
namespace Mesh {
namespace Tecplot {

#define CF_BREAK_LINE(f,x) { if( x+1 % 10) { f << "\n"; } }

////////////////////////////////////////////////////////////////////////////////

Common::ComponentBuilder < Tecplot::CWriter, CMeshWriter, LibTecplot> aTecplotWriter_Builder;

//////////////////////////////////////////////////////////////////////////////

CWriter::CWriter( const std::string& name )
: CMeshWriter(name)
{

}

/////////////////////////////////////////////////////////////////////////////

std::vector<std::string> CWriter::get_extensions()
{
  std::vector<std::string> extensions;
  extensions.push_back(".plt");
  return extensions;
}

/////////////////////////////////////////////////////////////////////////////

void CWriter::write_from_to(const CMesh::Ptr& mesh, boost::filesystem::path& path)
{

  m_mesh = mesh;

  // if the file is present open it
  boost::filesystem::fstream file;
  if (mpi::PE::instance().size() > 1)
  {
    path = boost::filesystem::basename(path) + "_P" + to_str(mpi::PE::instance().rank()) + boost::filesystem::extension(path);
  }
//  CFLog(VERBOSE, "Opening file " <<  path.string() << "\n");
  file.open(path,std::ios_base::out);
  if (!file) // didn't open so throw exception
  {
     throw boost::filesystem::filesystem_error( path.string() + " failed to open",
                                                boost::system::error_code() );
  }


  write_header(file);


  file.close();

}
/////////////////////////////////////////////////////////////////////////////

void CWriter::write_header(std::fstream& file)
{
  file << "TITLE      = COOLFluiD Mesh Data" << "\n";
  file << "VARIABLES  = ";

  Uint dimension = m_mesh->nodes().coordinates().row_size();
  // write the coordinate variable names
  for (Uint i = 0; i < dimension ; ++i)
  {
    file << " \"x" << i << "\" ";
  }
  
  // boost_foreach(boost::weak_ptr<CField2> field_ptr, m_fields)
  // {
  //   CField2& field = *field_ptr.lock();
  //   for (Uint iVar=0; iVar<field.nb_vars(); ++iVar)
  //   {
  //     CField2::VarType var_type = field.var_type(iVar);
  //     std::string var_name = field.var_name(iVar);
  //     if ( static_cast<Uint>(var_type) > 1)
  //     {
  //       for (Uint i=0; i<static_cast<Uint>(var_type); ++i)
  //       {
  //         file << " \"" << var_name << "["<<i<<"]\"";
  //       }
  //     }
  //     else
  //     {
  //       file << " \"" << var_name <<"\"";
  //     }
  //   }
  // }
  
  
  
  // loop over the element types
  // and create a zone in the tecplot file for each element type
  Uint glb_elem_id(0);
  boost_foreach (CElements& elements, find_components_recursively<CElements>(m_mesh->topology()) )
  {
    const ElementType& etype = elements.element_type();

    CList<Uint>& used_nodes = CEntities::used_nodes(elements);
    std::map<Uint,Uint> zone_node_idx;
    for (Uint n=0; n<used_nodes.size(); ++n)
      zone_node_idx[ used_nodes[n] ] = n+1;

    const Uint nbCellsInType  = elements.size();

    // Tecplot doesn't handle zones with 0 elements
    // which can happen in parallel, so skip them
    if (nbCellsInType == 0)
      continue;

    // print zone header,
    // one zone per element type per cpu
    // therefore the title is dependent on those parameters
    file << "ZONE "
         << "  T=\"" << elements.full_path().path() << "\""
         << ", N=" << used_nodes.size()
         << ", E=" << elements.size()
         << ", DATAPACKING=BLOCK"
         << ", ZONETYPE=" << zone_type(etype)
         << "\n\n";


         //    fout << ", VARLOCATION=( [" << init_id << "]=CELLCENTERED )" ;
         //   else
         //    fout << ", VARLOCATION=( [" << init_id << "-" << end_id << "]=CELLCENTERED )" ;
         // }

    file.setf(std::ios::scientific,std::ios::floatfield);
    file.precision(12);

    // loop over coordinates
    CTable<Real>& coordinates = m_mesh->nodes().coordinates();
    for (Uint d = 0; d < dimension; ++d)
    {
      file << "\n### variable x" << d << "\n\n"; // var name in comment
      boost_foreach(Uint n, used_nodes.array())
      {
        file << coordinates[n][d] << " ";
        CF_BREAK_LINE(file,n);
      }
      file << "\n";
    }
    file << "\n";


    file << "\n### connectivity\n\n";
    // write connectivity
    for (Uint e=0; e<elements.size(); ++e)
    {
      boost_foreach( CTable<Uint>::ConstRow e_nodes, elements.connectivity_table().array() )
      {
        boost_foreach ( Uint n, e_nodes)
        {
          file << zone_node_idx[n] << " ";
        }
        file << "\n";
      }        
    } // end write connectivity

  } 
  
}


std::string CWriter::zone_type(const ElementType& etype) const
{
  if ( etype.shape() == GeoShape::LINE)     return "FELINESEG";
  if ( etype.shape() == GeoShape::TRIAG)    return "FETRIANGLE";
  if ( etype.shape() == GeoShape::QUAD)     return "FEQUADRILATERAL";
  if ( etype.shape() == GeoShape::TETRA)    return "FETETRAHEDRON";
  if ( etype.shape() == GeoShape::PYRAM)    return "FEBRICK";  // with coalesced nodes
  if ( etype.shape() == GeoShape::PRISM)    return "FEBRICK";  // with coalesced nodes
  if ( etype.shape() == GeoShape::HEXA)     return "FEBRICK";   
  if ( etype.shape() == GeoShape::POINT)    return "FELINESEG"; // with coalesced nodes
}
////////////////////////////////////////////////////////////////////////////////

} // Tecplot
} // Mesh
} // CF