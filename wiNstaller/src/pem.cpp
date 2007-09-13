/*
 * Copyright (C) 2007 iZsh 
 * 
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License version 2 for more details
 */
using namespace std;
#include <iostream>
#include "pem.h"

EVP_PKEY * LoadPEM(const char* Keyfile)
{
  FILE * fp = fopen(Keyfile, "r");
  if (fp == NULL) {
    perror(Keyfile);
    exit (1);
  }
  EVP_PKEY * pkey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
  fclose(fp);
  
  if (pkey == NULL) { 
    ERR_print_errors_fp(stderr);
    exit(1);
  }
  return pkey;
}
