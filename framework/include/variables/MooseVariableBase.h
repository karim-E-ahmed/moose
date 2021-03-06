//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef MOOSEVARIABLEBASE_H
#define MOOSEVARIABLEBASE_H

#include "MooseTypes.h"
#include "MooseArray.h"

#include "libmesh/tensor_value.h"
#include "libmesh/vector_value.h"
#include "libmesh/fe_type.h"

// libMesh forward declarations
namespace libMesh
{
class DofMap;
class Variable;
}

class SubProblem;
class SystemBase;
class MooseMesh;

class MooseVariableBase
{
public:
  MooseVariableBase(unsigned int var_num,
                    const FEType & fe_type,
                    SystemBase & sys,
                    Moose::VarKindType var_kind,
                    THREAD_ID tid);
  virtual ~MooseVariableBase();

  /**
   * Get variable number coming from libMesh
   * @return the libmesh variable number
   */
  unsigned int number() const { return _var_num; }

  /**
   * Get the type of finite element object
   */
  const FEType & feType() const { return _fe_type; }

  /**
   * Get the system this variable is part of.
   */
  SystemBase & sys() { return _sys; }

  /**
   * Get the variable name
   */
  const std::string & name() const;

  /**
   * Get all global dofindices for the variable
   */
  const std::vector<dof_id_type> & allDofIndices() const;
  unsigned int totalVarDofs() { return allDofIndices().size(); }

  /**
   * Kind of the variable (Nonlinear, Auxiliary, ...)
   */
  Moose::VarKindType kind() const { return _var_kind; }

  /**
   * Set the scaling factor for this variable
   */
  void scalingFactor(Real factor) { _scaling_factor = factor; }

  /**
   * Get the scaling factor for this variable
   */
  Real scalingFactor() const { return _scaling_factor; }

  /**
   * Get the order of this variable
   * Note: Order enum can be implicitly converted to unsigned int.
   */
  Order order() const;

  /**
   * The DofMap associated with the system this variable is in.
   */
  const DofMap & dofMap() const { return _dof_map; }

  std::vector<dof_id_type> & dofIndices() { return _dof_indices; }

  const std::vector<dof_id_type> & dofIndices() const { return _dof_indices; }

  unsigned int numberOfDofs() { return _dof_indices.size(); }

  /**
   * Is this variable nodal
   * @return true if it nodal, otherwise false
   */
  virtual bool isNodal() const = 0;

  /**
   * @returns true if this is a vector-valued element, false otherwise.
   */
  virtual bool isVector() const = 0;

  void computingJacobian(bool computing_jacobian) { _computing_jacobian = computing_jacobian; }

protected:
  /// variable number (from libMesh)
  unsigned int _var_num;
  /// The FEType associated with this variable
  FEType _fe_type;
  /// variable number within MOOSE
  unsigned int _index;
  Moose::VarKindType _var_kind;
  /// Problem this variable is part of
  SubProblem & _subproblem;
  /// System this variable is part of
  SystemBase & _sys;

  /// libMesh variable object for this variable
  const Variable & _variable;

  /// DOF map
  const DofMap & _dof_map;
  /// DOF indices
  std::vector<dof_id_type> _dof_indices;

  /// mesh the variable is active in
  MooseMesh & _mesh;

  /// scaling factor for this variable
  Real _scaling_factor;

  bool _computing_jacobian;

  /// Thread ID
  THREAD_ID _tid;
};

#endif /* MOOSEVARIABLEBASE_H */
