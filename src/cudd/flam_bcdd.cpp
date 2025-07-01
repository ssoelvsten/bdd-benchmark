#include "../flam.cpp"

#include "adapter.h"

int
main(int argc, char** argv)
{
  return run_flam<cudd_bcdd_adapter>(argc, argv);
}
