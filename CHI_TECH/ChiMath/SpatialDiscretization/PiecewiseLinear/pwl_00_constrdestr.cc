#include "pwl.h"

//###################################################################
/**Constructor.*/
SpatialDiscretization_PWL::
  SpatialDiscretization_PWL(chi_mesh::MeshContinuumPtr in_grid) :
  SpatialDiscretization(0, in_grid,
                        SDMType::PIECEWISE_LINEAR_DISCONTINUOUS),
  line_quad_order_second(chi_math::QuadratureOrder::SECOND),
  tri_quad_order_second(chi_math::QuadratureOrder::SECOND),
  tet_quad_order_second(chi_math::QuadratureOrder::SECOND)
{
  mapping_initialized = false;
}

