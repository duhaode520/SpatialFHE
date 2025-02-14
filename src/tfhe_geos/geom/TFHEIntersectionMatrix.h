//
// Created by ubuntu on 1/8/25.
//

#ifndef TFHEINTERSECTIONMATRIX_H
#define TFHEINTERSECTIONMATRIX_H
#include <array>
#include <string>

#include "enums.h"

namespace SpatialFHE::geom {

    class TFHEIntersectionMatrix {
        // FIXME: Considering whether to use cipher text or plain text in IntersectionMatrix
    private:
        static const int firstDim;
        static const int secondDim;
        std::array<std::array<int, 3>, 3> matrix;

    public:
        TFHEIntersectionMatrix();
        /** \brief
         * Overridden constructor.
         *
         * Creates an IntersectionMatrix with the given dimension symbols.
         *
         * @param elements - reference to string containing pattern
         *                   of dimension values for elements.
         */
        TFHEIntersectionMatrix(const std::string& elements);
        /** \brief
         * Returns whether the elements of this IntersectionMatrix
         * satisfies the required dimension symbols.
         *
         * @param requiredDimensionSymbols - nine dimension symbols with
         *        which to compare the elements of this IntersectionMatrix.
         *        Possible values are {T, F, * , 0, 1, 2}.
         * @return true if this IntersectionMatrix matches the required
         *         dimension symbols.
         */
        bool matches(const std::string& requiredDimensionSymbols) const;

        /** \brief
         * Tests if given dimension value satisfies the dimension symbol.
         *
         * @param actualDimensionValue - valid dimension value stored in
         *        the IntersectionMatrix.
         *        Possible values are {TRUE, FALSE, DONTCARE, 0, 1, 2}.
         * @param requiredDimensionSymbol - a character used in the string
         *        representation of an IntersectionMatrix.
         *        Possible values are {T, F, * , 0, 1, 2}.
         * @return true if the dimension symbol encompasses the
         *         dimension value.
         */
        static bool matches(int actualDimensionValue, char requiredDimensionSymbol);

        /** \brief
         * Returns true if each of the actual dimension symbols satisfies
         * the corresponding required dimension symbol.
         *
         * @param actualDimensionSymbols - nine dimension symbols to validate.
         *        Possible values are {T, F, * , 0, 1, 2}.
         * @param requiredDimensionSymbols - nine dimension symbols to
         *        validate against.
         *        Possible values are {T, F, * , 0, 1, 2}.
         * @return true if each of the required dimension symbols encompass
         *         the corresponding actual dimension symbol.
         */
        static bool matches(const std::string& actualDimensionSymbols, const std::string& requiredDimensionSymbols);

        /** \brief
         * Changes the value of one of this IntersectionMatrixs elements.
         *
         * @param row - the row of this IntersectionMatrix, indicating
         *        the interior, boundary or exterior of the first Geometry.
         * @param column - the column of this IntersectionMatrix,
         *        indicating the interior, boundary or exterior of the
         *        second Geometry.
         * @param dimensionValue - the new value of the element.
         */
        void set(Location row, Location column, int dimension);

        /** \brief
         * Changes the elements of this IntersectionMatrix to the dimension
         * symbols in dimensionSymbols.
         *
         * @param dimensionSymbols - nine dimension symbols to which to
         *        set this IntersectionMatrix elements.
         *        Possible values are {T, F, * , 0, 1, 2}.
         */
        void set(const std::string& dimensionSymbols);

        /** \brief
         * Changes the elements of this IntersectionMatrix to dimensionValue.
         *
         * @param dimensionValue -
         *        the dimension value to which to set this
         *        IntersectionMatrix elements. Possible values {TRUE,
         *        FALSE, DONTCARE, 0, 1, 2}.
         */
        void setAll(int dimensionValue);

        /** \brief
         * Returns the value of one of this IntersectionMatrixs elements.
         *
         * @param row -
         *        the row of this IntersectionMatrix, indicating the
         *        interior, boundary or exterior of the first Geometry.
         *
         * @param column -
         *        the column of this IntersectionMatrix, indicating the
         *        interior, boundary or exterior of the second Geometry.
         *
         * @return the dimension value at the given matrix position.
         */
        int get(Location row, Location column) const {
            return matrix[static_cast<size_t>(row)][static_cast<size_t>(column)];
        }

        /** \brief
         * Returns true if this IntersectionMatrix is FF*FF****.
         *
         * @return true if the two Geometrys related by this
         *         IntersectionMatrix are disjoint.
         */
        bool isDisjoint() const;

        /** \brief
         * Returns true if isDisjoint returns false.
         *
         * @return true if the two Geometrys related by this
         *         IntersectionMatrix intersect.
         */
        bool isIntersects() const;

        /** \brief
         * Returns true if this IntersectionMatrix is FT*******, F**T*****
         * or F***T****.
         *
         * @param dimensionOfGeometryA - the dimension of the first Geometry.
         *
         * @param dimensionOfGeometryB - the dimension of the second Geometry.
         *
         * @return true if the two Geometry's related by this
         *         IntersectionMatrix touch, false if both Geometrys
         *         are points.
         */
        bool isTouches(int dimensionOfGeometryA, int dimensionOfGeometryB) const;

        /** \brief
         * Returns true if this IntersectionMatrix is:
         * - T*T****** (for a point and a curve, a point and an area or
         *   a line and an area)
         * - 0******** (for two curves)
         *
         * @param dimensionOfGeometryA - he dimension of the first Geometry.
         *
         * @param dimensionOfGeometryB - the dimension of the second Geometry.
         *
         * @return true if the two Geometry's related by this
         *         IntersectionMatrix cross.
         *
         * For this function to return true, the Geometrys must be a point
         * and a curve; a point and a surface; two curves; or a curve and
         * a surface.
         */
        bool isCrosses(int dimensionOfGeometryA, int dimensionOfGeometryB) const;

        /** \brief
         * Returns true if this IntersectionMatrix is T*F**F***.
         *
         * @return true if the first Geometry is within the second.
         */
        bool isWithin() const;

        /** \brief
         * Returns true if this IntersectionMatrix is T*****FF*.
         *
         * @return true if the first Geometry contains the second.
         */
        bool isContains() const;

        /** \brief
         * Returns true if this IntersectionMatrix is T*F**FFF*.
         *
         * @param dimensionOfGeometryA - he dimension of the first Geometry.
         * @param dimensionOfGeometryB - the dimension of the second Geometry.
         * @return true if the two Geometry's related by this
         *         IntersectionMatrix are equal; the Geometrys must have
         *         the same dimension for this function to return true
         */
        bool isEquals(int dimensionOfGeometryA, int dimensionOfGeometryB) const;

        /** \brief
         * Returns true if this IntersectionMatrix is:
         * - T*T***T** (for two points or two surfaces)
         * - 1*T***T** (for two curves)
         *
         * @param dimensionOfGeometryA - he dimension of the first Geometry.
         * @param dimensionOfGeometryB - the dimension of the second Geometry.
         * @return true if the two Geometry's related by this
         *         IntersectionMatrix overlap.
         *
         * For this function to return true, the Geometrys must be two points,
         * two curves or two surfaces.
         */
        bool isOverlaps(int dimensionOfGeometryA, int dimensionOfGeometryB) const;

        /** \brief
         * Returns true if this IntersectionMatrix is <code>T*****FF*</code>
         * or <code>*T****FF*</code> or <code>***T**FF*</code>
         * or <code>****T*FF*</code>
         *
         * @return <code>true</code> if the first Geometry covers the
         * second
         */
        bool isCovers() const;

        /** \brief
         * Returns true if this IntersectionMatrix is <code>T*F**F***</code>
         * <code>*TF**F***</code> or <code>**FT*F***</code>
         * or <code>**F*TF***</code>
         *
         * @return <code>true</code> if the first Geometry is covered by
         * the second
         */
        bool isCoveredBy() const;

        std::string toString() const;
    };

}  // namespace SpatialFHE::geom

// SpatialFHE

#endif  // TFHEINTERSECTIONMATRIX_H
