if { [file exist setup_locals.tcl] } {
	source setup_locals.tcl
} else {
	puts "Cannot find \"setup_locals.tcl\"."
	exit
}

if {[get_design] != "" } then {
	close_design
}

new_design flash
set_attr design_dir "" /designs/flash
set_attr auto_write_models "true" /designs/flash
define_sim_config -model_dir "./model" /designs/flash
set_attr source_files [list ../src/flash.cpp] /designs/flash
set_attr header_files [list ../src/flash.h ../src/flash_sched.h] /designs/flash
set_attr compile_flags " -w -I../src/ -I/opt/zynq-math/src/ -DTASK_QUEUE_SIZE=128 -DRUN_QUEUE_SIZE=64 -DWAIT_PER_TICK=128" /designs/flash
set_attr top_module_path "flash" /designs/flash
set_attr build_flat "true" /designs/flash
define_clock -name clock_0 -period 20000 -rise 0 -fall 10000
define_sim_config -makefile_name ../ctos_sim_unisim/Makefile -model_dir flash -simulator_args "-I../src/ -I/opt/zynq-math/src/ -I../tb/ -sc_main -I../syn/flash/ -D__CTOS__ -DTASK_QUEUE_SIZE=128 -DRUN_QUEUE_SIZE=64 -DWAIT_PER_TICK=128 -DTASKS_TO_SEND=128 -DTASKS_TO_READ=512 " -testbench_files "../tb/flash_tb.cpp ../tb/sc_main.cpp" -testbench_kind self_checking -success_msg ""
define_synth_config -run_dir "run_synth_gates" -standard_flow "default_synthesis_flow" -config_file_name ""

# implmentation
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

# from ctosgui but not from debayer
#write_sim -type verilog -suffix _post_build -birthday -dir ./model /designs/flash/modules/flash.
#write_wrapper -o ./model/flash_ctos_wrapper.h /designs/flash/modules/flash.

set_attr default_scheduling_effort low [get_design]

# loops
#break_combinational_loop /designs/flash/modules/flash/behaviors/get_next_task/nodes/GET_NEXT_TASK_L1_for_begin
break_combinational_loop /designs/flash/modules/flash/behaviors/remove_task_from_run_queue/nodes/REMOVE_FROM_RL_for_begin
break_combinational_loop /designs/flash/modules/flash/behaviors/add_task_to_run_queue/nodes/ADD_TASK_RL_ADD_LOOP_for_begin
break_combinational_loop /designs/flash/modules/flash/behaviors/add_task_to_run_queue/nodes/ADD_TASK_RL_EXIST_LOOP_for_begin
break_combinational_loop /designs/flash/modules/flash/behaviors/udiv_func_64__64__64__64__64__64_/nodes/DIVIDE_LOOP_for_begin
break_combinational_loop /designs/flash/modules/flash/behaviors/lookup_process/nodes/LOOKUP_PROCESS_LOOP_for_begin
break_combinational_loop /designs/flash/modules/flash/behaviors/find_empty_slot/nodes/FIND_EMPTY_LOOP_for_begin
break_combinational_loop /designs/flash/modules/flash/behaviors/flash_initialize/nodes/INIT_PROCESS_LIST_for_begin
break_combinational_loop /designs/flash/modules/flash/behaviors/flash_initialize/nodes/INIT_RUN_LIST_for_begin
unroll_loop /designs/flash/modules/flash/behaviors/flash_tick/nodes/for_ln50

# functions
#inline /designs/flash/modules/flash/behaviors/remove_task_from_run_queue /designs/flash/modules/flash/behaviors/lookup_process /designs/flash/modules/flash/behaviors/get_next_task /designs/flash/modules/flash/behaviors/find_empty_slot /designs/flash/modules/flash/behaviors/calculate_virtual_runtime /designs/flash/modules/flash/behaviors/add_task_to_run_queue
inline /designs/flash/modules/flash/behaviors/udiv_func_64__64__64__64__64__64_ /designs/flash/modules/flash/behaviors/remove_task_from_run_queue /designs/flash/modules/flash/behaviors/lookup_process /designs/flash/modules/flash/behaviors/get_next_task /designs/flash/modules/flash/behaviors/find_empty_slot /designs/flash/modules/flash/behaviors/calculate_virtual_runtime /designs/flash/modules/flash/behaviors/add_task_to_run_queue

# memory
#allocate_prototype_memory /designs/flash/modules/flash/arrays/runnable_list
#allocate_prototype_memory /designs/flash/modules/flash/arrays/process_list_vr
#allocate_prototype_memory /designs/flash/modules/flash/arrays/process_list_state
#allocate_prototype_memory /designs/flash/modules/flash/arrays/process_list_start_time
#allocate_prototype_memory /designs/flash/modules/flash/arrays/process_list_pri
#allocate_prototype_memory /designs/flash/modules/flash/arrays/process_list_pid
#allocate_prototype_memory /designs/flash/modules/flash/arrays/process_list_active

#allocate_builtin_ram /designs/flash/modules/flash/arrays/runnable_list
#allocate_builtin_ram /designs/flash/modules/flash/arrays/process_list_vr
#allocate_builtin_ram /designs/flash/modules/flash/arrays/process_list_state
#allocate_builtin_ram /designs/flash/modules/flash/arrays/process_list_start_time
#allocate_builtin_ram /designs/flash/modules/flash/arrays/process_list_pri
#allocate_builtin_ram /designs/flash/modules/flash/arrays/process_list_pid
#allocate_builtin_ram /designs/flash/modules/flash/arrays/process_list_active

allocate_builtin_ram -sync_read -read_latency 1 -clock /designs/flash/modules/flash/nets/clk /designs/flash/modules/flash/arrays/runnable_list
allocate_builtin_ram -sync_read -read_latency 1 -clock /designs/flash/modules/flash/nets/clk /designs/flash/modules/flash/arrays/process_list_vr
allocate_builtin_ram -sync_read -read_latency 1 -clock /designs/flash/modules/flash/nets/clk /designs/flash/modules/flash/arrays/process_list_state
allocate_builtin_ram -sync_read -read_latency 1 -clock /designs/flash/modules/flash/nets/clk /designs/flash/modules/flash/arrays/process_list_start_time
allocate_builtin_ram -sync_read -read_latency 1 -clock /designs/flash/modules/flash/nets/clk /designs/flash/modules/flash/arrays/process_list_pri
allocate_builtin_ram -sync_read -read_latency 1 -clock /designs/flash/modules/flash/nets/clk /designs/flash/modules/flash/arrays/process_list_pid
allocate_builtin_ram -sync_read -read_latency 1 -clock /designs/flash/modules/flash/nets/clk /designs/flash/modules/flash/arrays/process_list_active

#allocate_builtin_ram -read_interfaces 2 -write_interfaces 2 -sync_read -read_latency 1 -clock /designs/flash/modules/flash/nets/clk /designs/flash/modules/flash/arrays/runnable_list
#allocate_builtin_ram -read_interfaces 2 -write_interfaces 2 -sync_read -read_latency 1 -clock /designs/flash/modules/flash/nets/clk /designs/flash/modules/flash/arrays/process_list_vr
#allocate_builtin_ram -read_interfaces 1 -write_interfaces 1 -sync_read -read_latency 1 -clock /designs/flash/modules/flash/nets/clk /designs/flash/modules/flash/arrays/process_list_state
#allocate_builtin_ram -read_interfaces 2 -write_interfaces 2 -sync_read -read_latency 1 -clock /designs/flash/modules/flash/nets/clk /designs/flash/modules/flash/arrays/process_list_start_time
#allocate_builtin_ram -read_interfaces 2 -write_interfaces 1 -sync_read -read_latency 1 -clock /designs/flash/modules/flash/nets/clk /designs/flash/modules/flash/arrays/process_list_pri
#allocate_builtin_ram -read_interfaces 2 -write_interfaces 1 -sync_read -read_latency 1 -clock /designs/flash/modules/flash/nets/clk /designs/flash/modules/flash/arrays/process_list_pid
#allocate_builtin_ram -read_interfaces 1 -write_interfaces 2 -sync_read -read_latency 1 -clock /designs/flash/modules/flash/nets/clk /designs/flash/modules/flash/arrays/process_list_active

#allocate_builtin_ram -read_interfaces 2 -write_interfaces 2 /designs/flash/modules/flash/arrays/runnable_list
#allocate_builtin_ram -read_interfaces 2 -write_interfaces 2 /designs/flash/modules/flash/arrays/process_list_vr
#allocate_builtin_ram -read_interfaces 1 -write_interfaces 1 /designs/flash/modules/flash/arrays/process_list_state
#allocate_builtin_ram -read_interfaces 2 -write_interfaces 2 /designs/flash/modules/flash/arrays/process_list_start_time
#allocate_builtin_ram -read_interfaces 2 -write_interfaces 1 /designs/flash/modules/flash/arrays/process_list_pri
#allocate_builtin_ram -read_interfaces 2 -write_interfaces 1 /designs/flash/modules/flash/arrays/process_list_pid
#allocate_builtin_ram -read_interfaces 1 -write_interfaces 2 /designs/flash/modules/flash/arrays/process_list_active

# scheduling
set_attr relax_latency "true" /designs/flash/modules/flash/behaviors/flash_initialize
set_attr relax_latency "true" /designs/flash/modules/flash/behaviors/flash_tick
set_attr relax_latency "true" /designs/flash/modules/flash/behaviors/flash_process_change
set_attr relax_latency "true" /designs/flash/modules/flash/behaviors/flash_schedule
set_attr relax_latency "true" /designs/flash/modules/flash/behaviors/flash_timer
schedule -passes 200 -post_optimize none -verbose /designs/flash/modules/flash

allocate_registers /designs/flash

#write rtl
write_sim -type verilog -suffix _final -dir ./flash /designs/flash/modules/flash
write_rtl  -file ./flash/flash\_rtl.v /designs/flash/modules/flash

#Report time and area
report_timing > timing.txt
report_area > area.txt


