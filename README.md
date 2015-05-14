TYRION
======
A hardware-software implementation of Singular Value Decomposition.

Completed for CSEE4840: Embedded Systems Design. Spring 2015.

The directory structure is as follows:
.
├── cargo                 # files to use CARGO Linux simulator
├── device                # dts, driver, etc.
├── docs                  # some documentation
├── lsa                   # LSA-related files
├── svd                   # Files for svd accelerator hardware
│   ├── ctos-sim-unisim   ## post-synthesis simulation
│   ├── quartus           ## files for quartus: qpf, qsys, tcl, etc
│   ├── sim               ## SystemC simulation
│   ├── src               ## SystemC source code
│   ├── syn               ## files for synthesis Cadence C-to-Silicon
│   └── tb                ## testbench files
└── svd-base-impl         ## base implementations
    └── validation        ## verify correctness of chosen C implementation
