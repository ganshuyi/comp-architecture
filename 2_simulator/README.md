# **LC-2K Behavioral Simulator**

### **Project Task**
- Write a program that can simulate any legal LC-2K machine-code program
- Input: Machine-code file created from assembler

<br/>

### **Compilation Method**

The file **simulate.c** is compiled using GCC with the following input at command prompt.
    
    gcc -o simulate simulate.c

<br/>

### **Test Cases**

To run each test case for the simulator, the respective inputs are entered as shown at command prompt.

Case 1: Normal (No error)

    ./simulate test-mc-ok.mc > output

Case 2: Invalid source file

    ./simulate test-mc-err-no-file.mc > output

Case 3: Invalid line in source file

    ./simulate test-mc-err-line.mc > output

Case 4: Out-of-range offset (beq)

    ./simulate test-mc-err-offset.mc > output

Case 5: Invalid destination register 

    ./simulate test-mc-err-reg.mc > output

