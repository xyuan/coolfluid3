// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "Common/CBuilder.hpp"
#include "Common/OptionURI.hpp"
#include "Common/OptionT.hpp"
#include "Common/Foreach.hpp"
#include "Common/Log.hpp"
#include "Mesh/CFieldView.hpp"
#include "Mesh/CField.hpp"
#include "Mesh/CSpace.hpp"
#include "Mesh/ElementType.hpp"
#include "Mesh/CEntities.hpp"

#include "FVM/BCReflectCons2D.hpp"

/////////////////////////////////////////////////////////////////////////////////////

using namespace CF::Common;
using namespace CF::Mesh;

namespace CF {
namespace FVM {

///////////////////////////////////////////////////////////////////////////////////////

Common::ComponentBuilder < BCReflectCons2D, CAction, LibFVM > BCReflectCons2D_Builder;

///////////////////////////////////////////////////////////////////////////////////////
  
BCReflectCons2D::BCReflectCons2D ( const std::string& name ) : 
  CLoopOperation(name),
  m_connected_solution("solution_view"),
  m_face_normal("face_normal")
{
  mark_basic();
  // options
  m_properties.add_option(OptionURI::create("Solution","Cell based solution","cpath:/",URI::Scheme::CPATH))
    ->attach_trigger ( boost::bind ( &BCReflectCons2D::config_solution,   this ) )
    ->add_tag("solution");
    
  m_properties.add_option(OptionURI::create("FaceNormal","Unit normal to the face, outward from left cell", URI("cpath:"), URI::Scheme::CPATH))
    ->attach_trigger ( boost::bind ( &BCReflectCons2D::config_normal,   this ) )
    ->add_tag("face_normal");

  m_properties["Elements"].as_option().attach_trigger ( boost::bind ( &BCReflectCons2D::trigger_elements,   this ) );
  
}

////////////////////////////////////////////////////////////////////////////////

void BCReflectCons2D::config_solution()
{
  URI uri;  property("Solution").put_value(uri);
  CField::Ptr comp = Core::instance().root()->access_component_ptr(uri)->as_ptr<CField>();
  if ( is_null(comp) ) throw CastingFailed (FromHere(), "Field must be of a CField or derived type");
  m_connected_solution.set_field(comp);
}

////////////////////////////////////////////////////////////////////////////////

void BCReflectCons2D::config_normal()
{
  URI uri;  property("FaceNormal").put_value(uri);
  CField& comp = Core::instance().root()->access_component(uri).as_type<CField>();
  m_face_normal.set_field(comp);
}

////////////////////////////////////////////////////////////////////////////////

void BCReflectCons2D::trigger_elements()
{
  m_can_start_loop = m_connected_solution.set_elements(elements());
  m_can_start_loop &=  m_face_normal.set_elements(elements());
}

/////////////////////////////////////////////////////////////////////////////////////

void BCReflectCons2D::execute()
{
  cf_assert(m_face_normal.size());
  
  RealVector2 normal = to_vector(m_face_normal[idx()]);
  RealVector2 U;
  U << m_connected_solution[idx()][INNER][1]/m_connected_solution[idx()][INNER][0],
       m_connected_solution[idx()][INNER][2]/m_connected_solution[idx()][INNER][0];
  
  RealVector2 U_n = (U.dot(normal)) *normal;// normal velocity
  RealVector2 U_t = U - U_n;         // tangential velocity

  U = -U_n + U_t;  // switched sign of normal velocity

  // Change value in ghost cell
  m_connected_solution[idx()][GHOST][0] = m_connected_solution[idx()][INNER][0];
  m_connected_solution[idx()][GHOST][1] = U[XX]*m_connected_solution[idx()][INNER][0];
  m_connected_solution[idx()][GHOST][2] = U[YY]*m_connected_solution[idx()][INNER][0];
  m_connected_solution[idx()][GHOST][3] = m_connected_solution[idx()][INNER][3];
  
}

////////////////////////////////////////////////////////////////////////////////

} // FVM
} // CF

////////////////////////////////////////////////////////////////////////////////////

