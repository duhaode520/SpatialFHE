//
// Created by ubuntu on 1/23/25.
//

#ifndef TFHERELATEEDGE_H
#define TFHERELATEEDGE_H
#include <TFHEInt32.h>

#include <memory>
#include <vector>

#include "tfhe_geos/geom/enums.h"

// Forward declaration
namespace SpatialFHE::operation::relateng {
    class TFHERelateNode;
}

namespace SpatialFHE::geom {
    class TFHECoordinate;
    class TFHEGeometry;
}  // namespace SpatialFHE::geom

namespace SpatialFHE::operation::relateng {

    class TFHERelateEdge {
        using TFHECoordinate = geom::TFHECoordinate;
        using TFHEGeometry = geom::TFHEGeometry;
        using Location = geom::Location;

    private:
        /**
         * Indicates that the location is currently unknown
         */
        static constexpr Location LOC_UNKNOWN = Location::NONE;

        // Members
        const TFHERelateNode* node;
        const TFHECoordinate* dirPt;

        int aDim = DIM_UNKNOWN;
        Location aLocLeft = LOC_UNKNOWN;
        Location aLocRight = LOC_UNKNOWN;
        Location aLocLine = LOC_UNKNOWN;

        int bDim = DIM_UNKNOWN;
        Location bLocLeft = LOC_UNKNOWN;
        Location bLocRight = LOC_UNKNOWN;
        Location bLocLine = LOC_UNKNOWN;

        // Methods

        void setLocationsArea(bool isA, bool isForward);
        void setLocationsLine(bool isA);
        void setLocations(bool isA, Location locLeft, Location locRight, Location locLine);

        bool isKnown(bool isA) const;
        bool isKnown(bool isA, int pos) const;
        int dimension(bool isA) const;

        void setDimension(bool isA, int dimension);
        void setLeft(bool isA, Location loc);
        void setRight(bool isA, Location loc);
        void setOn(bool isA, Location loc);

        void mergeSideLocation(bool isA, int pos, Location loc);

        /**
         * Area edges override Line edges.
         * Merging edges of same dimension is a no-op for
         * the dimension and on location.
         * But merging an area edge into a line edge
         * sets the dimension to A and the location to BOUNDARY.
         *
         * @param isA
         * @param locEdge
         */
        void mergeDimEdgeLoc(bool isA, Location locEdge);

    public:
        // Constants
        static constexpr bool IS_FORWARD = true;
        static constexpr bool IS_REVERSE = false;
        static constexpr int DIM_UNKNOWN = -1;

        // Constructors
        TFHERelateEdge(const TFHERelateNode* node, const TFHECoordinate* pt, bool isA, bool isForward);

        TFHERelateEdge(const TFHERelateNode* node, const TFHECoordinate* pt, bool isA);

        TFHERelateEdge(
            const TFHERelateNode* node,
            const TFHECoordinate* pt,
            bool isA,
            Location locLeft,
            Location locRight,
            Location locLine);

        // Methods
        static TFHERelateEdge* create(
            const TFHERelateNode* node,
            const TFHECoordinate* dirPt,
            bool isA,
            int dim,
            bool isForward);

        static std::size_t findKnownEdgeIndex(std::vector<std::unique_ptr<TFHERelateEdge>>& edges, bool isA);

        static void setAreaInterior(std::vector<std::unique_ptr<TFHERelateEdge>>& edges, bool isA);

        bool isInterior(bool isA, int position) const;

        Location location(bool isA, int position) const;

        TFHEInt32 compareToEdge(const TFHECoordinate* edgeDirPt) const;

        void setDimLocations(bool isA, int dim, Location loc);

        void setAreaInterior(bool isA);

        void setLocation(bool isA, int pos, Location loc);

        void setAllLocations(bool isA, Location loc);

        void setUnknownLocations(bool isA, Location loc);

        void merge(bool isA, int dim, bool isForward);
    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHERELATEEDGE_H
