//
// Created by ubuntu on 1/14/25.
//

#include "TFHERelateGeometry.h"

#include <tfhe_geos/algorithm/TFHEOrientation.h>
#include <tfhe_geos/geom/TFHEGeometry.h>
#include <tfhe_geos/geom/TFHELinearRing.h>
#include <tfhe_geos/geom/TFHEPoint.h>
#include <tfhe_geos/geom/TFHEPolygon.h>
#include <tfhe_geos/noding/TFHESegmentString.h>

#include "DimensionLocation.h"

using namespace SpatialFHE::geom;
using SpatialFHE::noding::TFHESegmentString;

namespace SpatialFHE::operation::relateng {

    void TFHERelateGeometry::analyzeDimensions() {
        if (isGeomEmpty) {
            return;
        }

        TFHEGeometryTypeId typeId = geom->getGeometryTypeId();
        if (typeId == TFHEGeometryTypeId::TFHE_POINT) {
            hasPoints = true;
            geomDim = Dimension::P;
        }
        if (typeId == TFHEGeometryTypeId::TFHE_LINESTRING) {
            hasLines = true;
            geomDim = Dimension::L;
        }
        if (typeId == TFHEGeometryTypeId::TFHE_POLYGON) {
            hasAreas = true;
            geomDim = Dimension::A;
        }
    }

    TFHERelatePointLocator *TFHERelateGeometry::getLocator() {
        if (locator == nullptr) {
            locator.reset(new TFHERelatePointLocator(geom, m_isPrepared));
        }
        return locator.get();
    }

    TFHECoordinate::ConstSet TFHERelateGeometry::createUniquePoints() {
        // only called on P geometries;
        // currently only Point, we may add MultiPoint in the future
        // std::vector<const TFHECoordinate*> pts;
        // ComponentCoordinateExtracter::getCoordinates(*geom, pts);

        // so we directly add the point to the set
        return {geom->getCoordinate()};
    }

    void TFHERelateGeometry::extractSegmentStrings(
        bool isA,
        const TFHEEnvelope *env,
        const TFHEGeometry *geom,
        std::vector<const TFHESegmentString *> &segStrings,
        std::vector<std::unique_ptr<const TFHERelateSegmentString>> &segStore) {
        // we only need to consider single geometry. GeometryCollection is not supported
        extractSegmentStringsFromAtomic(isA, geom, nullptr, env, segStrings, segStore);
    }

    void TFHERelateGeometry::extractSegmentStringsFromAtomic(
        bool isA,
        const TFHEGeometry *geom,
        const TFHEGeometry *parentPolygonal,
        const TFHEEnvelope *env,
        std::vector<const TFHESegmentString *> &segStrings,
        std::vector<std::unique_ptr<const TFHERelateSegmentString>> &segStore) {
        if (geom->isEmpty()) {
            return;
        }

        bool doExtract = (env == nullptr) || env->intersects(geom->getEnvelope()).decrypt();
        if (!doExtract) {
            return;
        }

        elementId++;
        if (geom->getGeometryTypeId() == TFHEGeometryTypeId::TFHE_LINESTRING ||
            geom->getGeometryTypeId() == TFHEGeometryTypeId::TFHE_LINEARRING) {
            const TFHELineString *line = dynamic_cast<const TFHELineString *>(geom);
            /*
             * Condition the input Coordinate sequence so that it has no repeated points.
             * This requires taking a copy which removeRepeated does behind the scenes and stores in csStore.
             */
            // FIXME: removeRepeated is not implemented yet, different from GEOS
            // const TFHECoordinateSequence *cs = removeRepeated(line->getCoordinatesRO());

            const TFHECoordinateSequence *cs = line->getCoordinatesRO();
            auto ss = TFHERelateSegmentString::createLine(cs, isA, elementId, this);
            segStore.emplace_back(ss);
            segStrings.push_back(ss);
        } else if (geom->getGeometryTypeId() == TFHEGeometryTypeId::TFHE_POLYGON) {
            const TFHEPolygon *poly = static_cast<const TFHEPolygon *>(geom);
            const TFHEGeometry *parentPoly;
            if (parentPolygonal != nullptr)
                parentPoly = static_cast<const TFHEGeometry *>(parentPolygonal);
            else
                parentPoly = static_cast<const TFHEGeometry *>(poly);
            extractRingToSegmentString(isA, poly->getExteriorRing(), 0, env, parentPoly, segStrings, segStore);
            for (uint32_t i = 0; i < poly->getNumInteriorRing(); i++) {
                extractRingToSegmentString(
                    isA, poly->getInteriorRingN(i), static_cast<int>(i + 1), env, parentPoly, segStrings, segStore);
            }
        }
    }

    void TFHERelateGeometry::extractRingToSegmentString(
        bool isA,
        const TFHELinearRing *ring,
        int ringId,
        const TFHEEnvelope *env,
        const TFHEGeometry *parentPoly,
        std::vector<const TFHESegmentString *> &segStrings,
        std::vector<std::unique_ptr<const TFHERelateSegmentString>> &segStore) {
        if (ring->isEmpty())
            return;
        if (env != nullptr && !env->intersects(ring->getEnvelope()).decrypt())
            return;

        /*
         * Condition the input Coordinate sequence so that it has no repeated points
         * and is oriented in a deterministic way. This requires taking a copy which
         * orientAndRemoveRepeated does behind the scenes and stores in csStore.
         */
        bool requireCW = (ringId == 0);
        const TFHECoordinateSequence *cs = orientAndRemoveRepeated(ring->getCoordinatesRO(), requireCW);
        auto ss = TFHERelateSegmentString::createRing(cs, isA, elementId, ringId, parentPoly, this);
        segStore.emplace_back(ss);
        segStrings.push_back(ss);
    }

    std::unique_ptr<TFHECoordinateSequence> TFHERelateGeometry::removeRepeatedPointsInternal(
        const TFHECoordinateSequence *seq) {
        if (seq->isEmpty()) {
            return std::make_unique<TFHECoordinateSequence>();
        }
        auto ret = std::make_unique<TFHECoordinateSequence>();
        for (int i = 0; i < seq->size(); i++) {
            const TFHECoordinate &p = seq->getAt(i);
            if (ret->isEmpty() || !(ret->getAt(i - 1) == p)) {
                ret->add(p);
            }
        }
        return ret;
    }

    std::vector<const TFHESegmentString *> TFHERelateGeometry::extractSegmentStrings(
        bool isA,
        const TFHEEnvelope *env) {
        std::vector<const TFHESegmentString *> segStrings;

        // When we get called in the context of a prepared geometry
        // geomA might already have segments extracted and stored,
        // so check and reuse them if possible
        if (isA && isPrepared() && env == nullptr) {
            if (segStringPermStore.empty()) {
                extractSegmentStrings(isA, env, geom, segStrings, segStringPermStore);
            } else {
                for (auto &ss : segStringPermStore) {
                    segStrings.push_back(ss.get());
                }
            }
        } else {
            segStringTempStore.clear();
            extractSegmentStrings(isA, env, geom, segStrings, segStringTempStore);
        }
        return segStrings;
    }

    bool TFHERelateGeometry::isSelfNodingRequired() const {
        TFHEGeometryTypeId typeId = geom->getGeometryTypeId();
        if (typeId == TFHEGeometryTypeId::TFHE_POINT || typeId == TFHEGeometryTypeId::TFHE_POLYGON) {
            return false;
        }
        //-- a GC with a single polygon does not need noding
        // if (hasAreas && geom->getNumGeometries() == 1)
        //     return false;
        return true;
    }

    bool TFHERelateGeometry::hasAreaAndLine() const {
        return hasAreas && hasLines;
    }

    bool TFHERelateGeometry::isNodeInArea(const TFHECoordinate *nodePt, const TFHEGeometry *parentPolygonal) {
        int dimLoc = getLocator()->locateNodeWithDim(nodePt, parentPolygonal);
        return dimLoc == DimensionLocation::AREA_INTERIOR;
    }

    const TFHECoordinateSequence *TFHERelateGeometry::removeRepeatedPoints(const TFHECoordinateSequence *seq) {
        TFHEBool hasRepeated = seq->hasRepeatedPoints();
        if (!hasRepeated.decrypt()) {
            return seq;
        }
        auto deduped = removeRepeatedPointsInternal(seq);
        TFHECoordinateSequence *cs = deduped.get();
        csStore.emplace_back(cs);
        return cs;
    }

    const TFHECoordinateSequence *TFHERelateGeometry::orientAndRemoveRepeated(
        const TFHECoordinateSequence *seq,
        bool orientCW) {
        bool isFlipped = orientCW == algorithm::TFHEOrientation::isCCW(seq).decrypt();
        bool hasRepeated = seq->hasRepeatedPoints().decrypt();
        if (!isFlipped && !hasRepeated) {
            return seq;
        }
        if (hasRepeated) {
            auto deduped = removeRepeatedPointsInternal(seq);
            TFHECoordinateSequence *cs = deduped.get();
            csStore.emplace_back(cs);
            return cs;
        }
        if (isFlipped) {
            auto reversed = seq->clone();
            reversed->reverse();
            TFHECoordinateSequence *cs = reversed.release();
            csStore.emplace_back(cs);
            return cs;
        }
        return seq;
    }

    TFHERelateGeometry::TFHERelateGeometry(const TFHEGeometry *geom) : TFHERelateGeometry(geom, false) {}

    TFHERelateGeometry::TFHERelateGeometry(const TFHEGeometry *geom, bool isPrepared) :
            geom(geom),
            m_isPrepared(isPrepared),
            geomEnv(geom->getEnvelope()),
            geomDim(geom->getDimension()),
            isGeomEmpty(geom->isEmpty()) {
        analyzeDimensions();
    }

    const TFHERelateGeometry::TFHEGeometry *TFHERelateGeometry::getGeometry() const {
        return geom;
    }

    const TFHERelateGeometry::TFHEEnvelope *TFHERelateGeometry::getEnvelope() const {
        return geomEnv;
    }

    bool TFHERelateGeometry::isPrepared() const {
        return m_isPrepared;
    }

    int TFHERelateGeometry::getDimension() const {
        return geomDim;
    }

    TFHECoordinate::ConstSet &TFHERelateGeometry::getUniquePoints() {
        if (uniquePoints.empty()) {
            uniquePoints = createUniquePoints();
        }
        return uniquePoints;
    }

    std::vector<const TFHEPoint *> TFHERelateGeometry::getEffectivePoints() {
        std::vector<const TFHEPoint *> allPoints;

        // PointExtracter::getPoints

        // 这里的 geom 只可能是 Point 类型 或者 MultiPoint 类型(未来可能会添加)
        if (geom->getDimension() == Dimension::P) {
            allPoints = {dynamic_cast<const TFHEPoint *>(geom)};
        }

        if (getDimension() <= Dimension::P) {
            return allPoints;
        }

        // only returns points that are not on covered by another element
        std::vector<const TFHEPoint *> ptList;
        for (const TFHEPoint *pt : allPoints) {
            if (pt->isEmpty()) {
                continue;
            }
            int locDim = locateWithDim(pt->getCoordinate());
            if (DimensionLocation::dimension(locDim) == Dimension::P) {
                ptList.push_back(pt);
            }
        }
        return ptList;
    }

    int TFHERelateGeometry::locateWithDim(const TFHECoordinate *p) {
        return getLocator()->locateWithDim(p);
    }

    Location TFHERelateGeometry::locateNode(const TFHECoordinate *p, const TFHEGeometry *parentPolygonal) {
        return getLocator()->locateNode(p, parentPolygonal);
    }

    bool TFHERelateGeometry::hasBoundary() {
        return getLocator()->hasBoundary();
    }

    bool TFHERelateGeometry::hasEdges() const {
        return hasLines || hasAreas;
    }

    bool TFHERelateGeometry::isEmpty() const {
        return isGeomEmpty;
    }

    int TFHERelateGeometry::locateLineEndWithDim(const TFHECoordinate *p) {
        return getLocator()->locateLineEndWithDim(p);
    }

    Location TFHERelateGeometry::locateAreaVertex(const TFHECoordinate *pt) {
        return locateNode(pt, nullptr);
    }
}  // namespace SpatialFHE::operation::relateng

// operation
// SpatialFHE