//
// Created by ubuntu on 1/23/25.
//

#include "TFHERelateEdge.h"

#include <tfhe_geos/algorithm/TFHEPolygonNodeTopology.h>

#include "TFHERelateNode.h"
#include "tfhe_geos/geom/enums.h"

using namespace SpatialFHE::geom;
using namespace SpatialFHE::algorithm;

namespace SpatialFHE::operation::relateng {
    void TFHERelateEdge::setLocationsArea(bool isA, bool isForward) {
        Location locLeft = isForward ? Location::EXTERIOR : Location::INTERIOR;
        Location locRight = isForward ? Location::INTERIOR : Location::EXTERIOR;
        if (isA) {
            aDim = 2;
            aLocLeft = locLeft;
            aLocRight = locRight;
            aLocLine = Location::BOUNDARY;
        } else {
            bDim = 2;
            bLocLeft = locLeft;
            bLocRight = locRight;
            bLocLine = Location::BOUNDARY;
        }
    }

    void TFHERelateEdge::setLocationsLine(bool isA) {
        if (isA) {
            aDim = 1;
            aLocLeft = Location::EXTERIOR;
            aLocRight = Location::EXTERIOR;
            aLocLine = Location::INTERIOR;
        } else {
            bDim = 1;
            bLocLeft = Location::EXTERIOR;
            bLocRight = Location::EXTERIOR;
            bLocLine = Location::INTERIOR;
        }
    }

    void TFHERelateEdge::setLocations(bool isA, Location locLeft, Location locRight, Location locLine) {
        if (isA) {
            aDim = 2;
            aLocLeft = locLeft;
            aLocRight = locRight;
            aLocLine = locLine;
        } else {
            bDim = 2;
            bLocLeft = locLeft;
            bLocRight = locRight;
            bLocLine = locLine;
        }
    }

    bool TFHERelateEdge::isKnown(bool isA) const {
        if (isA) {
            return aDim != DIM_UNKNOWN;
        }
        return bDim != DIM_UNKNOWN;
    }

    bool TFHERelateEdge::isKnown(bool isA, int pos) const {
        return location(isA, pos) != LOC_UNKNOWN;
    }

    int TFHERelateEdge::dimension(bool isA) const {
        return isA ? aDim : bDim;
    }

    void TFHERelateEdge::setDimension(bool isA, int dim) {
        if (isA) {
            aDim = dim;
        } else {
            bDim = dim;
        }
    }

    void TFHERelateEdge::setLeft(bool isA, Location loc) {
        if (isA) {
            aLocLeft = loc;
        } else {
            bLocLeft = loc;
        }
    }

    void TFHERelateEdge::setRight(bool isA, Location loc) {
        if (isA) {
            aLocRight = loc;
        } else {
            bLocRight = loc;
        }
    }

    void TFHERelateEdge::setOn(bool isA, Location loc) {
        if (isA) {
            aLocLine = loc;
        } else {
            bLocLine = loc;
        }
    }

    void TFHERelateEdge::mergeSideLocation(bool isA, int pos, Location loc) {
        Location currLoc = location(isA, pos);
        //-- INTERIOR takes precedence over EXTERIOR
        if (currLoc != Location::INTERIOR) {
            setLocation(isA, pos, loc);
        }
    }

    void TFHERelateEdge::mergeDimEdgeLoc(bool isA, Location locEdge) {
        int dim = (locEdge == Location::BOUNDARY) ? Dimension::A : Dimension::L;
        if (dim == Dimension::A && dimension(isA) == Dimension::L) {
            setDimension(isA, dim);
            setOn(isA, Location::BOUNDARY);
        }
    }

    TFHERelateEdge::TFHERelateEdge(const TFHERelateNode *node, const TFHECoordinate *pt, bool isA, bool isForward) :
            node(node), dirPt(pt) {
        setLocationsArea(isA, isForward);
    }

    TFHERelateEdge::TFHERelateEdge(const TFHERelateNode *node, const TFHECoordinate *pt, bool isA) :
            node(node), dirPt(pt) {
        setLocationsLine(isA);
    }

    TFHERelateEdge::TFHERelateEdge(
        const TFHERelateNode *node,
        const TFHECoordinate *pt,
        bool isA,
        Location locLeft,
        Location locRight,
        Location locLine) :
            node(node), dirPt(pt) {
        setLocations(isA, locLeft, locRight, locLine);
    }

    TFHERelateEdge *TFHERelateEdge::create(
        const TFHERelateNode *node,
        const TFHECoordinate *dirPt,
        bool isA,
        int dim,
        bool isForward) {
        if (dim == Dimension::A) {
            // return area edge
            return new TFHERelateEdge(node, dirPt, isA, isForward);
        }
        // return line edge
        return new TFHERelateEdge(node, dirPt, isA);
    }

    std::size_t TFHERelateEdge::findKnownEdgeIndex(std::vector<std::unique_ptr<TFHERelateEdge>> &edges, bool isA) {
        for (std::size_t i = 0; i < edges.size(); i++) {
            auto &edge = edges[i];
            if (edge->isKnown(isA)) {
                return i;
            }
        }
        return -1;
    }

    void TFHERelateEdge::setAreaInterior(std::vector<std::unique_ptr<TFHERelateEdge>> &edges, bool isA) {
        for (auto &edge : edges) {
            edge->setAreaInterior(isA);
        }
    }

    bool TFHERelateEdge::isInterior(bool isA, int position) const {
        return location(isA, position) == Location::INTERIOR;
    }

    TFHERelateEdge::Location TFHERelateEdge::location(bool isA, int position) const {
        if (isA) {
            switch (position) {
            case Position::LEFT:
                return aLocLeft;
            case Position::RIGHT:
                return aLocRight;
            case Position::ON:
                return aLocLine;
            }
        } else {
            switch (position) {
            case Position::LEFT:
                return bLocLeft;
            case Position::RIGHT:
                return bLocRight;
            case Position::ON:
                return bLocLine;
            }
        }
    }

    TFHEInt32 TFHERelateEdge::compareToEdge(const TFHECoordinate *edgeDirPt) const {
        return TFHEPolygonNodeTopology::compareAngle(node->getCoordinate(), dirPt, edgeDirPt);
    }

    void TFHERelateEdge::setDimLocations(bool isA, int dim, Location loc) {}

    void TFHERelateEdge::setAreaInterior(bool isA) {
        if (isA) {
            aLocLeft = Location::INTERIOR;
            aLocRight = Location::INTERIOR;
            aLocLine = Location::INTERIOR;
        } else {
            bLocLeft = Location::INTERIOR;
            bLocRight = Location::INTERIOR;
            bLocLine = Location::INTERIOR;
        }
    }

    void TFHERelateEdge::setLocation(bool isA, int pos, Location loc) {
        switch (pos) {
        case Position::LEFT:
            setLeft(isA, loc);
            break;
        case Position::RIGHT:
            setRight(isA, loc);
            break;
        case Position::ON:
            setOn(isA, loc);
            break;
        }
    }

    void TFHERelateEdge::setAllLocations(bool isA, Location loc) {
        setLeft(isA, loc);
        setRight(isA, loc);
        setOn(isA, loc);
    }

    void TFHERelateEdge::setUnknownLocations(bool isA, Location loc) {
        if (!isKnown(isA, Position::LEFT)) {
            setLocation(isA, Position::LEFT, loc);
        }
        if (!isKnown(isA, Position::RIGHT)) {
            setLocation(isA, Position::RIGHT, loc);
        }
        if (!isKnown(isA, Position::ON)) {
            setLocation(isA, Position::ON, loc);
        }
    }

    void TFHERelateEdge::merge(bool isA, int dim, bool isForward) {
        Location locEdge = Location::INTERIOR;
        Location locLeft = Location::EXTERIOR;
        Location locRight = Location::EXTERIOR;
        if (dim == Dimension::A) {
            locEdge = Location::BOUNDARY;
            locLeft = isForward ? Location::EXTERIOR : Location::INTERIOR;
            locRight = isForward ? Location::INTERIOR : Location::EXTERIOR;
        }

        if (!isKnown(isA)) {
            setDimension(isA, dim);
            setOn(isA, locEdge);
            setLeft(isA, locLeft);
            setRight(isA, locRight);
            return;
        }
    }
}  // namespace SpatialFHE::operation::relateng

// operation
// SpatialFHE