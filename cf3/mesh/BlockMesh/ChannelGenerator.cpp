// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include <boost/assign.hpp>

#include "common/Builder.hpp"
#include "common/Core.hpp"
#include "common/Exception.hpp"
#include "common/EventHandler.hpp"
#include "common/Log.hpp"
#include "common/OptionComponent.hpp"
#include "common/OptionList.hpp"
#include "common/OptionT.hpp"
#include "common/XML/SignalFrame.hpp"
#include "common/XML/SignalOptions.hpp"
#include "common/Timer.hpp"

#include "common/PE/Comm.hpp"

#include "mesh/BlockMesh/ChannelGenerator.hpp"

namespace cf3 {
namespace mesh {
namespace BlockMesh {

using namespace cf3::common;
using namespace cf3::mesh;

using namespace boost::assign;

ComponentBuilder < ChannelGenerator, Component, LibBlockMesh > ChannelGenerator_Builder;

ChannelGenerator::ChannelGenerator(const std::string& name): MeshGenerator(name)
{
  options().add_option("nb_parts", PE::Comm::instance().size())
    .description("Total number of partitions (e.g. number of processors)")
    .pretty_name("Number of Partitions");

  options().add_option("cell_overlap", PE::Comm::instance().size())
    .description("Cell overlap between two adjacent processors")
    .pretty_name("Cell Overlap");

  options().add_option("x_segments", 10u)
    .description("Number of segments in the X direction")
    .pretty_name("X segments");

  options().add_option("y_segments_half", 10u)
    .description("Number of segments in the Y direction for one half of the channel")
    .pretty_name("Y segments half");

  options().add_option("z_segments", 10u)
    .description("Number of segments in the Z direction")
    .pretty_name("Z segments");

  options().add_option("length", 10.)
    .description("Length in the X direction")
    .pretty_name("Length");

  options().add_option("half_height", 0.5)
    .description("Channel half height, in the Y-direction")
    .pretty_name("Half Height");

  options().add_option("width", 10.)
    .description("Channel witdh in the Z-direction")
    .pretty_name("Width");

  options().add_option("grading", 0.2)
    .description("Grading ratio. Values smaller than one refine towards the wall")
    .pretty_name("Grading Ratio");
}

void ChannelGenerator::execute()
{
  if(is_not_null(get_child("BlockData")))
    remove_component("BlockData");

  if(is_not_null(get_child("ParallelBlocks")))
    remove_component("ParallelBlocks");

  const Uint x_segs = options().option("x_segments").value<Uint>();
  const Uint y_segs_half = options().option("y_segments_half").value<Uint>();
  const Uint z_segs = options().option("z_segments").value<Uint>();

  const Real length = options().option("length").value<Real>();
  const Real half_height = options().option("half_height").value<Real>();
  const Real width = options().option("width").value<Real>();
  const Real ratio = options().option("grading").value<Real>();

  BlockData& blocks = *create_component<BlockData>("BlockData");

  blocks.scaling_factor = 1.;
  blocks.dimension = 3;

  blocks.points += list_of(0.    )(-half_height)(0.   )
                 , list_of(length)(-half_height)(0.   )
                 , list_of(0.    )( 0.         )(0.   )
                 , list_of(length)( 0.         )(0.   )
                 , list_of(0.    )( half_height)(0.   )
                 , list_of(length)( half_height)(0.   )
                 , list_of(0.    )(-half_height)(width)
                 , list_of(length)(-half_height)(width)
                 , list_of(0.    )( 0.         )(width)
                 , list_of(length)( 0.         )(width)
                 , list_of(0.    )( half_height)(width)
                 , list_of(length)( half_height)(width);

  blocks.block_points += list_of(0)(1)(3)(2)(6)(7)(9)(8)
                       , list_of(2)(3)(5)(4)(8)(9)(11)(10);
  blocks.block_subdivisions += list_of(x_segs)(y_segs_half)(z_segs)
                             , list_of(x_segs)(y_segs_half)(z_segs);
  blocks.block_gradings += list_of(1.)(1.)(1.)(1.)(1./ratio)(1./ratio)(1./ratio)(1./ratio)(1.)(1.)(1.)(1.)
                         , list_of(1.)(1.)(1.)(1.)(ratio   )(ratio   )(ratio   )(ratio   )(1.)(1.)(1.)(1.);
  blocks.block_distribution += 0, 2;

  blocks.patch_names += "bottom", "top", "front", "back", "left", "right";
  blocks.patch_types += "wall"      , "wall"   , "wall", "wall", "wall", "wall";
  blocks.patch_points += list_of(0)(1)(7)(6),
                         list_of(4)(10)(11)(5),
                         list_of(0)(2)(3)(1)(2)(4)(5)(3),
                         list_of(6)(7)(9)(8)(8)(9)(11)(10),
                         list_of(0)(6)(8)(2)(2)(8)(10)(4),
                         list_of(1)(3)(9)(7)(3)(5)(11)(9);

  const Uint nb_parts = options().option("nb_parts").value<Uint>();

  Mesh& mesh = *m_mesh;

  if(PE::Comm::instance().is_active() && nb_parts > 1)
  {
    const Uint cell_overlap = options().option("cell_overlap").value<Uint>();
    BlockData& parallel_blocks = *create_component<BlockData>("ParallelBlocks");
    partition_blocks(blocks, nb_parts, XX, parallel_blocks);
    build_mesh(parallel_blocks, mesh, cell_overlap);
  }
  else
  {
    build_mesh(blocks, mesh);
  }

  raise_mesh_loaded();
}

} // BlockMesh
} // mesh
} // cf3
