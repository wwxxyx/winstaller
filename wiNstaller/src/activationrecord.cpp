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
#include <CoreFoundation.h>
#include <openssl/evp.h>
#include "activationrecord.h"
#include "accounttoken.h"

CFDictionaryRef GenerateActivationRecord(EVP_PKEY * PKey,
                                         const string & DeviceID,
                                         const string & IMEI,
                                         const string & ICCID)
{
  string accounttoken_str = GenerateAccountTokenString(DeviceID, IMEI, ICCID);
  CFDataRef accounttoken_signature = SignAccountToken(PKey, accounttoken_str);
  if (accounttoken_signature == NULL) return NULL;
  
  CFDataRef account_token = GenerateAccountToken(accounttoken_str);
  CFDictionaryRef activation_record = GenerateActivationRecord(account_token,
                                                               accounttoken_signature);
  return activation_record;
}

CFDictionaryRef GenerateRequest(EVP_PKEY * PKey,
                                const string & DeviceID,
                                const string & IMEI,
                                const string & ICCID)
{
  CFDictionaryRef activation_record = GenerateActivationRecord(PKey, DeviceID, IMEI, ICCID);
  CFDictionaryRef request = GenerateRequest(CFSTR("Activate"), activation_record);
  return request;
}

CFDictionaryRef GenerateActivationRecord(CFDataRef AccountToken,
                                         CFDataRef AccountTokenSignature)
{
  const void * keys[] = {
    CFSTR("AccountToken"),
    CFSTR("AccountTokenCertificate"),
    CFSTR("AccountTokenSignature"),
    CFSTR("DeviceCertificate"),
    CFSTR("FairPlayKeyData")
  };
  const void * values[] = {
    AccountToken,
    CFDataCreate(NULL, NULL, 0),
    AccountTokenSignature,
    CFDataCreate(NULL, NULL, 0),
    CFDataCreate(NULL, NULL, 0)
  };
  return CFDictionaryCreate (NULL /* allocator */,
    keys, values, sizeof (keys) / sizeof (void*),
    &kCFCopyStringDictionaryKeyCallBacks,
    &kCFTypeDictionaryValueCallBacks);
}

CFDictionaryRef GenerateRequest(CFStringRef Request, CFDictionaryRef Dict)
{
  const void * keys[] = { CFSTR("ActivationRecord"), CFSTR("Request") };
  const void * values[] = { Dict, Request };
  return CFDictionaryCreate (NULL /* allocator */,
    keys, values, sizeof (keys) / sizeof (void*),
    &kCFCopyStringDictionaryKeyCallBacks,
    NULL /* valueCallBacks */);
}
