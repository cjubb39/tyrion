new_design blargen
set_attr design_dir "" /designs/blargen
set_attr auto_write_models "true" /designs/blargen
define_sim_config -model_dir "./model" /designs/blargen
set_attr source_files [list ../src/blargen.cpp] /designs/blargen
set_attr header_files [list ../src/blargen.h] /designs/blargen
set_attr compile_flags " -w -I../src" /designs/blargen
set_attr top_module_path "blargen" /designs/blargen
set_attr build_flat "true" /designs/blargen
define_clock -name clk -period 20000 -rise 0 -fall 10000
set_attr implementation_target FPGA [get_design]
set_attr fpga_install_path /tools/xilinx/14.6/ISE_DS/ISE/bin/lin64/xst [get_design]
set_attr fpga_target [list Xilinx virtex7 xc7vx485t-2-ffg1761] [get_design]
set_attr verilog_use_indexed_part_select false [get_design]
build
set_attr scheduling_effort "low" /designs/blargen/modules/blargen/behaviors/blargen_beh
set_attr relax_latency "true" /designs/blargen/modules/blargen/behaviors/blargen_beh
schedule -passes 200 -post_optimize none -verbose /designs/blargen/modules/blargen
allocate_registers /designs/blargen/modules/blargen
write_rtl -file ./model/blargen_rtl.v /designs/blargen/modules/blargen
