# **LC-2K Assembler**

### **Project Task**
- Write a program to take an assembly-language program and translate it into machine language
- Translate symbolic names for addresses into numeric values

<br/>

### **Compilation Method**

The file **assemble.c** is compiled using GCC with the following input at command prompt.

    gcc -o assemble assemble.c

<br/>

### **Test Cases**

To run each test case for the assembler, the respective inputs are entered as shown at command prompt.

Case 1: Normal (No error)

    ./assemble test-ok.ac test.mc

Case 2: Label error

    ./assemble test-err-label.ac test.mc

Case 3: Duplicate labels

    ./assemble test-err-label-dup.ac test.mc

Case 4: Invalid opcode
    
    ./assemble test-err-opcode.ac test.mc

Case 5: Fill overflow

    ./assemble test-err-fill.ac test.mc

Case 6: Invalid register

    ./assemble test-err-reg.ac test.mc

Case 7: Invalid destination register
    
    ./assemble test-err-dest.ac test.mc

Case 8: Empty/Inexistent source file

    ./assemble test-err-no-file.ac test.mc