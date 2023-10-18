# RISC-V-Simulator

Usage : rv32i [-d] [ -i] [-r] [- z] [-l exec - limit ] [-m hex - mem - size ] infile  
-d show disassembly before program execution  
-i show instruction printing during execution  
-l maximum number of instructions to exec  
-m specify memory size ( default = 0 x100 )  
-r show register printing during execution  
-z show a dump of the regs & memory after simulation  

Any of the command-line arguments may appear in any order and:  
  
On their own:  
-d -i -r -z -l 1234 -m efc0  
  
Or in groups:  
-dirz -l1234 -mefc0  
