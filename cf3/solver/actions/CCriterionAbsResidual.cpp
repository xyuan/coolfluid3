// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "common/Builder.hpp"
#include "common/OptionComponent.hpp"
#include "common/OptionList.hpp"
#include "common/OptionT.hpp"
#include "common/PropertyList.hpp"

#include "solver/CTime.hpp"
#include "solver/actions/CCriterionAbsResidual.hpp"

namespace cf3 {
namespace solver {
namespace actions {

using namespace common;

ComponentBuilder< CCriterionAbsResidual, CCriterion, LibActions > CCriterionAbsResidual_Builder;

////////////////////////////////////////////////////////////////////////////////

CCriterionAbsResidual::CCriterionAbsResidual( const std::string& name  ) :
  CCriterion ( name ),
  m_max_iter(0)
{
  properties()["brief"] = std::string("Maximum Iterations Criterion object");
  std::string description = properties().value<std::string>("description")+
    "Returns true if a the maximum number of iterations is achived\n";
  properties()["description"] = description;

  options().add_option("MaxIter", m_max_iter)
      .description("Maximum number of iterations")
      .mark_basic()
      .link_to( &m_max_iter );

  options().add_option("iteration", m_iter_comp)
      .description("Iteration tracking component")
      .pretty_name("Iteration")
      .link_to(&m_iter_comp);
}

CCriterionAbsResidual::~CCriterionAbsResidual() {}

////////////////////////////////////////////////////////////////////////////////

bool CCriterionAbsResidual::operator()()
{
  if (is_null(m_iter_comp)) throw SetupError(FromHere(),"Component holding iteration number was not set in ["+uri().string()+"]");
  Component& comp_iter = *m_iter_comp;

  const Uint cur_iter = comp_iter.options().option("iter").value<Uint>();

  return ( cur_iter > m_max_iter );
}

////////////////////////////////////////////////////////////////////////////////

} // actions
} // solver
} // cf3
