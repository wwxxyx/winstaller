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
#include <string>
#include <sstream>
#include <CoreFoundation.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include "accounttoken.h"

string GenerateAccountTokenString(const string & DeviceID,
                                  const string & IMEI,
                                  const string & ICCID)
{
  ostringstream ostr("");
  
  ostr << "{" << endl
    << "\t\"UniqueDeviceID\" = \"" << DeviceID << "\";" << endl
    << "\t\"InternationalMobileEquipmentIdentity\" = \"" << IMEI << "\";" << endl
    << "\t\"IntegratedCircuitCardIdentity\" = \"" << ICCID << "\";" << endl
    << "}" << endl;
  ostr.flush();
  return ostr.str();
}

CFDataRef GenerateAccountToken(const string & AccountTokenStr)
{
  CFDataRef account_token = CFDataCreate(NULL,
                                         reinterpret_cast<const UInt8*>(AccountTokenStr.c_str()),
                                         AccountTokenStr.length());
  return account_token;
}

CFDataRef SignAccountToken(EVP_PKEY * PKey,
                      const string & AccountToken)
{
  EVP_MD_CTX md_ctx;
  unsigned char sig_buf [4096];
  
  EVP_SignInit(&md_ctx, EVP_sha1());
  EVP_SignUpdate (&md_ctx, AccountToken.c_str(), AccountToken.length());
  unsigned int sig_len = sizeof(sig_buf);
  if (EVP_SignFinal (&md_ctx, sig_buf, &sig_len, PKey) != 1) {
    ERR_print_errors_fp(stderr);
    return NULL;
  }
  
  return CFDataCreate(NULL, sig_buf, sig_len);
}
