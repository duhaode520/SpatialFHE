//
// Created by ubuntu on 1/12/25.
//

#ifndef TFHEPOLYGON_H
#define TFHEPOLYGON_H
#include "TFHEGeometry.h"
#include "TFHELinearRing.h"

namespace SpatialFHE::geom {

    class TFHEPolygon : public TFHEGeometry {
    protected:
        std::unique_ptr<TFHEEnvelope> computeEnvelopeInternal() const override;

        [[nodiscard]] TFHEPolygon* cloneInternal() const override;

        TFHEPolygon(const TFHEPolygon& other);

        TFHEPolygon(TFHELinearRing* p_shell, std::vector<TFHELinearRing*>* p_holes, const TFHEGeometryFactory* p_factory);
        TFHEPolygon(std::unique_ptr<TFHELinearRing> shell, const TFHEGeometryFactory& factory);
        TFHEPolygon(
            std::unique_ptr<TFHELinearRing> p_shell,
            std::vector<std::unique_ptr<TFHELinearRing>> p_holes,
            const TFHEGeometryFactory& p_factory);

        std::unique_ptr<TFHELinearRing> shell;
        std::vector<std::shared_ptr<TFHELinearRing>> holes;

    public:
        friend class TFHEGeometryFactory;

        bool isEmpty() const override;
        bool isRectangle() const override;

        const TFHECoordinate* getCoordinate() const override;
        std::unique_ptr<TFHECoordinateSequence> getCoordinates() const override;
        std::size_t getNumPoints() const override;

        std::string getGeometryType() const override;
        TFHEGeometryTypeId getGeometryTypeId() const override;

        Dimension::DimensionType getDimension() const override;
        std::unique_ptr<TFHEGeometry> getBoundary() const override;
        ~TFHEPolygon() override = default;

        std::unique_ptr<TFHEPolygon> clone() const;

        /// Returns the exterior ring
        const TFHELinearRing* getExteriorRing() const;

        std::size_t getNumInteriorRing() const;
        const TFHELinearRing* getInteriorRingN(std::size_t n) const;
    };

}  // namespace SpatialFHE::geom

// SpatialFHE

#endif  // TFHEPOLYGON_H
