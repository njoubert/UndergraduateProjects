int main(int argc, char** argv)
{
  int retVal, retVal2;
  retVal = exec("blah.coff", 0, argv);
  printf("retVal: %d\n", retVal);
  retVal2 = join(retVal, 0);
  printf("retVal2: %d\n", retVal2);
  return 888;
}
