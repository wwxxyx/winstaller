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
#ifndef ACTIVATIONRECORD_H__
#define ACTIVATIONRECORD_H__

CFDictionaryRef GenerateActivationRecord(EVP_PKEY * PKey,
                                         const string & DeviceID,
                                         const string & IMEI,
                                         const string & ICCID);

CFDictionaryRef GenerateActivationRecord(CFDataRef AccountToken,
                                         CFDataRef AccountTokenSignature);

CFDictionaryRef GenerateRequest(EVP_PKEY * PKey,
                                const string & DeviceID,
                                const string & IMEI,
                                const string & ICCID);

CFDictionaryRef GenerateRequest(CFStringRef Request, CFDictionaryRef Dict);

#endif

