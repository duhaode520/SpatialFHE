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
%ignore *::operator=;

namespace std {
   %template(IntVector)             vector<int>;
   %template(DoubleVector)          vector<double>;
   %template(LongVector)            vector<uint32_t>;
   %rename(LongVector)              vector<uint32_t>;
   %template(BooleanVector)         vector<bool>;
   %template(StringVector)          vector<string>;
   %template(PlainTextVector)       vector<SpatialFHE::PlainText>;
}

%include "ciphertext.h"
%include "plaintext.h"
%include "hecrypto.h"
%include "sealcrypto.h"

