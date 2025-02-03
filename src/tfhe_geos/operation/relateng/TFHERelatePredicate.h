//
// Created by ubuntu on 1/14/25.
//

#ifndef TFHERELATEPREDICATE_H
#define TFHERELATEPREDICATE_H
#include <tfhe_geos/geom/TFHEEnvelope.h>

#include <memory>

#include "TFHEBasicPredicate.h"
#include "TFHEIMPredicate.h"
#include "TFHERelateGeometry.h"

namespace SpatialFHE::operation::relateng {

    class TFHERelatePredicate {
    public:
        /************************************************************************
         *
         * Creates a predicate to determine whether two geometries intersect.
         *
         * The intersects predicate has the following equivalent definitions:
         *
         *  * The two geometries have at least one point in common
         *  * The DE-9IM Intersection Matrix for the two geometries matches
         *    at least one of the patterns
         *
         *    [T********]
         *    [*T*******]
         *    [***T*****]
         *    [****T****]
         *
         *  disjoint() = false
         *  (intersects is the inverse of disjoint)
         *
         * @return the predicate instance
         *
         * @see disjoint()
         */

        class IntersectsPredicate : public TFHEBasicPredicate {
            std::string name() const override {
                return std::string("intersects");
            }

            bool requireSelfNoding() const override {
                //-- self-noding is not required to check for a simple interaction
                return false;
            }

            bool requireExteriorCheck(bool isSourceA) const override {
                (void)isSourceA;
                //-- intersects only requires testing interaction
                return false;
            }

            void init(const TFHEEnvelope& envA, const TFHEEnvelope& envB) override {
                require(envA.intersects(envB).decrypt());
            }

            void updateDimension(Location locA, Location locB, int dimension) override {
                (void)dimension;
                setValueIf(true, isIntersection(locA, locB));
            }

            void finish() override {
                //-- if no intersecting locations were found
                setValue(false);
            }
        };

        static std::unique_ptr<TFHEBasicPredicate> intersects();

        /************************************************************************
         *
         * Creates a predicate to determine whether two geometries are disjoint.
         *
         * The disjoint predicate has the following equivalent definitions:
         *
         *   * The two geometries have no point in common
         *   * The DE-9IM Intersection Matrix for the two geometries matches
         *        [FF*FF****]
         *   * intersects() = false
         *     (disjoint is the inverse of intersects)
         *
         * @return the predicate instance
         *
         * @see intersects()
         */
        class DisjointPredicate : public TFHEBasicPredicate {
            std::string name() const override {
                return std::string("disjoint");
            }

            bool requireSelfNoding() const override {
                //-- self-noding is not required to check for a simple interaction
                return false;
            }

            bool requireInteraction() const override {
                //-- ensure entire matrix is computed
                return false;
            }

            bool requireExteriorCheck(bool isSourceA) const override {
                (void)isSourceA;
                //-- intersects only requires testing interaction
                return false;
            }

            void init(const TFHEEnvelope& envA, const TFHEEnvelope& envB) override {
                setValueIf(true, envA.disjoint(envB).decrypt());
            }

            void updateDimension(Location locA, Location locB, int dimension) override {
                (void)dimension;
                setValueIf(false, isIntersection(locA, locB));
            }

            void finish() override {
                //-- if no intersecting locations were found
                setValue(true);
            }
        };

        static std::unique_ptr<TFHEBasicPredicate> disjoint();

        /************************************************************************
         * Creates a predicate to determine whether a geometry contains another geometry.
         *
         * The contains predicate has the following equivalent definitions:
         *
         *   * Every point of the other geometry is a point of this geometry,
         *     and the interiors of the two geometries have at least one point in common.
         *   * The DE-9IM Intersection Matrix for the two geometries matches
         *     the pattern
         *       [T*****FF*]
         *   * within(B, A) = true
         *     (contains is the converse of within)
         *
         * An implication of the definition is that "Geometries do not
         * contain their boundary".  In other words, if a geometry A is a subset of
         * the points in the boundary of a geometry B, B.contains(A) = false.
         * (As a concrete example, take A to be a LineString which lies in the boundary of a Polygon B.)
         * For a predicate with similar behavior but avoiding
         * this subtle limitation, see covers().
         *
         * @return the predicate instance
         *
         * @see within()
         */
        class ContainsPredicate : public TFHEIMPredicate {
            std::string name() const override {
                return std::string("contains");
            }

            bool requireCovers(bool isSourceA) override {
                return isSourceA == TFHERelateGeometry::GEOM_A;
            }

            bool requireExteriorCheck(bool isSourceA) const override {
                //-- only need to check B against Exterior of A
                return isSourceA == TFHERelateGeometry::GEOM_B;
            }

            void init(int _dimA, int _dimB) override {
                TFHEIMPredicate::init(_dimA, _dimB);
                require(isDimsCompatibleWithCovers(dimA, dimB));
            }

            void init(const TFHEEnvelope& envA, const TFHEEnvelope& envB) override {
                TFHEBasicPredicate::requireCovers(envA, envB);
            }

            bool isDetermined() const override {
                return intersectsExteriorOf(TFHERelateGeometry::GEOM_A);
            }

            bool valueIM() override {
                return intMatrix.isContains();
            }
        };

        static std::unique_ptr<TFHEIMPredicate> contains();

        /************************************************************************
         * Creates a predicate to determine whether a geometry is within another geometry.
         *
         * The within predicate has the following equivalent definitions:
         *
         *   * Every point of this geometry is a point of the other geometry,
         *     and the interiors of the two geometries have at least one point in common.
         *   * The DE-9IM Intersection Matrix for the two geometries matches
         *     [T*F**F***]
         *   *  contains(B, A) = true
         *      (within is the converse of  contains())
         *
         * An implication of the definition is that
         * "The boundary of a Geometry is not within the Geometry".
         * In other words, if a geometry A is a subset of
         * the points in the boundary of a geometry B, within(B, A) = false
         * (As a concrete example, take A to be a LineString which lies in the boundary of a Polygon B.)
         * For a predicate with similar behavior but avoiding
         * this subtle limitation, see coveredimBy().
         *
         * @return the predicate instance
         *
         * @see #contains()
         */
        class WithinPredicate : public TFHEIMPredicate {
            std::string name() const override {
                return std::string("within");
            }

            bool requireCovers(bool isSourceA) override {
                return isSourceA == TFHERelateGeometry::GEOM_B;
            }

            bool requireExteriorCheck(bool isSourceA) const override {
                //-- only need to check B against Exterior of A
                return isSourceA == TFHERelateGeometry::GEOM_A;
            }

            void init(int _dimA, int _dimB) override {
                TFHEIMPredicate::init(_dimA, _dimB);
                require(isDimsCompatibleWithCovers(dimB, dimA));
            }

            void init(const TFHEEnvelope& envA, const TFHEEnvelope& envB) override {
                TFHEBasicPredicate::requireCovers(envB, envA);
            }

            bool isDetermined() const override {
                return intersectsExteriorOf(TFHERelateGeometry::GEOM_B);
            }

            bool valueIM() override {
                return intMatrix.isWithin();
            }
        };

        static std::unique_ptr<TFHEIMPredicate> within();

        /************************************************************************
         * Creates a predicate to determine whether a geometry covers another geometry.
         *
         * The covers predicate has the following equivalent definitions:
         *
         * Every point of the other geometry is a point of this geometry.
         * The DE-9IM Intersection Matrix for the two geometries matches
         * at least one of the following patterns:
         *
         *  * [T*****FF*]
         *  * [*T****FF*]
         *  * [***T**FF*]
         *  * [****T*FF*]
         *
         * coveredimBy(b, a) = true
         * (covers is the converse of coveredimBy())
         *
         * If either geometry is empty, the value of this predicate is false.
         *
         * This predicate is similar to contains(),
         * but is more inclusive (i.e. returns true for more cases).
         * In particular, unlike contains it does not distinguish between
         * points in the boundary and in the interior of geometries.
         * For most cases, covers should be used in preference to contains.
         * As an added benefit, covers is more amenable to optimization,
         * and hence should be more performant.
         *
         * @return the predicate instance
         *
         * @see #coveredimBy()
         */

        class CoversPredicate : public TFHEIMPredicate {
            std::string name() const override {
                return std::string("covers");
            }

            bool requireCovers(bool isSourceA) override {
                return isSourceA == TFHERelateGeometry::GEOM_A;
            }

            bool requireExteriorCheck(bool isSourceA) const override {
                //-- only need to check B against Exterior of A
                return isSourceA == TFHERelateGeometry::GEOM_B;
            }

            void init(int _dimA, int _dimB) override {
                TFHEIMPredicate::init(_dimA, _dimB);
                require(isDimsCompatibleWithCovers(dimA, dimB));
            }

            void init(const TFHEEnvelope& envA, const TFHEEnvelope& envB) override {
                TFHEBasicPredicate::requireCovers(envA, envB);
            }

            bool isDetermined() const override {
                return intersectsExteriorOf(TFHERelateGeometry::GEOM_A);
            }

            bool valueIM() override {
                return intMatrix.isCovers();
            }
        };

        static std::unique_ptr<TFHEIMPredicate> covers();

        /************************************************************************
         * Creates a predicate to determine whether a geometry is covered
         * by another geometry.
         *
         * The coveredimBy predicate has the following equivalent definitions:
         *
         * Every point of this geometry is a point of the other geometry.
         * The DE-9IM Intersection Matrix for the two geometries matches
         * at least one of the following patterns:
         *
         *   [T*F**F***]
         *   [*TF**F***]
         *   [**FT*F***]
         *   [**F*TF***]
         *
         * covers(B, A) = true
         * (coveredimBy is the converse of covers())
         *
         * If either geometry is empty, the value of this predicate is false.
         *
         * This predicate is similar to within(),
         * but is more inclusive (i.e. returns true for more cases).
         *
         * @return the predicate instance
         *
         * @see #covers()
         */
        class CoveredByPredicate : public TFHEIMPredicate {
            std::string name() const override {
                return std::string("coveredBy");
            }

            bool requireCovers(bool isSourceA) override {
                return isSourceA == TFHERelateGeometry::GEOM_B;
            }

            bool requireExteriorCheck(bool isSourceA) const override {
                //-- only need to check B against Exterior of A
                return isSourceA == TFHERelateGeometry::GEOM_A;
            }

            void init(int _dimA, int _dimB) override {
                TFHEIMPredicate::init(_dimA, _dimB);
                require(isDimsCompatibleWithCovers(dimB, dimA));
            }

            void init(const TFHEEnvelope& envA, const TFHEEnvelope& envB) override {
                TFHEBasicPredicate::requireCovers(envB, envA);
            }

            bool isDetermined() const override {
                return intersectsExteriorOf(TFHERelateGeometry::GEOM_B);
            }

            bool valueIM() override {
                return intMatrix.isCoveredBy();
            }
        };

        static std::unique_ptr<TFHEIMPredicate> coveredBy();

        /************************************************************************
         * Creates a predicate to determine whether a geometry crosses another geometry.
         *
         * The crosses predicate has the following equivalent definitions:
         *
         * The geometries have some but not all interior points in common.
         * The DE-9IM Intersection Matrix for the two geometries matches
         * one of the following patterns:
         *
         *    [T*T******] (for P/L, P/A, and L/A cases)
         *    [T*****T**] (for L/P, A/P, and A/L cases)
         *    [0********] (for L/L cases)
         *
         *
         * For the A/A and P/P cases this predicate returns false.
         *
         * The SFS defined this predicate only for P/L, P/A, L/L, and L/A cases.
         * To make the relation symmetric
         * JTS extends the definition to apply to L/P, A/P and A/L cases as well.
         *
         * @return the predicate instance
         */
        class CrossesPredicate : public TFHEIMPredicate {
            std::string name() const override {
                return std::string("crosses");
            }

            void init(int _dimA, int _dimB) override {
                TFHEIMPredicate::init(_dimA, _dimB);
                bool isBothPointsOrAreas =
                    (dimA == Dimension::P && dimB == Dimension::P) || (dimA == Dimension::A && dimB == Dimension::A);
                require(!isBothPointsOrAreas);
            }

            bool isDetermined() const override {
                if (dimA == Dimension::L && dimB == Dimension::L) {
                    //-- L/L interaction can only be dim = P
                    if (getDimension(Location::INTERIOR, Location::INTERIOR) > Dimension::P)
                        return true;
                } else if (dimA < dimB) {
                    if (isIntersects(Location::INTERIOR, Location::INTERIOR) &&
                        isIntersects(Location::INTERIOR, Location::EXTERIOR)) {
                        return true;
                    }
                } else if (dimA > dimB) {
                    if (isIntersects(Location::INTERIOR, Location::INTERIOR) &&
                        isIntersects(Location::EXTERIOR, Location::INTERIOR)) {
                        return true;
                    }
                }
                return false;
            }

            bool valueIM() override {
                return intMatrix.isCrosses(dimA, dimB);
            }
        };

        static std::unique_ptr<TFHEIMPredicate> crosses();

        /************************************************************************
         * Creates a predicate to determine whether two geometries are
         * topologically equal.
         *
         * The equals predicate has the following equivalent definitions:
         *
         * The two geometries have at least one point in common,
         * and no point of either geometry lies in the exterior of the other geometry.
         * The DE-9IM Intersection Matrix for the two geometries matches
         * the pattern T*F**FFF*
         *
         * @return the predicate instance
         */
        class EqualsTopoPredicate : public TFHEIMPredicate {
            std::string name() const override {
                return std::string("equals");
            }

            bool requireInteraction() const override {
                //-- allow EMPTY = EMPTY
                return false;
            };

            void init(int _dimA, int _dimB) override {
                TFHEIMPredicate::init(_dimA, _dimB);
                //-- don't require equal dims, because EMPTY = EMPTY for all dims
            }

            void init(const TFHEEnvelope& envA, const TFHEEnvelope& envB) override {
                //-- handle EMPTY = EMPTY cases
                setValueIf(true, envA.isNull() && envB.isNull());

                require((envA == envB).decrypt());
            }

            bool isDetermined() const override {
                bool isEitherExteriorIntersects = isIntersects(Location::INTERIOR, Location::EXTERIOR) ||
                    isIntersects(Location::BOUNDARY, Location::EXTERIOR) ||
                    isIntersects(Location::EXTERIOR, Location::INTERIOR) ||
                    isIntersects(Location::EXTERIOR, Location::BOUNDARY);

                return isEitherExteriorIntersects;
            }

            bool valueIM() override {
                return intMatrix.isEquals(dimA, dimB);
            }
        };

        static std::unique_ptr<TFHEIMPredicate> equalsTopo();

        /************************************************************************
         * Creates a predicate to determine whether a geometry overlaps another geometry.
         *
         * The overlaps predicate has the following equivalent definitions:
         *
         * The geometries have at least one point each not shared by the other
         *     (or equivalently neither covers the other),
         *     they have the same dimension,
         *     and the intersection of the interiors of the two geometries has
         *     the same dimension as the geometries themselves.
         * The DE-9IM Intersection Matrix for the two geometries matches
         *     [T*T***T**] (for P/P and A/A cases)
         *     or [1*T***T**] (for L/L cases)
         *
         * If the geometries are of different dimension this predicate returns false.
         * This predicate is symmetric.
         *
         * @return the predicate instance
         */
        class OverlapsPredicate : public TFHEIMPredicate {
            std::string name() const override {
                return std::string("overlaps");
            }

            void init(int _dimA, int _dimB) override {
                TFHEIMPredicate::init(_dimA, _dimB);
                require(dimA == dimB);
            }

            bool isDetermined() const override {
                if (dimA == Dimension::A || dimA == Dimension::P) {
                    if (isIntersects(Location::INTERIOR, Location::INTERIOR) &&
                        isIntersects(Location::INTERIOR, Location::EXTERIOR) &&
                        isIntersects(Location::EXTERIOR, Location::INTERIOR))
                        return true;
                }
                if (dimA == Dimension::L) {
                    if (isDimension(Location::INTERIOR, Location::INTERIOR, Dimension::L) &&
                        isIntersects(Location::INTERIOR, Location::EXTERIOR) &&
                        isIntersects(Location::EXTERIOR, Location::INTERIOR))
                        return true;
                }
                return false;
            }

            bool valueIM() override {
                return intMatrix.isOverlaps(dimA, dimB);
            }
        };

        static std::unique_ptr<TFHEIMPredicate> overlaps();

        /************************************************************************
         * Creates a predicate to determine whether a geometry touches another geometry.
         *
         * The touches predicate has the following equivalent definitions:
         *
         * The geometries have at least one point in common,
         * but their interiors do not intersect.
         * The DE-9IM Intersection Matrix for the two geometries matches
         * at least one of the following patterns
         *
         *   [FT*******]
         *   [F**T*****]
         *   [F***T****]
         *
         *
         * If both geometries have dimension 0, the predicate returns false,
         * since points have only interiors.
         * This predicate is symmetric.
         *
         * @return the predicate instance
         */
        class TouchesPredicate : public TFHEIMPredicate {
            std::string name() const override {
                return std::string("touches");
            }

            void init(int _dimA, int _dimB) override {
                TFHEIMPredicate::init(_dimA, _dimB);
                bool isBothPoints = (dimA == 0 && dimB == 0);
                require(!isBothPoints);
            }

            bool isDetermined() const override {
                bool isInteriorsIntersects = isIntersects(Location::INTERIOR, Location::INTERIOR);
                return isInteriorsIntersects;
            }

            bool valueIM() override {
                return intMatrix.isTouches(dimA, dimB);
            }
        };

        static std::unique_ptr<TFHEIMPredicate> touches();

        static std::unique_ptr<TFHETopologyPredicate> matches(const std::string& pattern);
    };

}  // namespace SpatialFHE::operation::relateng

#endif  // TFHERELATEPREDICATE_H
