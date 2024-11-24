// @formatting: off
%module(directors = "1") spatialfhe 
%{
    #include "CipherText.h"
    #include "PlainText.h"
    #include "HECrypto.h"
    #include "SealCrypto.h"
    #include "CipherMat.h"
    #include "BaseFHEManager.h"
    #include "MatFHEManager.h"
    #include "SpatialFHEManager.h"
    #include "PhantomCrypto.cuh"
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
   %template(CipherTextVector)      vector<SpatialFHE::CipherText>;
}

%include "CipherText.h"
%include "PlainText.h"
%include "HECrypto.h"
%include "SealCrypto.h"
%include "PhantomCrypto.cuh"
%include "CipherMat.h"
%include "BaseFHEManager.h"
%include "MatFHEManager.h"
%include "SpatialFHEManager.h"

