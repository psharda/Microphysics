#include <AMReX_PlotFileUtil.H>
#include <AMReX_ParmParse.H>
#include <AMReX_Print.H>

#include <AMReX_Geometry.H>
#include <AMReX_MultiFab.H>
#include <AMReX_BCRec.H>


using namespace amrex;

#include "test_screen.H"
#include "test_screen_F.H"
#include "AMReX_buildInfo.H"

#include <network.H>
#include <eos.H>
#include <screen.H>
#include <variables.H>

int main (int argc, char* argv[])
{
    amrex::Initialize(argc, argv);

    main_main();

    amrex::Finalize();
    return 0;
}

void main_main ()
{

    // AMREX_SPACEDIM: number of dimensions
    int n_cell, max_grid_size;
    Vector<int> bc_lo(AMREX_SPACEDIM,0);
    Vector<int> bc_hi(AMREX_SPACEDIM,0);

    // inputs parameters
    {
        // ParmParse is way of reading inputs from the inputs file
        ParmParse pp;

        // We need to get n_cell from the inputs file - this is the
        // number of cells on each side of a square (or cubic) domain.
        pp.get("n_cell", n_cell);

        // The domain is broken into boxes of size max_grid_size
        max_grid_size = 32;
        pp.query("max_grid_size", max_grid_size);

    }

    Vector<int> is_periodic(AMREX_SPACEDIM,0);
    for (int idim=0; idim < AMREX_SPACEDIM; ++idim) {
      is_periodic[idim] = 1;
    }

    // make BoxArray and Geometry
    BoxArray ba;
    Geometry geom;
    {
        IntVect dom_lo(AMREX_D_DECL(       0,        0,        0));
        IntVect dom_hi(AMREX_D_DECL(n_cell-1, n_cell-1, n_cell-1));
        Box domain(dom_lo, dom_hi);

        // Initialize the boxarray "ba" from the single box "bx"
        ba.define(domain);

        // Break up boxarray "ba" into chunks no larger than
        // "max_grid_size" along a direction
        ba.maxSize(max_grid_size);

        // This defines the physical box, [0, 1] in each direction.
        RealBox real_box({AMREX_D_DECL(0.0, 0.0, 0.0)},
                         {AMREX_D_DECL(1.0, 1.0, 1.0)});

        // This defines a Geometry object
        geom.define(domain, &real_box,
                    CoordSys::cartesian, is_periodic.data());
    }

    // Nghost = number of ghost cells for each array
    int Nghost = 0;

    // do the runtime parameter initializations and microphysics inits
    if (ParallelDescriptor::IOProcessor()) {
      std::cout << "reading extern runtime parameters ..." << std::endl;
    }

    ParmParse ppa("amr");

    std::string probin_file = "probin";

    ppa.query("probin_file", probin_file);

    const int probin_file_length = probin_file.length();
    Vector<int> probin_file_name(probin_file_length);

    for (int i = 0; i < probin_file_length; i++)
      probin_file_name[i] = probin_file[i];

    init_unit_test(probin_file_name.dataPtr(), &probin_file_length);

    init_extern_parameters();

    eos_init();

    screening_init();

    auto vars = init_variables();

    // time = starting time in the simulation
    Real time = 0.0;

    // How Boxes are distrubuted among MPI processes
    DistributionMapping dm(ba);

    // we allocate our main multifabs
    MultiFab state(ba, dm, vars.n_plot_comps, Nghost);

    // Initialize the state to zero; we will fill
    // it in below in do_eos.
    state.setVal(0.0);

    // What time is it now?  We'll use this to compute total run time.
    Real strt_time = ParallelDescriptor::second();

    const int ih1 = network_spec_index("hydrogen-1");
    const int ihe4 = network_spec_index("helium-4");


    // initialize the screening factors
    int jscr = 0;
    add_screening_factor(jscr, zion[ihe4],aion[ihe4],zion[ihe4],aion[ihe4]);

    jscr++;
    add_screening_factor(jscr, zion[ihe4],aion[ihe4],4.0e0_rt,8.0e0_rt);

    jscr++;
    add_screening_factor(jscr, zion[ic12],aion[ic12],zion[ihe4],aion[ihe4]);

    jscr++;
    add_screening_factor(jscr, zion[ic12],aion[ic12],zion[ic12],aion[ic12]);

    jscr++;
    add_screening_factor(jscr, zion[ic12],aion[ic12],zion[io16],aion[io16]);

    jscr++;
    add_screening_factor(jscr, zion[io16],aion[io16],zion[io16],aion[io16]);

    jscr++;
    add_screening_factor(jscr, zion[io16],aion[io16],zion[ihe4],aion[ihe4]);

    jscr++;
    add_screening_factor(jscr, zion[ine20],aion[ine20],zion[ihe4],aion[ihe4]);

    jscr++;
    add_screening_factor(jscr, zion[img24],aion[img24],zion[ihe4],aion[ihe4]);

    jscr++;
    add_screening_factor(jscr, 13.0e0_rt,27.0e0_rt,1.0e0_rt,1.0e0_rt);

    jscr++;
    add_screening_factor(jscr, zion[isi28],aion[isi28],zion[ihe4],aion[ihe4]);

    jscr++;
    add_screening_factor(jscr, 15.0e0_rt,31.0e0_rt,1.0e0_rt,1.0e0_rt);

    jscr++;
    add_screening_factor(jscr, zion[is32],aion[is32],zion[ihe4],aion[ihe4]);

    jscr++;
    add_screening_factor(jscr, 17.0e0_rt,35.0e0_rt,1.0e0_rt,1.0e0_rt);

    jscr++;
    add_screening_factor(jscr, zion[iar36],aion[iar36],zion[ihe4],aion[ihe4]);

    jscr++;
    add_screening_factor(jscr, 19.0e0_rt,39.0e0_rt,1.0e0_rt,1.0e0_rt);

    jscr++;
    add_screening_factor(jscr, zion[ica40],aion[ica40],zion[ihe4],aion[ihe4]);

    jscr++;
    add_screening_factor(jscr, 21.0e0_rt,43.0e0_rt,1.0e0_rt,1.0e0_rt);

    jscr++;
    add_screening_factor(jscr, zion[iti44],aion[iti44],zion[ihe4],aion[ihe4]);

    jscr++;
    add_screening_factor(jscr, 23.0e0_rt,47.0e0_rt,1.0e0_rt,1.0e0_rt);

    jscr++;
    add_screening_factor(jscr, zion[icr48],aion[icr48],zion[ihe4],aion[ihe4]);

    jscr++;
    add_screening_factor(jscr, 25.0e0_rt,51.0e0_rt,1.0e0_rt,1.0e0_rt);

    jscr++;
    add_screening_factor(jscr, zion[ife52],aion[ife52],zion[ihe4],aion[ihe4]);

    jscr++;
    add_screening_factor(jscr, 27.0e0_rt,55.0e0_rt,1.0e0_rt,1.0e0_rt);

    jscr++;
    add_screening_factor(jscr, zion[ife54],aion[ife54),1.0e0_rt,1.0e0_rt);

    jscr++;
    add_screening_factor(jscr, zion[ife54],aion[ife54],zion[ihe4],aion[ihe4]);

    jscr++;
    add_screening_factor(jscr, zion[ife56],aion[ife56),1.0e0_rt,1.0e0_rt);

    jscr++;
    add_screening_factor(jscr, 1.0e0_rt,2.0e0_rt,zion[ih1],aion[ih1]);

    jscr++;
    add_screening_factor(jscr, zion[ih1],aion[ih1],zion[ih1],aion[ih1]);

    jscr++;
    add_screening_factor(jscr, zion[ihe3],aion[ihe3],zion[ihe3],aion[ihe3]);

    jscr++;
    add_screening_factor(jscr, zion[ihe3],aion[ihe3],zion[ihe4],aion[ihe4]);

    jscr++;
    add_screening_factor(jscr, zion[ic12],aion[ic12],zion[ih1],aion[ih1]);

    jscr++;
    add_screening_factor(jscr, zion[in14],aion[in14],zion[ih1],aion[ih1]);

    jscr++;
    add_screening_factor(jscr, zion[io16],aion[io16],zion[ih1],aion[ih1]);

    jscr++;
    add_screening_factor(jscr, zion[in14],aion[in14],zion[ihe4],aion[ihe4]);


    Real dlogrho = 0.0e0_rt;
    Real dlogT   = 0.0e0_rt;
    Real dmetal  = 0.0e0_rt;

    if (n_cell > 1) {
        dlogrho = (log10(dens_max) - log10(dens_min))/(n_cell - 1);
        dlogT   = (log10(temp_max) - log10(temp_min))/(n_cell - 1);
        dmetal  = (metalicity_max  - 0.0)/(n_cell - 1);
    }

    // Initialize the state and compute the different thermodynamics
    // by inverting the EOS
    for ( MFIter mfi(state); mfi.isValid(); ++mfi )
    {
      const Box& bx = mfi.validbox();

      Array4<Real> const sp = state.array(mfi);

      AMREX_PARALLEL_FOR_3D(bx, i, j, k,
      {

        // set the composition -- approximately solar
        Real metalicity = 0.0 + static_cast<Real> (k) * dmetal;

        eos_t eos_state;

        for (int n = 0; n < NumSpec; n++) {
          eos_state.xn[n] = metalicity/(NumSpec - 2);
        }
        eos_state.xn[ih1] = 0.75 - 0.5*metalicity;
        eos_state.xn[ihe4] = 0.25 - 0.5*metalicity;

        Real temp_zone = std::pow(10.0, log10(temp_min) + static_cast<Real>(j)*dlogT);
        eos_state.T = temp_zone;

        Real dens_zone = std::pow(10.0, log10(dens_min) + static_cast<Real>(i)*dlogrho);
        eos_state.rho = dens_zone;

        // store default state
        sp(i, j, k, vars.irho) = eos_state.rho;
        sp(i, j, k, vars.itemp) = eos_state.T;
        for (int n = 0; n < NumSpec; n++) {
          sp(i, j, k, vars.ispec+n) = eos_state.xn[n];
        }

        // call the EOS using rho, T
        eos(eos_input_rt, eos_state);

        conductivity(eos_state);

        sp(i, j, k, vars.ih) = eos_state.h;
        sp(i, j, k, vars.ie) = eos_state.e;
        sp(i, j, k, vars.ip) = eos_state.p;
        sp(i, j, k, vars.is) = eos_state.s;

        sp(i, j, k, vars.iconductivity) = eos_state.conductivity;

      });

    }

    // Call the timer again and compute the maximum difference between
    // the start time and stop time over all processors
    Real stop_time = ParallelDescriptor::second() - strt_time;
    const int IOProc = ParallelDescriptor::IOProcessorNumber();
    ParallelDescriptor::ReduceRealMax(stop_time, IOProc);


    std::string name = "test_conductivity_C.";

    // Write a plotfile
    WriteSingleLevelPlotfile(name + cond_name, state, vars.names, geom, time, 0);

    // Tell the I/O Processor to write out the "run time"
    amrex::Print() << "Run time = " << stop_time << std::endl;

}
