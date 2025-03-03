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

    // TFHE
//    #include "TFHERegisteredType.h"
    #include "TFHEInt32.h"
    #include "TFHEBool.h"
    #include "TFHEContext.h"

    // TFHE GEOS
    #include "tfhe_geos/geom/enums.h"
    #include "tfhe_geos/geom/TFHECoordinate.h"
    #include "tfhe_geos/geom/TFHEEnvelope.h"
    #include "tfhe_geos/geom/TFHEIntersectionMatrix.h"
    #include "tfhe_geos/geom/TFHEGeometry.h"
    #include "tfhe_geos/geom/TFHEPoint.h"
    #include "tfhe_geos/geom/TFHELineString.h"
    #include "tfhe_geos/geom/TFHELinearRing.h"
    #include "tfhe_geos/geom/TFHEPolygon.h"
    #include "tfhe_geos/io/WKTReader.h"


%}

%include "std_vector.i"
%include "std_string.i"
%include "stdint.i"
%include "std_unique_ptr.i"
%include "std_shared_ptr.i"

// %feature("director") SpatialFHE::geom::TFHEGeometry;
%ignore *::operator=;
%ignore *::clone;

// %typemap(jtype) int32_t "int"
// %typemap(jstype) int32_t "int"

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
//    %template(TFHERegisteredInt)     TFHERegisteredType<int>;
//    %template(TFHERegisteredBool)    TFHERegisteredType<bool>;
%unique_ptr(SpatialFHE::geom::TFHEGeometry)
%unique_ptr(SpatialFHE::geom::TFHEPoint)
%unique_ptr(SpatialFHE::geom::TFHELineString)
%unique_ptr(SpatialFHE::geom::TFHELinearRing)
%unique_ptr(SpatialFHE::geom::TFHEPolygon)
%unique_ptr(SpatialFHE::geom::TFHEEnvelope)
%unique_ptr(SpatialFHE::geom::TFHECoordinate)


// 还可以添加辅助转换函数
%extend SpatialFHE::geom::TFHEGeometry {
    SpatialFHE::geom::TFHEPoint* asPoint() {
        return dynamic_cast<SpatialFHE::geom::TFHEPoint*>($self);
    }
    
    SpatialFHE::geom::TFHELineString* asLineString() {
        return dynamic_cast<SpatialFHE::geom::TFHELineString*>($self);
    }
    
    SpatialFHE::geom::TFHEPolygon* asPolygon() {
        return dynamic_cast<SpatialFHE::geom::TFHEPolygon*>($self);
    }
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

%include "TFHEContext.h"
%include "TFHEInt32.h"
%include "TFHEBool.h"
%include "tfhe_geos/geom/enums.h"
%include "tfhe_geos/geom/TFHECoordinate.h"
%include "tfhe_geos/geom/TFHEEnvelope.h"
%include "tfhe_geos/geom/TFHEIntersectionMatrix.h"
%include "tfhe_geos/geom/TFHEGeometry.h"
%include "tfhe_geos/geom/TFHEPoint.h"
%include "tfhe_geos/geom/TFHELineString.h"
%include "tfhe_geos/geom/TFHELinearRing.h"
%include "tfhe_geos/geom/TFHEPolygon.h"
%include "tfhe_geos/io/WKTReader.h"
