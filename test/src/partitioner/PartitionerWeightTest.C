//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PartitionerWeightTest.h"

registerMooseObject("MooseTestApp", PartitionerWeightTest);

template <>
InputParameters
validParams<PartitionerWeightTest>()
{
  InputParameters params = validParams<PetscExternalPartitioner>();

  params.addClassDescription("Partition mesh using the weighted graph");

  return params;
}

PartitionerWeightTest::PartitionerWeightTest(const InputParameters & params)
  : PetscExternalPartitioner(params)
{
}

std::unique_ptr<Partitioner>
PartitionerWeightTest::clone() const
{
  return libmesh_make_unique<PartitionerWeightTest>(_pars);
}

dof_id_type
PartitionerWeightTest::computeElementWeight(Elem & elem)
{
  auto centroid = elem.centroid();
  if (centroid(0) < 0.5)
    return 2;
  else
    return 1;
}

dof_id_type
PartitionerWeightTest::computeSideWeight(Elem & elem, unsigned int side)
{
  auto side_elem = elem.build_side_ptr(side);
  auto centroid = side_elem->centroid();
  if (centroid(0) == 0.5)
    return 20;
  else
    return 1;
}
