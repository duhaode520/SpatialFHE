//
// Created by ubuntu on 1/7/25.
//

#include "TFHEGeometry.h"

#include <tfhe_geos/operation/relateng/TFHERelateNG.h>

#include "TFHEGeometryFactory.h"

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

    TFHEBool TFHEGeometry::intersects(const TFHEGeometry *g) const {
        TFHEBool envIntersects = getEnvelope()->intersects(g->getEnvelope());
        if (!envIntersects.decrypt()) {
            return TFHEBool::tfhe_false;
        }

        // TODO: optimization for rectangle arguments
        // TODO: implement RectangleIntersects
        // if (isRectangle()) {
        //     auto p = dynamic_cast<const TFHEPolygon *>(this);
        // }
        // if (g->isRectangle()) {
        //     auto p = dynamic_cast<const TFHEPolygon *>(g);
        // }
        return operation::relateng::TFHERelateNG::intersects(this, g);
    }

    TFHEBool TFHEGeometry::contains(const TFHEGeometry *geom) const {
        return operation::relateng::TFHERelateNG::contains(this, geom);
    }

    TFHEBool TFHEGeometry::within(const TFHEGeometry *geom) const {
        return operation::relateng::TFHERelateNG::contains(geom, this);
    }

    TFHEBool TFHEGeometry::touches(const TFHEGeometry *geom) const {
        // Short-circuit test
        TFHEBool envIntersects = getEnvelope()->intersects(geom->getEnvelope());
        if (!envIntersects.decrypt()) {
            return TFHEBool::tfhe_false;
        }
        return operation::relateng::TFHERelateNG::touches(this, geom);
    }

    TFHEBool TFHEGeometry::overlaps(const TFHEGeometry *geom) const {
        return operation::relateng::TFHERelateNG::overlaps(this, geom);
    }

    TFHEBool TFHEGeometry::crosses(const TFHEGeometry *geom) const {
        return operation::relateng::TFHERelateNG::crosses(this, geom);
    }

    TFHEBool TFHEGeometry::disjoint(const TFHEGeometry *geom) const {
        return operation::relateng::TFHERelateNG::disjoint(this, geom);
    }

    TFHEBool TFHEGeometry::equals(const TFHEGeometry *geom) const {
        return operation::relateng::TFHERelateNG::equalsTopo(this, geom);
    }

    TFHEBool TFHEGeometry::covers(const TFHEGeometry *geom) const {
        return operation::relateng::TFHERelateNG::covers(this, geom);
    }

    TFHEBool TFHEGeometry::coveredBy(const TFHEGeometry *geom) const {
        return operation::relateng::TFHERelateNG::contains(geom, this);
    }

    TFHEBool TFHEGeometry::relate(const TFHEGeometry *geom, const std::string &pattern) const {
        return operation::relateng::TFHERelateNG::relate(this, geom, pattern);
    }

    std::unique_ptr<TFHEIntersectionMatrix> TFHEGeometry::relate(const TFHEGeometry *geom) {
        // checkNotGeometryCollection(geom);
        // chekcNotGeometryCollection(*this);
        return operation::relateng::TFHERelateNG::relate(this, geom);
    }

    std::unique_ptr<TFHEIntersectionMatrix> TFHEGeometry::relate(const TFHEGeometry &geom) {
        return relate(&geom);
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