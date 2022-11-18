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
        //printf("In NOP/HALT fetch");
        *valP = getPC() + 1;
    }

    if (*icode == OPQ){
        *valP = getPC() + 2;
    }

    if (*icode == RRMOVQ){
        *valP = getPC() + 2;
    }

    if (*icode == PUSHQ){
        *valP = getPC() + 2;
    }

    if (*icode == POPQ){
        *valP = getPC() + 2;
    }

    if ((*icode == IRMOVQ) || (*icode == RMMOVQ) || (*icode == MRMOVQ)){
        wordType thirdWord = getWordFromMemory(temp + 2);
        *valC = thirdWord;
        *valP = getPC() + 10;
    }

    if ((*icode == JXX) || (*icode == CALL)){
        wordType secondWord = getWordFromMemory(temp + 1);
        *valC = secondWord;
        *valP = getPC() + 9;
    }

    if (*icode == RET){
        *valP = getPC() + 1;
    }

    if (*icode == CMOVXX){
        *valP = getPC() + 2;
    }
}

void decodeStage(int icode, int rA, int rB, wordType *valA, wordType *valB) {
    if (icode == OPQ) {
        *valA = getRegister(rA);
        *valB = getRegister(rB);
    }

    if (icode == RRMOVQ) {
        *valA = getRegister(rA);
    }

    if (icode == RMMOVQ) {
        *valA = getRegister(rA);
        *valB = getRegister(rB);
    }

    if (icode == MRMOVQ) {
        *valB = getRegister(rB);
    }

    if (icode == PUSHQ) {
        *valA = getRegister(rA);
        *valB = getRegister(RSP);
    }

    if (icode == POPQ) {
        *valA = getRegister(RSP);
        *valB = getRegister(RSP);
    }

    if (icode == CALL) {
        *valB = getRegister(RSP);
    }

    if (icode == RET) {
        *valA = getRegister(RSP);
        *valB = getRegister(RSP);
    }

    if (icode == CMOVXX) {
        *valA = getRegister(rA);
    }
}

void executeStage(int icode, int ifun, wordType valA, wordType valB, wordType valC, wordType *valE, bool *Cnd) {

    if (icode == OPQ){
        if (ifun == ADD){
            *valE = (valB + valA);
        }
        if (ifun == SUB){
            *valE = (valB - valA);
        }
        if (ifun == AND){
            *valE = (valB & valA);
        }
        if (ifun == XOR){
            *valE = (valB ^ valA);
        }
        //Flag checks for OPQ
        if (*valE < 0){
            signFlag = 1;
        }
        else {
            signFlag = 0;
        }

        if (*valE == 0){
            zeroFlag = 1;
        }
        else {
            zeroFlag = 0;
        }

        if (ifun == ADD) {
            if (((valA < 0) == (valB < 0)) && ((*valE < 0) != (valA < 0))) {
                overflowFlag = 1;
            }
            else {
                overflowFlag = 0;
            }
        }
        if (ifun == SUB) {
            if (((-valA > 0) == (valB > 0)) && ((*valE < 0) != (-valA < 0))) {
                overflowFlag = 1;
            }
            else {
                overflowFlag = 0;
            }
        }
    }

    if (icode == RRMOVQ){
        *valE = 0 + valA;
    }

    if (icode == IRMOVQ){
        *valE = 0 + valC;
    }

    if (icode == RMMOVQ){
        *valE = valB + valC;
    }

    if (icode == MRMOVQ){
        *valE = valB + valC;
    }

    if (icode == PUSHQ){
        *valE = valB + (-8);
    }

    if (icode == POPQ){
        *valE = valB + 8;
    }

    if (icode == JXX){
        *Cnd = Cond(ifun);
    }

    if (icode == CALL){
        *valE = valB + (-8);
    }

    if (icode == RET){
        *valE = valB + 8;
    }

    if (icode == CMOVXX){
        *valE = 0 + valA;
        *Cnd = Cond(ifun);
    }
}

void memoryStage(int icode, wordType valA, wordType valP, wordType valE, wordType *valM) {
    if (icode == RMMOVQ) {
        setWordInMemory(valE, valA);
    }
    if (icode == MRMOVQ) {
        *valM = getWordFromMemory(valE);
    }
    if (icode == PUSHQ) {
        setWordInMemory(valE, valA);
    }
    if (icode == POPQ) {
        *valM = getWordFromMemory(valA);
    }
    if (icode == CALL) {
        setWordInMemory(valE, valP);
    }
    if (icode == RET) {
        *valM = getWordFromMemory(valA);
    }
}

void writebackStage(int icode, int rA, int rB, wordType valE, wordType valM, bool Cnd) {
    if ((icode == OPQ) || (icode == RRMOVQ) || (icode == IRMOVQ)) {
        setRegister(rB, valE);
    }

    if (icode == MRMOVQ) {
        setRegister(rA, valM);
    }
    if (icode == PUSHQ) {
        setRegister(RSP, valE);
    }
    if (icode == POPQ) {
        setRegister(RSP, valE);
        setRegister(rA, valM);
    }
    if (icode == CALL) {
        setRegister(RSP, valE);
    }
    if (icode == RET) {
        setRegister(RSP, valE);
    }
    if (icode == CMOVXX) {
        if (Cnd) {
            setRegister(rB, valE);
        }
    }
}

void pcUpdateStage(int icode, wordType valC, wordType valP, bool Cnd, wordType valM) {
    if ((icode == CALL) || (icode == RET)){
        if (icode == CALL) {
            setPC(valC);
        }
        if (icode == RET) {
            setPC(valM);
        }
    }
    else {
        setPC(valP);
    }

    if (icode == HALT){
        setStatus(STAT_HLT);
    }
    if (icode == JXX){
        setPC (Cnd?valC:valP);
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
  
  writebackStage(icode, rA, rB, valE, valM, Cnd);
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