//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef CREATESPECIALPROBLEMACTION_H
#define CREATESPECIALPROBLEMACTION_H

#include "Action.h"

class CreateSpecialProblemAction;

template <>
InputParameters validParams<CreateSpecialProblemAction>();

/**
 * Small test action that creates a special problem based on a particular input syntax.
 */
class CreateSpecialProblemAction : public Action
{
public:
  CreateSpecialProblemAction(InputParameters parameters);

  virtual void act() override;
};

#endif /* CREATESPECIALPROBLEMACTION_H */
