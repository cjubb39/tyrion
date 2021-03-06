if { [file exist setup_locals.tcl] } {
	source setup_locals.tcl
} else {
	puts "Cannot find \"setup_locals.tcl\"."
	exit
}

if {[get_design] != "" } then {
	close_design
}

new_design tyrion
set_attr design_dir "" /designs/tyrion
set_attr auto_write_models "true" /designs/tyrion
define_sim_config -model_dir "./tyrion" /designs/tyrion
set_attr source_files [list ../src/svd.cpp ../src/svd_wrapper.cpp] /designs/tyrion
set_attr header_files [list ../src/svd.h ../src/svd_wrapper.h ../src/svd_ctos_funcs.h ../src/svd_data.h] /designs/tyrion
set_attr compile_flags " -w -I../src/ -I/opt/zynq-math/src/" /designs/tyrion
set_attr top_module_path "svd_wrapper" /designs/tyrion
set_attr build_flat "true" /designs/tyrion
define_clock -name clk -period 20000 -rise 0 -fall 10000
define_sim_config -makefile_name ../ctos_sim_unisim/Makefile -model_dir tyrion -simulator_args "-I../src/ -I/opt/zynq-math/src/" -testbench_files "../tb/svd_tb.cpp ../tb/sc_main.cpp" -testbench_kind self_checking -success_msg ""
define_synth_config -run_dir "run_synth_gates" -standard_flow "default_synthesis_flow" -config_file_name ""

# implementation
#set_attr implementation_target FPGA [get_design]
#set_attr fpga_install_path /opt/altera/quartus/14.1/quartus/linux64/quartus_sta [get_design]
#set_attr fpga_target [list Altera Cyclone2 EP2C35F672C6] [get_design]
set_attr implementation_target FPGA [get_design]
set_attr fpga_install_path /tools/xilinx/14.6/ISE_DS/ISE/bin/lin64/xst [get_design]
set_attr fpga_target [list Xilinx virtex7 xc7vx485t-2-ffg1761] [get_design]
set_attr verilog_use_indexed_part_select false [get_design]
set_attr reset_registers internal_and_outputs [get_design]

build
write_sim_makefile -overwrite

set_attr default_scheduling_effort high [get_design]

# loops
break_combinational_loop /designs/tyrion/modules/svd_wrapper/behaviors/copyMatrix/nodes/COPY_MATRIX_INNER_for_begin
break_combinational_loop /designs/tyrion/modules/svd_wrapper/behaviors/cordic_func_80__64__24_/nodes/CORDIC_LOOP_for_begin
break_combinational_loop /designs/tyrion/modules/svd_wrapper/behaviors/findLargestElement/nodes/FLE_POPULATE_INNER_for_begin
break_combinational_loop /designs/tyrion/modules/svd_wrapper/behaviors/identify/nodes/IDENTITY_INNER_for_begin
break_combinational_loop /designs/tyrion/modules/svd_wrapper/behaviors/svd_wrapper_dut_load_input/nodes/LOAD_OUTER_for_begin
break_combinational_loop /designs/tyrion/modules/svd_wrapper/behaviors/multiply/nodes/MULTIPLY_MATRIX_OUTER_for_begin
break_combinational_loop /designs/tyrion/modules/svd_wrapper/behaviors/svd_wrapper_dut_store_output/nodes/OUTPUT_S_OUTER_for_begin
break_combinational_loop /designs/tyrion/modules/svd_wrapper/behaviors/svd_wrapper_dut_store_output/nodes/OUTPUT_U_OUTER_for_begin
break_combinational_loop /designs/tyrion/modules/svd_wrapper/behaviors/svd_wrapper_dut_store_output/nodes/OUTPUT_V_OUTER_for_begin
break_combinational_loop /designs/tyrion/modules/svd_wrapper/behaviors/reorder/nodes/REORDER_FIND_NEG_for_begin
break_combinational_loop /designs/tyrion/modules/svd_wrapper/behaviors/reorder/nodes/REORDER_INNER_for_begin
break_combinational_loop /designs/tyrion/modules/svd_wrapper/behaviors/swapRows/nodes/SWAP_ROW_for_begin
break_combinational_loop /designs/tyrion/modules/svd_wrapper/behaviors/transpose/nodes/TRANSPOSE_INNER_for_begin
break_combinational_loop /designs/tyrion/modules/svd_wrapper/behaviors/transpose/nodes/TRANSPOSE_OUTER_for_begin

# initial memory
flatten_array  /designs/tyrion/modules/svd_wrapper/arrays/a22 /designs/tyrion/modules/svd_wrapper/arrays/a21 /designs/tyrion/modules/svd_wrapper/arrays/a12 /designs/tyrion/modules/svd_wrapper/arrays/a11

# functions
inline /designs/tyrion/modules/svd_wrapper/behaviors/transpose /designs/tyrion/modules/svd_wrapper/behaviors/swapRows /designs/tyrion/modules/svd_wrapper/behaviors/rotate /designs/tyrion/modules/svd_wrapper/behaviors/reorder /designs/tyrion/modules/svd_wrapper/behaviors/multiply /designs/tyrion/modules/svd_wrapper/behaviors/jacobi /designs/tyrion/modules/svd_wrapper/behaviors/identify /designs/tyrion/modules/svd_wrapper/behaviors/fp_abs /designs/tyrion/modules/svd_wrapper/behaviors/findLargestElement /designs/tyrion/modules/svd_wrapper/behaviors/cos_sin_cordic_func_80__64__24_ /designs/tyrion/modules/svd_wrapper/behaviors/cordic_func_80__64__24_ /designs/tyrion/modules/svd_wrapper/behaviors/copyMatrix /designs/tyrion/modules/svd_wrapper/behaviors/atan2_cordic_func_80__64__24_ /designs/tyrion/modules/svd_wrapper/behaviors/abs_64__24_


# memory
#allocate_prototype_memory -interface_types {rw r } -clock /designs/tyrion/modules/svd_wrapper/nets/clk /designs/tyrion/modules/svd_wrapper/arrays/dut_v_m_mant
#allocate_prototype_memory -interface_types {rw } -clock /designs/tyrion/modules/svd_wrapper/nets/clk /designs/tyrion/modules/svd_wrapper/arrays/dut_Ui_m_mant
#allocate_prototype_memory -interface_types {r w } -clock /designs/tyrion/modules/svd_wrapper/nets/clk /designs/tyrion/modules/svd_wrapper/arrays/dut_matrix_in_m_mant
#allocate_prototype_memory -interface_types {rw r } -clock /designs/tyrion/modules/svd_wrapper/nets/clk /designs/tyrion/modules/svd_wrapper/arrays/dut_s_m_mant
#allocate_prototype_memory -interface_types {rw } -clock /designs/tyrion/modules/svd_wrapper/nets/clk /designs/tyrion/modules/svd_wrapper/arrays/dut_tempMatrix_m_mant
#allocate_prototype_memory -interface_types {rw r } -clock /designs/tyrion/modules/svd_wrapper/nets/clk /designs/tyrion/modules/svd_wrapper/arrays/dut_u_m_mant

allocate_prototype_memory /designs/tyrion/modules/svd_wrapper/arrays/dut_v_m_mant
allocate_prototype_memory /designs/tyrion/modules/svd_wrapper/arrays/dut_Ui_m_mant
allocate_prototype_memory /designs/tyrion/modules/svd_wrapper/arrays/dut_matrix_in_m_mant
allocate_prototype_memory /designs/tyrion/modules/svd_wrapper/arrays/dut_s_m_mant
allocate_prototype_memory /designs/tyrion/modules/svd_wrapper/arrays/dut_tempMatrix_m_mant
allocate_prototype_memory /designs/tyrion/modules/svd_wrapper/arrays/dut_u_m_mant

# schedule
set_attr relax_latency "true" /designs/tyrion/modules/svd_wrapper/behaviors/svd_wrapper_config_svd
set_attr relax_latency "true" /designs/tyrion/modules/svd_wrapper/behaviors/svd_wrapper_handle_irq
set_attr relax_latency "true" /designs/tyrion/modules/svd_wrapper/behaviors/svd_wrapper_handle_input
set_attr relax_latency "true" /designs/tyrion/modules/svd_wrapper/behaviors/svd_wrapper_handle_output
set_attr relax_latency "true" /designs/tyrion/modules/svd_wrapper/behaviors/svd_wrapper_dut_config_svd
set_attr relax_latency "true" /designs/tyrion/modules/svd_wrapper/behaviors/svd_wrapper_dut_load_input
set_attr relax_latency "true" /designs/tyrion/modules/svd_wrapper/behaviors/svd_wrapper_dut_store_output
set_attr relax_latency "true" /designs/tyrion/modules/svd_wrapper/behaviors/svd_wrapper_dut_process_svd
schedule -passes 200 -verbose /designs/tyrion/modules/svd_wrapper

allocate_registers /designs/tyrion/modules/svd_wrapper

write_sim -type verilog -suffix _final -dir ./tyrion /designs/tyrion/modules/svd_wrapper
write_rtl -file ./tyrion/tyrion_rtl.v /designs/tyrion/modules/svd_wrapper

report_timing > timing.txt
report_area > area.txt

