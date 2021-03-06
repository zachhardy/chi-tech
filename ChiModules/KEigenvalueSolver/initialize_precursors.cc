#include "k_eigenvalue_solver.h"

#include <ChiMesh/Cell/cell.h>
#include "ChiMath/SpatialDiscretization/FiniteElement/PiecewiseLinear/pwl.h"

#include <chi_log.h>
#include <chi_mpi.h>

extern ChiLog& chi_log;
extern ChiMPI& chi_mpi;

using namespace LinearBoltzmann;

//###################################################################
/**Computes the point wise delayed neutron precursor concentrations.*/
void KEigenvalue::Solver::InitializePrecursors()
{
  if (options.use_precursors)
  {
    precursor_new_local.assign(precursor_new_local.size(), 0.0);

    //======================================== Loop over cells
    for (auto& cell : grid->local_cells)
    {
      //==================== Cell information
      const auto xs_id = matid_to_xs_map[cell.material_id];
      auto& xs = material_xs[xs_id];
      auto& transport_view = cell_transport_views[cell.local_id];

      //============================== Loop over nodes
      const int num_nodes = transport_view.NumNodes();
      for (int i = 0; i < num_nodes; ++i)
      {
        size_t ir = transport_view.MapDOF(i, 0, 0);
        size_t jr = discretization->MapDOFLocal(cell, i, precursor_uk_man, 0, 0);
        double* Nj_newp = &precursor_new_local[jr];

        // Contribute if precursors live on this material
        if (xs->num_precursors > 0)
        {
          //======================================== Loop over precursors
          for (size_t j = 0; j < xs->num_precursors; ++j)
          {

            //======================================== Loop over groups
            for (size_t g = 0; g < groups.size(); ++g)
              precursor_new_local[jr + j] +=
                  xs->precursor_yield[j] / xs->precursor_lambda[j] *
                  xs->nu_delayed_sigma_f[g] * phi_new_local[ir + g] / k_eff;
          }//for precursors
        }//if num_precursors > 0
      } //for node
    }//for cell
  }
}


