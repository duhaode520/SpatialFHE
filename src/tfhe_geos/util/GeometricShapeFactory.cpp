//
// Created by ubuntu on 2/20/25.
//
#include "GeometricShapeFactory.h"
#include <tfhe_geos/geom/TFHECoordinate.h>
#include <tfhe_geos/geom/TFHEGeometryFactory.h>
using namespace SpatialFHE::geom;

namespace SpatialFHE::util {

    GeometricShapeFactory::GeometricShapeFactory(const TFHEGeometryFactory* factory) :
            geomFact(factory), nPts(100) {}

    void GeometricShapeFactory::setBase(const TFHECoordinate& base) {
        dim.setBase(base);
    }

    void GeometricShapeFactory::setCentre(const TFHECoordinate& centre) {
        dim.setCentre(centre);
    }

    void GeometricShapeFactory::setNumPoints(uint32_t nNPts) {
        nPts = nNPts;
    }

    void GeometricShapeFactory::setSize(double size) {
        dim.setSize(size);
    }

    void GeometricShapeFactory::setWidth(double width) {
        dim.setWidth(width);
    }

    void GeometricShapeFactory::setHeight(double height) {
        dim.setHeight(height);
    }

    std::unique_ptr<TFHEPolygon> GeometricShapeFactory::createRectangle() {
        uint32_t i;
        uint32_t ipt = 0;
        uint32_t nSide = nPts / 4;
        if (nSide < 1) {
            nSide = 1;
        }
        std::unique_ptr<TFHEEnvelope> env(dim.getEnvelope());
        TFHEDecimal XsegLen = env->getWidth() / nSide;
        TFHEDecimal YsegLen = env->getHeight() / nSide;

        std::vector<TFHECoordinate> vc(4 * nSide + 1);

        for (i = 0; i < nSide; i++) {
            TFHEDecimal x = env->getMinX() + XsegLen * i;
            TFHEDecimal y = env->getMinY();
            vc[ipt++] = coord(x, y);
        }
        for (i = 0; i < nSide; i++) {
            TFHEDecimal x = env->getMaxX();
            TFHEDecimal y = env->getMinY() + YsegLen * i;
            vc[ipt++] = coord(x, y);
        }
        for (i = 0; i < nSide; i++) {
            TFHEDecimal x = env->getMaxX() - XsegLen * i;
            TFHEDecimal y = env->getMaxY();
            vc[ipt++] = coord(x, y);
        }
        for (i = 0; i < nSide; i++) {
            TFHEDecimal x = env->getMinX();
            TFHEDecimal y = env->getMaxY() - YsegLen * i;
            vc[ipt++] = coord(x, y);
        }
        vc[ipt++] = vc[0];
        auto cs = geomFact->getCoordinateSequenceFactory()->create(std::move(vc));
        auto ring = geomFact->createLinearRing(std::move(cs));
        auto poly = geomFact->createPolygon(std::move(ring));
        return poly;
    }

    GeometricShapeFactory::Dimensions::Dimensions() : base(TFHECoordinate()), centre(TFHECoordinate()) {}

    void GeometricShapeFactory::Dimensions::setBase(const TFHECoordinate& newBase) {
        base = newBase;
    }

    void GeometricShapeFactory::Dimensions::setCentre(const TFHECoordinate& newCentre) {
        centre = newCentre;
    }

    void GeometricShapeFactory::Dimensions::setSize(double size) {
        height = size;
        width = size;
    }

    void GeometricShapeFactory::Dimensions::setWidth(double nWidth) {
        width = nWidth;
    }

    void GeometricShapeFactory::Dimensions::setHeight(double nHeight) {
        height = nHeight;
    }

    std::unique_ptr<TFHEEnvelope> GeometricShapeFactory::Dimensions::getEnvelope() const {
        int iw = static_cast<int>(width);
        int ih = static_cast<int>(height);
        if (!base.isNull()) {
            return std::make_unique<TFHEEnvelope>(base.x, base.x + iw, base.y, base.y + ih);
        }
        if (!centre.isNull()) {
            return std::make_unique<TFHEEnvelope>(
                centre.x - iw / 2, centre.x + iw / 2, centre.y - ih / 2, centre.y + ih / 2);
        }
        return std::make_unique<TFHEEnvelope>(TFHEDecimal(0), TFHEDecimal(iw), TFHEDecimal(0), TFHEDecimal(ih));
    }

    /*protected*/
    TFHECoordinate GeometricShapeFactory::coord(TFHEDecimal& x, TFHEDecimal& y) const {
        TFHECoordinate ret(x, y);
        return ret;
    }
}