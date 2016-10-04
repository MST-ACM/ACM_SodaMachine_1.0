#include "reg89C51.h"
#include "soda89C51.h"

void main()
{
  IE = 0x90;
  SCON = 0x50;
  