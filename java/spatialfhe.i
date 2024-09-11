// @formatting: off
%module(directors = "1") spatialfhe 
%{
    #include "ciphertext.h"
    #include "plaintext.h"
    #include "hecrypto.h"
    #include "sealcrypto.h"
%}

%include "std_vector.i"
%include "std_string.i"

%feature("director") SEALCrypto;


%include "ciphertext.h"
%include "plaintext.h"
%include "hecrypto.h"
%include "sealcrypto.h"

