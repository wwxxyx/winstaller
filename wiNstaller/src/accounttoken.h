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
#ifndef ACCOUNTTOKEN_H__
#define ACCOUNTTOKEN_H__

string GenerateAccountTokenString(const string & DeviceID,
                                  const string & IMEI,
                                  const string & ICCID);

CFDataRef GenerateAccountToken(const string & AccountTokenStr);

CFDataRef SignAccountToken(EVP_PKEY * PKey,
                          const string & AccountToken);

#endif
