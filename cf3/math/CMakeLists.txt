coolfluid_find_orphan_files()

list( APPEND coolfluid_math_files
  LibMath.cpp
  LibMath.hpp
  BoostMath.hpp
  Checks.hpp
  Consts.hpp
  Defs.hpp
  FindMinimum.hpp
  FloatingPoint.hpp
  Functions.hpp
  Integrate.hpp
  MatrixTypes.hpp
  MathExceptions.hpp
  MathExceptions.cpp
  VariableManager.hpp
  VariableManager.cpp
  VariablesDescriptor.hpp
  VariablesDescriptor.cpp
  VectorialFunction.hpp
  VectorialFunction.cpp
)

# lssinterface provide connection with linear system solvers
add_subdirectory( LSS )

list( APPEND coolfluid_math_cflibs coolfluid_fparser coolfluid_common )

set( coolfluid_math_kernellib TRUE )

coolfluid_add_library( coolfluid_math )

