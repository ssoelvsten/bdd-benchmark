#include "../flam.cpp"

#include "adapter.h"

int
main(int argc, char** argv)
{
  return run_flam<sylvan_bcdd_adapter>(argc, argv);
}
