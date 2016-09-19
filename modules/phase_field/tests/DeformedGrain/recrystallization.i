[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 128
  ny = 128
  nz = 0
  xmin = 0
  xmax = 256
  ymin = 0
  ymax = 256
  uniform_refine = 1
  elem_type = QUAD4
[]

[GlobalParams]
  int_width = 4.0
  block = 0
  op_num = 30
  deformed_grain_num = 20
  var_name_base = gr
  grain_tracker = grain_tracker
  time_scale = 1e-2
  length_scale = 1e-8
  Elas_Mod = 2e11
  Disloc_Den = 8e15
[]

[Variables]
  [./PolycrystalVariables]
  [../]
[]

[AuxVariables]
  [./bnds]
    order = FIRST
    family = LAGRANGE
  [../]
  [./unique_grains]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./var_indices]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[ICs]
  [./PolycrystalICs]
    [./PolycrystalVoronoiIC]
      op_num = 20
      grain_num = 20
    [../]
  [../]
  [./eta_IC]
    variable = gr20
    type = RandomIC
    min = 0.0
    max = 1.0e-3
    seed = 12345
  [../]
  [./eta1_IC]
    variable = gr21
    type = RandomIC
    min = 0.0
    max = 1.0e-3
    seed = 2468
  [../]
  [./eta2_IC]
    variable = gr22
    type = RandomIC
    min = 0.0
    max = 1.0e-3
    seed = 8901
  [../]
  [./eta3_IC]
    variable = gr23
    type = RandomIC
    min = 0.0
    max = 1.0e-3
    seed = 112233
  [../]
  [./eta4_IC]
    variable = gr24
    type = RandomIC
    min = 0.0
    max = 1.0e-3
    seed = 334455
  [../]
  [./eta5_IC]
    variable = gr25
    type = RandomIC
    min = 0.0
    max = 1.0e-3
    seed = 667788
  [../]
  [./eta6_IC]
    variable = gr26
    type = RandomIC
    min = 0.0
    max = 1.0e-3
    seed = 516688
  [../]
  [./eta7_IC]
    variable = gr27
    type = RandomIC
    min = 0.0
    max = 1.0e-3
    seed = 991110
  [../]
  [./eta8_IC]
    variable = gr28
    type = RandomIC
    min = 0.0
    max = 1.0e-3
    seed = 102030
  [../]
  [./eta9_IC]
    variable = gr29
    type = RandomIC
    min = 0.0
    max = 1.0e-3
    seed = 252345
  [../]
[]

[BCs]
  [./Periodic]
    [./all]
      auto_direction = 'x y'
    [../]
  [../]
[]

[Kernels]
  [./PolycrystalKernel]
  [../]
  [./PolycrystalStoredEnergy]
  [../]
[]


[AuxKernels]
  [./BndsCalc]
    type = BndsCalcAux
    variable = bnds
    execute_on = timestep_end
  [../]
  [./unique_grains]
    type = FeatureFloodCountAux
    variable = unique_grains
    flood_counter = grain_tracker
    execute_on = 'initial timestep_end'
  [../]
  [./var_indices]
    type = FeatureFloodCountAux
    variable = var_indices
    flood_counter = grain_tracker
    execute_on = 'initial timestep_end'
    field_display = VARIABLE_COLORING
  [../]
[]

[Materials]
  [./deformed]
    type = DeformedGrainMaterial
    outputs = exodus
  [../]
[]

[UserObjects]
  [./grain_tracker]
    type = GrainTracker
    threshold = 0.2
    connecting_threshold = 0.08
    compute_op_maps = true
    flood_entity_type = elemental
    execute_on = ' initial timestep_begin'
  #  reserve_op = 10
   # reserve_op_threshold = 0.2
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  # Preconditioned JFNK (default)
  type = Transient
  nl_max_its = 15
  scheme = bdf2
  solve_type = PJFNK
  petsc_options_iname = -pc_type
  petsc_options_value = asm
  l_max_its = 15
  l_tol = 1.0e-3
  nl_rel_tol = 1.0e-8
  start_time = 0.0
  num_steps = 100
  nl_abs_tol = 1e-8
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 0.1e-1
  [../]
[]
[Postprocessors]
  [./gr_25]
    type = ElementIntegralVariablePostprocessor
    variable = gr25
  [../]
[Outputs]
  [./exodus]
  type = Exodus
  interval = 1
  [../]
  [./csv]
  type = CSV
  interval = 1
  [../]
  print_perf_log = true
[]

