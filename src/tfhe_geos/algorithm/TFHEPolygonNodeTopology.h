//
// Created by ubuntu on 1/23/25.
//

#ifndef THFEPOLYGONNODETOPOLOGY_H
#define THFEPOLYGONNODETOPOLOGY_H
#include <TFHEInt32.h>

// Forward declarations
namespace SpatialFHE::geom {
    class TFHECoordinate;
}

namespace SpatialFHE::algorithm {

    /**
     * Functions to compute topological information
     * about nodes (ring intersections) in polygonal geometry.
     *
     * @author mdavis
     *
     */
    class TFHEPolygonNodeTopology {
        using TFHECoordinate = geom::TFHECoordinate;
    public:
        /*
         * Check if the segments at a node between two rings (or one ring) cross.
         * The node is topologically valid if the rings do not cross.
         * This function assumes that the segments are not collinear.
         *
         * @param nodePt the node location
         * @param a0 the previous segment endpoint in a ring
         * @param a1 the next segment endpoint in a ring
         * @param b0 the previous segment endpoint in the other ring
         * @param b1 the next segment endpoint in the other ring
         * @return true if the rings cross at the node
         */
        static bool isCrossing(
            const TFHECoordinate* nodePt,
            const TFHECoordinate* a0,
            const TFHECoordinate* a1,
            const TFHECoordinate* b0,
            const TFHECoordinate* b1);

        /**
         * Tests whether an segment node-b lies in the interior or exterior
         * of a corner of a ring formed by the two segments a0-node-a1.
         * The ring interior is assumed to be on the right of the corner
         * (i.e. a CW shell or CCW hole).
         * The test segment must not be collinear with the corner segments.
         *
         * @param nodePt the node location
         * @param a0 the first vertex of the corner
         * @param a1 the second vertex of the corner
         * @param b the other vertex of the test segment
         * @return true if the segment is interior to the ring corner
         */
        static bool isInteriorSegment(
            const TFHECoordinate* nodePt,
            const TFHECoordinate* a0,
            const TFHECoordinate* a1,
            const TFHECoordinate* b);

        /**
         * Compares the angles of two vectors
         * relative to the positive X-axis at their origin.
         * Angles increase CCW from the X-axis.
         *
         * @param origin the origin of the vectors
         * @param p the endpoint of the vector P
         * @param q the endpoint of the vector Q
         * @return a negative integer, zero, or a positive integer as this vector P has angle less than, equal to, or
         * greater than vector Q
         */
        static TFHEInt32 compareAngle(const TFHECoordinate* origin, const TFHECoordinate* p, const TFHECoordinate* q);

    private:
        /**
         * Tests if an edge p is between edges e0 and e1,
         * where the edges all originate at a common origin.
         * The "inside" of e0 and e1 is the arc which does not include the origin.
         * The edges are assumed to be distinct (non-collinear).
         *
         * @param origin the origin
         * @param p the destination point of edge p
         * @param e0 the destination point of edge e0
         * @param e1 the destination point of edge e1
         * @return true if p is between e0 and e1
         */
        static bool isBetween(
            const TFHECoordinate* origin,
            const TFHECoordinate* p,
            const TFHECoordinate* e0,
            const TFHECoordinate* e1);

        /**
         * Compares whether an edge p is between or outside the edges e0 and e1,
         * where the edges all originate at a common origin.
         * The "inside" of e0 and e1 is the arc which does not include
         * the positive X-axis at the origin.
         * If p is collinear with an edge 0 is returned.
         *
         * @param origin the origin
         * @param p the destination point of edge p
         * @param e0 the destination point of edge e0
         * @param e1 the destination point of edge e1
         * @return a negative integer, zero or positive integer as the vector P lies outside, collinear with, or inside
         * the vectors E0 and E1
         */
        static int compareBetween(
            const TFHECoordinate* origin,
            const TFHECoordinate* p,
            const TFHECoordinate* e0,
            const TFHECoordinate* e1);

        /**
         * Tests if the angle with the origin of a vector P is greater than that of the
         * vector Q.
         *
         * @param origin the origin of the vectors
         * @param p the endpoint of the vector P
         * @param q the endpoint of the vector Q
         * @return true if vector P has angle greater than Q
         */
        static bool isAngleGreater(const TFHECoordinate* origin, const TFHECoordinate* p, const TFHECoordinate* q);

        static int quadrant(const TFHECoordinate* origin, const TFHECoordinate* p);

    };

}  // namespace SpatialFHE::algorithm

#endif  // THFEPOLYGONNODETOPOLOGY_H
