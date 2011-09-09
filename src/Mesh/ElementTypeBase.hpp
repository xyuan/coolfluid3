// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

/// @file
/// @brief This file deals with the translation from the dynamic API of
/// ElementType to static implementations of element types.
///
/// Implementations of Element types don't inherit from Common::Component
/// e.g. LagrangeP1::Triag2D. \n
/// The actual concrete component is created as ElementTypeT<LagrangeP1::Triag2D>
///
/// @author Willem Deconinck

#ifndef CF_Mesh_ElementTypeBase_hpp
#define CF_Mesh_ElementTypeBase_hpp

////////////////////////////////////////////////////////////////////////////////

#include "Common/StringConversion.hpp"
#include "Math/MatrixTypes.hpp"
#include "Mesh/GeoShape.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Mesh {
  class  ElementType;
  struct ElementTypeFaceConnectivity;

////////////////////////////////////////////////////////////////////////////////

/// @brief Fallback class if a concrete Element Type doesn't implement a static function
///
/// Implements all functions ElementTypeT expects.
/// When creating a new element type, this list shows all the possible static functions
/// that can be implemented and are recognized by the dynamic interface from ElementType
/// @author Willem Deconinck
template <typename ETYPE, typename TR>
class ElementTypeBase
{
public:

  // Information coming from Traits
  // ------------------------------
  typedef typename TR::SF SF;
  static const GeoShape::Type shape = TR::SF::shape;
  enum { order          = TR::SF::order          };
  enum { dimensionality = TR::SF::dimensionality };
  enum { dimension      = TR::dimension          };
  enum { nb_faces       = TR::nb_faces           };
  enum { nb_edges       = TR::nb_edges           };
  enum { nb_nodes       = TR::SF::nb_nodes       };

  static std::string type_name() { return GeoShape::Convert::instance().to_str(shape)+Common::to_str((Uint)dimension)+"D"; }

  // Typedefs for special matrices
  // -----------------------------
  typedef typename TR::SF::MappedCoordsT                 MappedCoordsT;
  typedef Eigen::Matrix<Real, dimension, 1>              CoordsT;
  typedef Eigen::Matrix<Real, nb_nodes, dimension>       NodesT;
  typedef Eigen::Matrix<Real, dimensionality, dimension> JacobianT;

  // Not-implemented static functions
  // --------------------------------
  static void compute_mapped_coordinate(const CoordsT& coord, const NodesT& nodes, MappedCoordsT& mapped_coord);
  static Real jacobian_determinant(const MappedCoordsT& mapped_coord, const NodesT& nodes);
  static void compute_jacobian(const MappedCoordsT& mapped_coord, const NodesT& nodes, JacobianT& jacobian);
  static void compute_jacobian_adjoint(const MappedCoordsT& mapped_coord, const NodesT& nodes, JacobianT& result);
  static Real volume(const NodesT& nodes);
  static Real area(const NodesT& nodes);
  static void compute_normal(const NodesT& nodes, CoordsT& normal);
  static void compute_centroid(const NodesT& nodes , CoordsT& centroid);
  static bool is_coord_in_element(const CoordsT& coord, const NodesT& nodes);
  static void compute_plane_jacobian_normal(const MappedCoordsT& mapped_coord, const NodesT& nodes, const CoordRef orientation, CoordsT& result);

  // Implemented static functions
  // ----------------------------
  static MappedCoordsT mapped_coordinate(const CoordsT& coord, const NodesT& nodes);
  static JacobianT jacobian(const MappedCoordsT& mapped_coord, const NodesT& nodes);
  static CoordsT plane_jacobian_normal(const MappedCoordsT& mapped_coord, const NodesT& nodes, const CoordRef orientation);

private:

  static void throw_not_implemented(const Common::CodeLocation& where);

};

////////////////////////////////////////////////////////////////////////////////

template <typename ETYPE,typename TR>
const GeoShape::Type ElementTypeBase<ETYPE,TR>::shape;

////////////////////////////////////////////////////////////////////////////////

template <typename ETYPE,typename TR>
inline void ElementTypeBase<ETYPE,TR>::throw_not_implemented(const Common::CodeLocation& where)
{
  throw Common::NotImplemented(where,"static function not implemented / not applicable for element type ["+ETYPE::type_name()+"]");
}

////////////////////////////////////////////////////////////////////////////////

template <typename ETYPE,typename TR>
typename ElementTypeBase<ETYPE,TR>::MappedCoordsT ElementTypeBase<ETYPE,TR>::mapped_coordinate(const CoordsT& coord, const NodesT& nodes)
{
  throw_not_implemented(FromHere());
  return MappedCoordsT();
}

////////////////////////////////////////////////////////////////////////////////

template <typename ETYPE,typename TR>
void ElementTypeBase<ETYPE,TR>::compute_mapped_coordinate(const CoordsT& coord, const NodesT& nodes, MappedCoordsT& mapped_coord)
{
  throw_not_implemented(FromHere());
}

////////////////////////////////////////////////////////////////////////////////

template <typename ETYPE,typename TR>
Real ElementTypeBase<ETYPE,TR>::jacobian_determinant(const MappedCoordsT& mapped_coord, const NodesT& nodes)
{
  throw_not_implemented(FromHere());
  return 0.;
}

////////////////////////////////////////////////////////////////////////////////

template <typename ETYPE,typename TR>
typename ElementTypeBase<ETYPE,TR>::JacobianT ElementTypeBase<ETYPE,TR>::jacobian(const MappedCoordsT& mapped_coord, const NodesT& nodes)
{
  throw_not_implemented(FromHere());
  return JacobianT();
}

////////////////////////////////////////////////////////////////////////////////

template <typename ETYPE,typename TR>
void ElementTypeBase<ETYPE,TR>::compute_jacobian(const MappedCoordsT& mapped_coord, const NodesT& nodes, JacobianT& jacobian)
{
  throw_not_implemented(FromHere());
}

////////////////////////////////////////////////////////////////////////////////

template <typename ETYPE,typename TR>
void ElementTypeBase<ETYPE,TR>::compute_jacobian_adjoint(const MappedCoordsT& mapped_coord, const NodesT& nodes, JacobianT& result)
{
  throw_not_implemented(FromHere());
}

////////////////////////////////////////////////////////////////////////////////

template <typename ETYPE,typename TR>
Real ElementTypeBase<ETYPE,TR>::volume(const NodesT& nodes)
{
  throw_not_implemented(FromHere());
  return 0.;
}

////////////////////////////////////////////////////////////////////////////////

template <typename ETYPE,typename TR>
Real ElementTypeBase<ETYPE,TR>::area(const NodesT& nodes)
{
  throw_not_implemented(FromHere());
  return 0.;
}

////////////////////////////////////////////////////////////////////////////////

template <typename ETYPE,typename TR>
void ElementTypeBase<ETYPE,TR>::compute_normal(const NodesT& nodes, CoordsT& normal)
{
  throw_not_implemented(FromHere());
}

////////////////////////////////////////////////////////////////////////////////

template <typename ETYPE,typename TR>
void ElementTypeBase<ETYPE,TR>::compute_centroid(const NodesT& nodes , CoordsT& centroid)
{
  throw_not_implemented(FromHere());
}

////////////////////////////////////////////////////////////////////////////////

template <typename ETYPE,typename TR>
bool ElementTypeBase<ETYPE,TR>::is_coord_in_element(const CoordsT& coord, const NodesT& nodes)
{
  throw_not_implemented(FromHere());
  return false;
}

////////////////////////////////////////////////////////////////////////////////

template <typename ETYPE,typename TR>
typename ElementTypeBase<ETYPE,TR>::CoordsT ElementTypeBase<ETYPE,TR>::plane_jacobian_normal(const MappedCoordsT& mapped_coord, const NodesT& nodes, const CoordRef orientation)
{
  throw_not_implemented(FromHere());
  return CoordsT();
}

////////////////////////////////////////////////////////////////////////////////

template <typename ETYPE,typename TR>
void ElementTypeBase<ETYPE,TR>::compute_plane_jacobian_normal(const MappedCoordsT& mapped_coord, const NodesT& nodes, const CoordRef orientation, CoordsT& result)
{
  throw_not_implemented(FromHere());
}

////////////////////////////////////////////////////////////////////////////////

} // Mesh
} // CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_Mesh_ElementTypeBase_hpp