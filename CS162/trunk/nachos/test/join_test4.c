int main(int argc, char** argv)
{
  int retVal, retVal2, stat;
  retVal = exec("matmult.coff", 0, argv);
  printf("retVal: %d\n", retVal);
  retVal2 = join(retVal, &stat);
  printf("retVal2 (should be 1): %d\n", retVal2);
  printf("status return value (should be 7220): %d\n", stat);
}
