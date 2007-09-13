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
#ifndef PEM_H__
#define PEM_H__

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/pem.h>

EVP_PKEY * LoadPEM(const char* Keyfile);

#endif

