/* user_settings.h
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#if defined(RENESAS_T4_USE)
  #include "wolfssl_simple_demo.h"
#endif
/*-- Renesas MCU type ---------------------------------------------------------
 *
 *
 *----------------------------------------------------------------------------*/
  #define WOLFSSL_RENESAS_RX72N


/*-- Renesas TSIP usage and its version ---------------------------------------
 *
 *  "WOLFSSL_RENESAS_TSIP" definition makes wolfSSL to use H/W acceleration
 *   for cipher operations. 
 *  TSIP definition asks to have its version number.
 *  "WOLFSSL_RENESAS_TSIP_VER" takes following value:
 *      106: TSIPv1.06
 *      109: TSIPv1.09
 *      113: TSIPv1.13
 *      114: TSIPv1.14
 *      115: TSIPv1.15
 *----------------------------------------------------------------------------*/
  #define WOLFSSL_RENESAS_TSIP
  #define WOLFSSL_RENESAS_TSIP_VER     115

#if defined(SIMPLE_TLS_CLIENT) || defined(SIMPLE_TLS_SERVER)
 #undef WOLFSSL_RENESAS_TSIP
 #undef WOLFSSL_RENESAS_TSIP_VER
#endif

/*-- TLS version definitions  --------------------------------------------------
 *
 * wolfSSL supports TLSv1.2 by default. In case you want your system to support
 * TLSv1.3, uncomment line below.
 * 
 *----------------------------------------------------------------------------*/
#define WOLFSSL_TLS13


/*-- Operating System related definitions --------------------------------------
 * 
 *  In case any real-time OS is used, define its name(e.g. FREERTOS).
 *  Otherwise, define "SINGLE_THREADED". They are exclusive each other.
 *   
 *----------------------------------------------------------------------------*/
#if !defined(RENESAS_T4_USE)
 #define FREERTOS
 #define FREERTOS_TCP
#else
 #define SINGLE_THREADED
#endif

#if !defined(FREERTOS_TCP)
    #define WOLFSSL_NO_SOCK
    #define WOLFSSL_USER_IO
#endif

/*-- Cipher related definitions  -----------------------------------------------
 *
 *
 *----------------------------------------------------------------------------*/

  #define NO_DEV_RANDOM
  #define NO_MD4
  #define WOLFSSL_DH_CONST
  #define HAVE_TLS_EXTENSIONS

  #define HAVE_AESGCM
  #define HAVE_AESCCM
  #define HAVE_AES_CBC
  #define WOLFSSL_SHA512

  #define HAVE_SUPPORTED_CURVES
  #define HAVE_ECC
  #define HAVE_CURVE25519
  #define CURVE25519_SMALL


  #define WOLFSSL_STATIC_RSA

  /* USE_ECC_CERT
   * This macro is for selecting root CA certificate to load, it is valid only
   * in example applications. wolfSSL does not refer this macro. 
   * If you want to use cipher suites including ECDSA authentication in 
   * the example applications with TSIP, enable this macro.
   * In TSIP 1.13 or later version, following cipher suites are 
   * available:
   * - TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256
   * - TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SAH256
   * 
   * Note that, this macro disables cipher suites including RSA
   * authentication such as:
   * - TLS_RSA_WITH_AES_128_CBC_SHA
   * - TLS_RSA_WITH_AES_256_CBC_SHA 
   * - TLS_RSA_WITH_AES_128_CBC_SHA256
   * - TLS_RSA_WITH_AES_256_CBC_SHA256
   * - TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256
   * - TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA256
   * 
   */
  #define USE_ECC_CERT

  /* In this example application, Root CA cert buffer named 
   * "ca_ecc_cert_der_256" is used under the following macro definition 
   * for ECDSA.
   */
  #define USE_CERT_BUFFERS_256

  /* In this example application, Root CA cert buffer named 
   * "ca_cert_der_2048" is used under the following macro definition 
   * for RSA authentication.
   */
  #define USE_CERT_BUFFERS_2048

/*-- Misc definitions  ---------------------------------------------------------
 *
 *
 *----------------------------------------------------------------------------*/
  #define SIZEOF_LONG_LONG 8


 /* 
  * -- "NO_ASN_TIME" macro is to avoid certificate expiration validation --
  *  
  * Note. In your actual products, do not forget to comment-out 
  * "NO_ASN_TIME" macro. And prepare time function to get calender time,
  * otherwise, certificate expiration validation will not work.  
  */
  /*#define NO_ASN_TIME*/
  
  #define NO_MAIN_DRIVER
  #define BENCH_EMBEDDED
  #define NO_WOLFSSL_DIR 
  #define WOLFSSL_NO_CURRDIR
  #define NO_FILESYSTEM
  #define WOLFSSL_LOG_PRINTF
  #define WOLFSSL_HAVE_MIN
  #define WOLFSSL_HAVE_MAX
  #define WOLFSSL_SMALL_STACK
  #define NO_WRITEV
  

  #define WOLFSSL_USER_CURRTIME /* for benchmark */
  #define TIME_OVERRIDES
  #define XTIME    time
  #define WOLFSSL_GMTIME
  #define XGMTIME(c,t)  gmtime(c)
  #define USE_WOLF_SUSECONDS_T
  #define USE_WOLF_TIMEVAL_T

  #define  XSTRNCASECMP(s1,s2,n)  strncmp(s1,s2,n)

  #define WC_RSA_BLINDING
  #define TFM_TIMING_RESISTANT
  #define ECC_TIMING_RESISTANT

  #define USE_FAST_MATH

/*-- Debugging options  ------------------------------------------------------
 *
 * "DEBUG_WOLFSSL" definition enables log to output into stdout.
 * Note: wolfSSL_Debugging_ON() must be called just after wolfSSL_Init().
 *----------------------------------------------------------------------------*/

/*#define DEBUG_WOLFSSL*/

/*-- Definitions for functionality negation  -----------------------------------
 *
 * 
 *----------------------------------------------------------------------------*/

/*#define NO_RENESAS_TSIP_CRYPT*/
/*#define NO_WOLFSSL_RENESAS_TSIP_TLS_SESSION*/

#if defined(WOLFCRYPT_ONLY)
    #undef WOLFSSL_RENESAS_TSIP
#endif
/*-- Consistency checking between definitions  ---------------------------------
 *
 *  
 *----------------------------------------------------------------------------*/

/*-- TSIP TLS specific definitions --*/
#if defined(WOLFSSL_RENESAS_TSIP)
    #if !defined(WOLFSSL_RENESAS_TSIP_VER)
      #error "WOLFSSL_RENESAS_TSIP_VER is required to be defined and have value"
    #endif
#endif

/*-- Complementary definitions  ------------------------------------------------
 *
 *
 *----------------------------------------------------------------------------*/

#if defined(WOLFSSL_RENESAS_TSIP)

    #if !defined(NO_RENESAS_TSIP_CRYPT)
        #define WOLFSSL_RENESAS_TSIP_CRYPT
        #define WOLFSSL_RENESAS_TSIP_TLS
        #define WOLFSSL_RENESAS_TSIP_TLS_AES_CRYPT
        #define HAVE_PK_CALLBACKS
        #define WOLF_CRYPTO_CB
    #endif

#else
    #define OPENSSL_EXTRA
    #define WOLFSSL_GENSEED_FORTEST /* Warning: define your own seed gen */
#endif


/*-- TLS version and required definitions --*/
#if defined(WOLFSSL_TLS13)
    #define HAVE_FFDHE_2048
    #define HAVE_HKDF
    #define WC_RSA_PSS
#endif

/*-- strcasecmp */
#define XSTRCASECMP(s1,s2) strcmp((s1),(s2))
