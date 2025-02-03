//
// Created by ubuntu on 1/21/25.
//

#include "TFHEPointLocation.h"

#include "TFHEOrientation.h"
#include "TFHERayCrossingCounter.h"
#include "tfhe_geos/geom/TFHECoordinateSequence.h"
#include "tfhe_geos/geom/TFHEEnvelope.h"

namespace SpatialFHE::algorithm {
    TFHEBool TFHEPointLocation::isOnSegment(
        const geom::TFHECoordinate &p,
        const geom::TFHECoordinate &p0,
        const geom::TFHECoordinate &p1) {
        // ignore the envelope test cuz it's need decryption and might be slow

        // handle the case where p0 == p1
        if (p == p0) {
            return TFHEBool::tfhe_true;
        }
        TFHEBool isOnline = TFHEOrientation::index(p0, p1, p) == TFHEOrientation::COLLINEAR;
        return isOnline;
    }

    TFHEBool TFHEPointLocation::isOnLine(const geom::TFHECoordinate &p, const geom::TFHECoordinateSequence *line) {
        std::size_t pt_size = line->size();
        if (pt_size == 0) {
            return TFHEBool::tfhe_false;
        }

        TFHEBool res = TFHEBool::tfhe_false;
        for (std::size_t i = 1; i < pt_size; i++) {
            TFHEBool on_segment = isOnSegment(p, line->getAt(i - 1), line->getAt(i));
            if (TFHEBool::isLocalTrue(on_segment)) {
                return TFHEBool::tfhe_true;
            }
            res = res || on_segment;
        }
        return res;
    }

    geom::Location TFHEPointLocation::locateInRing(
        const geom::TFHECoordinate &p,
        const std::vector<const geom::TFHECoordinate *> &ring) {
        return TFHERayCrossingCounter::locatePointInRing(p, ring);
    }

    geom::Location TFHEPointLocation::locateInRing(
        const geom::TFHECoordinate &p,
        const geom::TFHECoordinateSequence &ring) {
        return TFHERayCrossingCounter::locatePointInRing(p, ring);
    }
}  // namespace SpatialFHE::algorithm

// SpatialFHE