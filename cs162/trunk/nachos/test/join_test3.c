int main(int argc, char** argv)
{
  int retVal, retVal2, retVal3, joinVal3, joinVal4, status, status2;
  
  retVal = exec("join_test2.coff", 0, argv);
  printf("retVal: %d\n", retVal);
  retVal2 = join(retVal, &status);
  printf("retVal2: %d\n", retVal2);
  printf("status1: %d\n", status);

/*
retVal = exec("matmult.coff", 0, argv);
  printf("retVal: %d\n", retVal);
  retVal2 = exec("matmult.coff", 0, argv);
  printf("retVal2: %d\n", retVal2);
  retVal3 = exec("join_abnormal.coff", 0, argv);
  printf("retVal3: %d\n", retVal3);
  joinVal3 = join(retVal2, status);
  printf("status1: %d\n", *status);
  printf("joinVal3: %d\n", joinVal3);
  joinVal4 = join(retVal3, status2);
  printf("status2: %d\n", *status2);
  printf("joinVal4: %d\n", joinVal4);*/
}
