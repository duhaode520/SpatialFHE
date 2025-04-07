//
// Created by ubuntu on 2/19/25.
//

#include "SineStarFactory.h"

#include <cmath>

#include "tfhe_geos/geom/TFHECoordinate.h"
#include "tfhe_geos/geom/TFHEEnvelope.h"
#include "tfhe_geos/geom/TFHEGeometryFactory.h"
#include "tfhe_geos/geom/TFHEPolygon.h"

using namespace SpatialFHE::geom;

namespace SpatialFHE::util {
    std::unique_ptr<TFHEPolygon> SineStarFactory::createSineStar() const {
        std::unique_ptr<TFHEEnvelope> env(dim.getEnvelope());
        double radius = env->getWidth().decrypt() / 2.0;

        double armRatio = armLengthRatio;
        if (armRatio < 0.0) {
            armRatio = 0.0;
        }
        if (armRatio > 1.0) {
            armRatio = 1.0;
        }

        double armMaxLen = armRatio * radius;
        double insideRadius = (1 - armRatio) * radius;

        double centreX = env->getMinX().decrypt() + radius;
        double centreY = env->getMinY().decrypt() + radius;

        std::vector<TFHECoordinate> pts(nPts + 1);
        // uint32_t iPt = 0;
        // for (uint32_t i = 0; i < nPts; i++) {
        //     // the fraction of the way thru the current arm - in [0,1]
        //     double ptArcFrac = (i / (double)nPts) * numArms;
        //     double armAngFrac = ptArcFrac - floor(ptArcFrac);

        //     // the angle for the current arm - in [0,2Pi]
        //     // (each arm is a complete sine wave cycle)
        //     double armAng = 2 * MATH_PI * armAngFrac;
        //     // the current length of the arm
        //     double armLenFrac = (cos(armAng) + 1.0) / 2.0;

        //     // the current radius of the curve (core + arm)
        //     double curveRadius = insideRadius + armMaxLen * armLenFrac;

        //     // the current angle of the curve
        //     double ang = i * (2 * MATH_PI / nPts);
        //     TFHEInt32 x = TFHEInt32(curveRadius * cos(ang) + centreX);
        //     TFHEInt32 y = TFHEInt32(curveRadius * sin(ang) + centreY);
        //     pts[iPt++] = coord(x, y);
        // }
        // pts[iPt] = pts[0];

        // Define max threads (could be a class member or parameter)
        uint32_t maxThreads = 2;
        if (maxThreads == 0)
            maxThreads = 4;                       // Fallback if hardware_concurrency returns 0
        maxThreads = std::min(maxThreads, nPts);  // Don't create more threads than points

        std::vector<std::future<void>> futures;
        futures.reserve(maxThreads);

        auto computePoints = [&](uint32_t startIdx, uint32_t endIdx) {
            for (uint32_t i = startIdx; i < endIdx; i++) {
                double ptArcFrac = (i / (double)nPts) * numArms;
                double armAngFrac = ptArcFrac - floor(ptArcFrac);

                double armAng = 2 * MATH_PI * armAngFrac;
                double armLenFrac = (cos(armAng) + 1.0) / 2.0;

                double curveRadius = insideRadius + armMaxLen * armLenFrac;

                double ang = i * (2 * MATH_PI / nPts);
                TFHEInt32::getContext()->setServerKey();
                TFHEInt32 x = TFHEInt32(curveRadius * cos(ang) + centreX);
                TFHEInt32 y = TFHEInt32(curveRadius * sin(ang) + centreY);
                pts[i] = coord(x, y);
            }
        };

        // Distribute points among threads
        uint32_t pointsPerThread = nPts / maxThreads;
        uint32_t remainingPoints = nPts % maxThreads;
        uint32_t startIdx = 0;

        for (uint32_t t = 0; t < maxThreads; t++) {
            uint32_t endIdx = startIdx + pointsPerThread + (t < remainingPoints ? 1 : 0);
            futures.push_back(std::async(std::launch::async, computePoints, startIdx, endIdx));
            startIdx = endIdx;
        }

        // Wait for all tasks to complete
        for (auto &f : futures) {
            f.get();
        }

        pts[nPts] = pts[0];

        auto cs = geomFact->getCoordinateSequenceFactory()->create(std::move(pts));
        auto ring = geomFact->createLinearRing(std::move(cs));
        auto poly = geomFact->createPolygon(std::move(ring));
        return poly;
    }

    std::unique_ptr<geom::TFHEPolygon> SineStarFactory::create(
        geom::TFHECoordinate &origin,
        double size,
        int nPts,
        int nArms,
        double armLengthRatio) {
        SineStarFactory gsf(TFHEGeometryFactory::getDefaultInstance());
        gsf.setCentre(origin);
        gsf.setSize(size);
        gsf.setNumPoints(nPts);
        gsf.setNumArms(nArms);
        gsf.setArmLengthRatio(armLengthRatio);
        return gsf.createSineStar();
    }

    std::shared_ptr<geom::TFHEPolygon> SineStarFactory::createShared(
        geom::TFHECoordinate &origin,
        double size,
        int nPts,
        int nArms,
        double armLengthRatio) {
        return std::shared_ptr<TFHEPolygon>(std::move(create(origin, size, nPts, nArms, armLengthRatio)));
    }

}  // namespace SpatialFHE::util

// SpatialFHE