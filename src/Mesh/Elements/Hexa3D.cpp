#include <boost/foreach.hpp>

#include "Common/ObjectProvider.hpp"
#include "Hexa3D.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Mesh {
  
////////////////////////////////////////////////////////////////////////////////

Hexa3D::Hexa3D()
{
  m_shape = shape;
  m_dimension = dimension;
  m_dimensionality = dimensionality;
  m_nb_faces = nb_faces;
  m_nb_edges = nb_edges;
}

////////////////////////////////////////////////////////////////////////////////

} // Mesh
} // CF
