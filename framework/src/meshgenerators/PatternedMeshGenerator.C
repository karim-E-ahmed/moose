//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PatternedMeshGenerator.h"

#include "CastUniquePointer.h"

#include "libmesh/replicated_mesh.h"
#include "libmesh/distributed_mesh.h"
#include "libmesh/boundary_info.h"
#include "libmesh/mesh_modification.h"

registerMooseObject("MooseApp", PatternedMeshGenerator);

template <>
InputParameters
validParams<PatternedMeshGenerator>()
{
  InputParameters params = validParams<MeshGenerator>();

  params.addRequiredParam<std::vector<MeshGeneratorName>>("inputs", "The input MeshGenerators.");
  params.addRangeCheckedParam<Real>(
      "x_width", 0, "x_width>=0", "The tile width in the x direction");
  params.addRangeCheckedParam<Real>(
      "y_width", 0, "y_width>=0", "The tile width in the y direction");
  params.addRangeCheckedParam<Real>(
      "z_width", 0, "z_width>=0", "The tile width in the z direction");

  // Boundaries : user has to provide id or name for each boundary
  // If an id is provided, any provided name for this particular boundary will be ignored.

  // x boundary names
  params.addParam<BoundaryName>("left_boundary", "left", "name of the left (x) boundary");
  params.addParam<BoundaryName>("right_boundary", "right", "name of the right (x) boundary");

  // y boundary names
  params.addParam<BoundaryName>("top_boundary", "top", "name of the top (y) boundary");
  params.addParam<BoundaryName>("bottom_boundary", "bottom", "name of the bottom (y) boundary");

  // x boundary ids
  params.addParam<boundary_id_type>("left_boundary_id", "id of the left (x) boundary");
  params.addParam<boundary_id_type>("right_boundary_id", "id of the right (x) boundary");

  // y boundary ids
  params.addParam<boundary_id_type>("top_boundary_id", "name of the top (y) boundary");
  params.addParam<boundary_id_type>("bottom_boundary_id", "name of the bottom (y) boundary");

  params.addRequiredParam<std::vector<std::vector<unsigned int>>>(
      "pattern", "A double-indexed array starting with the upper-left corner");

  params.addClassDescription("Creates a 2D mesh from a specified set of unique 'tiles' meshes and "
                             "a two-dimensional pattern.");

  return params;
}

PatternedMeshGenerator::PatternedMeshGenerator(const InputParameters & parameters)
  : MeshGenerator(parameters),
    _input_names(getParam<std::vector<MeshGeneratorName>>("inputs")),
    _pattern(getParam<std::vector<std::vector<unsigned int>>>("pattern")),
    _x_width(getParam<Real>("x_width")),
    _y_width(getParam<Real>("y_width")),
    _z_width(getParam<Real>("z_width"))
{
  _mesh_ptrs.reserve(_input_names.size());
  for (auto i = beginIndex(_input_names); i < _input_names.size(); ++i)
    _mesh_ptrs.push_back(&getMeshByName(_input_names[i]));
}

std::unique_ptr<MeshBase>
PatternedMeshGenerator::generate()
{
  // Reserve spaces for all the meshes
  _meshes.reserve(_input_names.size());

  // Read in all of the meshes
  for (auto i = beginIndex(_input_names); i < _input_names.size(); ++i)
  {
    _meshes.push_back(dynamic_pointer_cast<ReplicatedMesh>(*_mesh_ptrs[i]));
  }

  // Data structure that holds each row
  _row_meshes.resize(_pattern.size());

  // Trying to get the boundaries by name
  boundary_id_type left =
      _meshes[0]->get_boundary_info().get_id_by_name(getParam<BoundaryName>("left_boundary"));
  boundary_id_type right =
      _meshes[0]->get_boundary_info().get_id_by_name(getParam<BoundaryName>("right_boundary"));
  boundary_id_type top =
      _meshes[0]->get_boundary_info().get_id_by_name(getParam<BoundaryName>("top_boundary"));
  boundary_id_type bottom =
      _meshes[0]->get_boundary_info().get_id_by_name(getParam<BoundaryName>("bottom_boundary"));

  // For each boundary, check if the user provided an id
  if (isParamValid("left_boundary_id"))
    left = getParam<boundary_id_type>("left_boundary_id");
  if (isParamValid("right_boundary_id"))
    right = getParam<boundary_id_type>("right_boundary_id");
  if (isParamValid("top_boundary_id"))
    top = getParam<boundary_id_type>("top_boundary_id");
  if (isParamValid("bottom_boundary_id"))
    bottom = getParam<boundary_id_type>("bottom_boundary_id");

  // Check if all the boundaries have been initialized
  if (left == -123)
    mooseError("The left boundary has not been initialized properly.");
  if (right == -123)
    mooseError("The right boundary has not been initialized properly.");
  if (top == -123)
    mooseError("The top boundary has not been initialized properly.");
  if (bottom == -123)
    mooseError("The bottom boundary has not been initialized properly.");

  // Build each row mesh
  for (auto i = beginIndex(_pattern); i < _pattern.size(); ++i)
    for (auto j = beginIndex(_pattern[i]); j < _pattern[i].size(); ++j)
    {
      Real deltax = j * _x_width, deltay = i * _y_width;

      // If this is the first cell of the row initialize the row mesh
      if (j == 0)
      {
        //_row_meshes[i] = _mesh_ptrs[_pattern[i][j]]->clone();
        auto clone = _meshes[_pattern[i][j]]->clone();
        _row_meshes[i] = dynamic_pointer_cast<ReplicatedMesh>(clone);

        MeshTools::Modification::translate(*_row_meshes[i], deltax, -deltay, 0);

        continue;
      }

      ReplicatedMesh & cell_mesh = *_meshes[_pattern[i][j]];

      // Move the mesh into the right spot.  -i because we are starting at the top
      MeshTools::Modification::translate(cell_mesh, deltax, -deltay, 0);

      _row_meshes[i]->stitch_meshes(cell_mesh,
                                    right,
                                    left,
                                    TOLERANCE,
                                    /*clear_stitched_boundary_ids=*/true);

      // Undo the translation
      MeshTools::Modification::translate(cell_mesh, -deltax, deltay, 0);
    }

  // Now stitch together the rows
  // We're going to stitch them all to row 0 (which is the real mesh)
  for (auto i = beginIndex(_pattern) + 1; i < _pattern.size(); i++)
    _row_meshes[0]->stitch_meshes(
        *_row_meshes[i], bottom, top, TOLERANCE, /*clear_stitched_boundary_ids=*/true);

  return dynamic_pointer_cast<MeshBase>(_row_meshes[0]);
}
