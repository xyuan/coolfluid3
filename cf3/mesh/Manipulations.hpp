// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef cf3_mesh_Manipulations_hpp
#define cf3_mesh_Manipulations_hpp

////////////////////////////////////////////////////////////////////////////////

#include "common/PE/Buffer.hpp"

#include "mesh/List.hpp"
#include "mesh/Table.hpp"
#include "mesh/DynTable.hpp"

namespace cf3 {
namespace mesh {

  class Geometry;
  class Elements;

  ////////////////////////////////////////////////////////////////////////////////

struct RemoveNodes
{
  RemoveNodes(Geometry& nodes);

  void operator() (const Uint idx);

  void flush();

  List<Uint>::Buffer       glb_idx;
  List<Uint>::Buffer       rank;
  Table<Real>::Buffer      coordinates;
  DynTable<Uint>::Buffer   connected_elements;
};

struct RemoveElements
{
  RemoveElements(Elements& elements);

  void operator() (const Uint idx);

  void flush();

  List<Uint>::Buffer       glb_idx;
  List<Uint>::Buffer       rank;
  Table<Uint>::Buffer      connected_nodes;
};


struct PackUnpackElements: common::PE::PackedObject
{
  enum CommunicationType {COPY=0, MIGRATE=1};

  PackUnpackElements(Elements& elements);

  PackUnpackElements& operator() (const Uint idx,const bool remove_after_pack = false);

  void remove(const Uint idx);

  virtual void pack(common::PE::Buffer& buf);

  virtual void unpack(common::PE::Buffer& buf);

  void flush();

  Elements& m_elements;
  Uint m_idx;
  bool m_remove_after_pack;
  List<Uint>::Buffer       glb_idx;
  List<Uint>::Buffer       rank;
  Table<Uint>::Buffer      connected_nodes;
};


struct PackUnpackNodes: common::PE::PackedObject
{
  enum CommunicationType {COPY=0, MIGRATE=1};

  PackUnpackNodes(Geometry& nodes);

  PackUnpackNodes& operator() (const Uint idx,const bool remove_after_pack = false);

  void remove(const Uint idx);

  virtual void pack(common::PE::Buffer& buf);

  virtual void unpack(common::PE::Buffer& buf);

  void flush();

  Geometry& m_nodes;
  Uint m_idx;
  bool m_remove_after_pack;
  List<Uint>::Buffer       glb_idx;
  List<Uint>::Buffer       rank;
  Table<Real>::Buffer      coordinates;
  DynTable<Uint>::Buffer   connected_elements;
};

////////////////////////////////////////////////////////////////////////////////

} // mesh
} // cf3

////////////////////////////////////////////////////////////////////////////////

#endif // cf3_mesh_Manipulations_hpp