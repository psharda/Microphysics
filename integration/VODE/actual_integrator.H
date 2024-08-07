#ifndef actual_integrator_H
#define actual_integrator_H

#include <network.H>
#include <burn_type.H>

#include <integrator_data.H>
#include <integrator_setup_strang.H>

#include <vode_type.H>
#include <vode_dvode.H>

template <typename BurnT>
AMREX_GPU_HOST_DEVICE AMREX_INLINE
void actual_integrator (BurnT& state, amrex::Real dt, bool is_retry=false)
{

    constexpr int int_neqs = integrator_neqs<BurnT>();

    auto vode_state = integrator_setup<BurnT, dvode_t<int_neqs>>(state, dt, is_retry);
    auto state_save = integrator_backup(state);

    auto istate = dvode(state, vode_state);

    integrator_cleanup(vode_state, state, istate, state_save, dt);

}

#endif
