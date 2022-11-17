#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

const int MAX_MEM_SIZE  = (1 << 13);

void fetchStage(int *icode, int *ifun, int *rA, int *rB, wordType *valC, wordType *valP) {
    wordType temp = getPC();
    byteType firstByte = getByteFromMemory(temp);
    *icode = ((firstByte & 0xf0) >> 4);
    *ifun = (firstByte & 0x0f);
    byteType secondByte = getByteFromMemory(temp + 1);
    *rA = ((secondByte & 0xf0) >> 4);
    *rB = (secondByte & 0x0f);

    if ((*icode == NOP) || (*icode == HALT)){
        printf("In NOP/HALT fetch");
        *valP = getPC() + 1;
    }
    if (*icode == OPQ){
        *valP = getPC() + 2;
    }
}

void decodeStage(int icode, int rA, int rB, wordType *valA, wordType *valB) {
    *valA = getRegister(rA);
    *valB = getRegister(rB);
}

void executeStage(int icode, int ifun, wordType valA, wordType valB, wordType valC, wordType *valE, bool *Cnd) {
    *valE = valB + valA;
    if (*valE < 0){
        signFlag = 1;
    }
    if (*valE == 0){
        zeroFlag = 1;
    }
    if ((valA < 0 == valB < 0) && (*valE < 0 != valA < 0)){
        overflowFlag = 1;
    }

}

void memoryStage(int icode, wordType valA, wordType valP, wordType valE, wordType *valM) {
    printf("In memoryStage");
}

void writebackStage(int icode, int rA, int rB, wordType valE, wordType valM) {
    setRegister(rB, valE);
}

void pcUpdateStage(int icode, wordType valC, wordType valP, bool Cnd, wordType valM) {
    setPC(valP);
    if (icode == HALT){
        setStatus(STAT_HLT);
    }
}

void stepMachine(int stepMode) {
  /* FETCH STAGE */
  int icode = 0, ifun = 0;
  int rA = 0, rB = 0;
  wordType valC = 0;
  wordType valP = 0;
 
  /* DECODE STAGE */
  wordType valA = 0;
  wordType valB = 0;

  /* EXECUTE STAGE */
  wordType valE = 0;
  bool Cnd = 0;

  /* MEMORY STAGE */
  wordType valM = 0;

  fetchStage(&icode, &ifun, &rA, &rB, &valC, &valP);
  applyStageStepMode(stepMode, "Fetch", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

  decodeStage(icode, rA, rB, &valA, &valB);
  applyStageStepMode(stepMode, "Decode", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  executeStage(icode, ifun, valA, valB, valC, &valE, &Cnd);
  applyStageStepMode(stepMode, "Execute", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  memoryStage(icode, valA, valP, valE, &valM);
  applyStageStepMode(stepMode, "Memory", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  writebackStage(icode, rA, rB, valE, valM);
  applyStageStepMode(stepMode, "Writeback", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);
  
  pcUpdateStage(icode, valC, valP, Cnd, valM);
  applyStageStepMode(stepMode, "PC update", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

  incrementCycleCounter();
}

/** 
 * main
 * */
int main(int argc, char **argv) {
  int stepMode = 0;
  FILE *input = parseCommandLine(argc, argv, &stepMode);

  initializeMemory(MAX_MEM_SIZE);
  initializeRegisters();
  loadMemory(input);

  applyStepMode(stepMode);
  while (getStatus() != STAT_HLT) {
    stepMachine(stepMode);
    applyStepMode(stepMode);
#ifdef DEBUG
    printMachineState();
    printf("\n");
#endif
  }
  printMachineState();
  return 0;
}