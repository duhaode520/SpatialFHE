//
// Created by ubuntu on 1/7/25.
//

#include "tfhe_geos/TFHEGeometry.h"
#include "tfhe_geos/TFHEGeometryFactory.h"

namespace SpatialFHE::geom {

    TFHEGeometry::TFHEGeometry(const TFHEGeometry &geom) : factory(geom.factory) {
        factory->addRef();
    }

    TFHEGeometry::TFHEGeometry(const TFHEGeometryFactory *factory) : factory(factory) {
        if (this->factory == nullptr) {
            this->factory = TFHEGeometryFactory::getDefaultInstance();
        }
        this->factory->addRef();
    }

    TFHEBool TFHEGeometry::intersects(const TFHEGeometry *geom) const {}

    TFHEBool TFHEGeometry::contains(const TFHEGeometry *geom) const {}

    TFHEBool TFHEGeometry::within(const TFHEGeometry *geom) const {}

    TFHEBool TFHEGeometry::touches(const TFHEGeometry *geom) const {}

    TFHEBool TFHEGeometry::overlaps(const TFHEGeometry *geom) const {}

    TFHEBool TFHEGeometry::crosses(const TFHEGeometry *geom) const {}

    TFHEBool TFHEGeometry::disjoint(const TFHEGeometry *geom) const {}

    TFHEBool TFHEGeometry::equals(const TFHEGeometry *geom) const {}

    TFHEBool TFHEGeometry::covers(const TFHEGeometry *geom) const {}

    TFHEBool TFHEGeometry::coveredBy(const TFHEGeometry *geom) const {}

    TFHEBool TFHEGeometry::relate(const TFHEGeometry *geom, const std::string &pattern) const {}

    std::shared_ptr<TFHEIntersectionMatrix> TFHEGeometry::relate(const TFHEGeometry *geom) {
        // checkNotGeometryCollection(geom);
        // chekcNotGeometryCollection(*this);
    }
    bool TFHEGeometry::hasNullElements(const TFHECoordinateSequence *list) {
        std::size_t npts = list->size();
        for (std::size_t i = 0; i < npts; i++) {
            if (list->getAt(i).isNull()) {
                return true;
            }
        }
        return false;
    }
}  // namespace SpatialFHE::geom